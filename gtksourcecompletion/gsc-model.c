/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*- 
 * gscmodel.c
 * This file is part of gsc
 *
 * Copyright (C) 2009 - Jesse van den Kieboom
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, 
 * Boston, MA  02110-1301  USA
 */

#include "gsc-model.h"

#define ITEMS_PER_CALLBACK 100

#define GSC_COMPLETION_MODEL_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE((object), GSC_TYPE_MODEL, GscModelPrivate))

typedef struct
{
	GscModel *model;

	GscProvider *provider;
	GscProposal *proposal;
	
	gulong changed_id;
} ProposalNode;

typedef struct
{
	GList *item;
	GHashTable *hash;
	guint num;
} HeaderInfo;

typedef struct
{
	GscModel *model;
	GHashTable *proposals;
	GtkTreePath *path;
} RemoveInfo;
	

struct _GscModelPrivate
{
	GType column_types[GSC_COMPLETION_MODEL_N_COLUMNS];
	GList *store;
	GList *last;
	
	guint num;
	GHashTable *num_per_provider;
	
	guint idle_id;
	GQueue *item_queue;
};

/* Signals */
enum
{
	ITEMS_ADDED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

static void tree_model_iface_init (gpointer g_iface, gpointer iface_data);

G_DEFINE_TYPE_WITH_CODE (GscModel, 
                         gsc_completion_model, 
                         G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (GTK_TYPE_TREE_MODEL,
                                                tree_model_iface_init))


/* Interface implementation */
static ProposalNode *
node_from_iter (GtkTreeIter *iter)
{
	return (ProposalNode *)(((GList *)iter->user_data)->data);
}

static GtkTreePath *
path_from_list (GscModel *model,
                GList                    *item)
{
	gint index = 0;

	g_assert (GTK_IS_PROPOSAL (((ProposalNode*)item->data)->proposal));
	index = g_list_position (model->priv->store, item);
	
	if (index == -1)
	{
		return NULL;
	}
	else
	{
		return gtk_tree_path_new_from_indices (index, -1);
	}
}

static GtkTreeModelFlags
tree_model_get_flags (GtkTreeModel *tree_model)
{
	g_return_val_if_fail (GSC_IS_MODEL (tree_model), 0);

	return 0;
}

static gint
tree_model_get_n_columns (GtkTreeModel *tree_model)
{
	g_return_val_if_fail (GSC_IS_MODEL (tree_model), 0);

	return GSC_COMPLETION_MODEL_N_COLUMNS;
}

static GType
tree_model_get_column_type (GtkTreeModel *tree_model,
			    gint          index)
{
	g_return_val_if_fail (GSC_IS_MODEL (tree_model), G_TYPE_INVALID);
	g_return_val_if_fail (index >= 0 && index < GSC_COMPLETION_MODEL_N_COLUMNS, G_TYPE_INVALID);

	return GSC_COMPLETION_MODEL (tree_model)->priv->column_types[index];
}

static gboolean
get_iter_from_index (GscModel *model,
                     GtkTreeIter              *iter,
                     gint                      index)
{
	GList *item;

	if (index < 0 || index >= model->priv->num)
	{
		return FALSE;
	}
	
	item = model->priv->store;
	
	item = g_list_nth (item, index);
	
	if (item != NULL)
	{
		iter->user_data = item;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

static gboolean
tree_model_get_iter (GtkTreeModel *tree_model,
		     GtkTreeIter  *iter, 
		     GtkTreePath  *path)
{
	GscModel *model;
	gint *indices;
	
	g_return_val_if_fail (GSC_IS_MODEL (tree_model), FALSE);
	g_return_val_if_fail (iter != NULL, FALSE);
	g_return_val_if_fail (path != NULL, FALSE);
	
	model = GSC_COMPLETION_MODEL (tree_model);
	indices = gtk_tree_path_get_indices (path);
	
	return get_iter_from_index (model, iter, indices[0]);
}

static GtkTreePath *
tree_model_get_path (GtkTreeModel *tree_model,
		     GtkTreeIter  *iter)
{
	GscModel *model;
	
	g_return_val_if_fail (GSC_IS_MODEL (tree_model), NULL);
	g_return_val_if_fail (iter != NULL, NULL);
	g_return_val_if_fail (iter->user_data != NULL, NULL);

	model = GSC_COMPLETION_MODEL (tree_model);
	return path_from_list (model, (GList *)iter->user_data);
}

static void
tree_model_get_value (GtkTreeModel *tree_model,
		      GtkTreeIter  *iter, 
		      gint          column,
		      GValue       *value)
{
	ProposalNode *node;

	g_return_if_fail (GSC_IS_MODEL (tree_model));
	g_return_if_fail (iter != NULL);
	g_return_if_fail (iter->user_data != NULL);
	g_return_if_fail (column >= 0 && column < GSC_COMPLETION_MODEL_N_COLUMNS);

	node = node_from_iter (iter);

	g_value_init (value, GSC_COMPLETION_MODEL (tree_model)->priv->column_types[column]);

	switch (column)
	{
		case GSC_COMPLETION_MODEL_COLUMN_PROVIDER:
			g_value_set_object (value, node->provider);
			break;
		case GSC_COMPLETION_MODEL_COLUMN_PROPOSAL:
			g_value_set_object (value, node->proposal);
			break;
		case GSC_COMPLETION_MODEL_COLUMN_LABEL:
			g_value_set_string (value, gsc_completion_proposal_get_label (node->proposal));
			break;
		case GSC_COMPLETION_MODEL_COLUMN_MARKUP:
			g_value_set_string (value, gsc_completion_proposal_get_markup (node->proposal));
			break;
		case GSC_COMPLETION_MODEL_COLUMN_ICON:
			if (node->proposal == NULL)
			{
				g_value_set_object (value, 
				                    (gpointer)gsc_completion_provider_get_icon (
				                    	node->provider));
			}
			else
			{
				g_value_set_object (value, 
				                    (gpointer)gsc_completion_proposal_get_icon (
				                    	node->proposal));
			}
			break;
	}
}

static gboolean
get_next_element (GList *item,
		  GtkTreeIter *iter)
{
	item = g_list_next (item);
	
	if (item != NULL)
	{
		iter->user_data = item;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

static gboolean
tree_model_iter_next (GtkTreeModel *tree_model,
		      GtkTreeIter  *iter)
{
	g_return_val_if_fail (GSC_IS_MODEL (tree_model), FALSE);
	g_return_val_if_fail (iter != NULL, FALSE);
	
	return get_next_element ((GList *)iter->user_data, iter);
}

static gboolean
tree_model_iter_children (GtkTreeModel *tree_model,
			  GtkTreeIter  *iter,
			  GtkTreeIter  *parent)
{
	g_return_val_if_fail (GSC_IS_MODEL (tree_model), FALSE);
	g_return_val_if_fail (iter != NULL, FALSE);
	g_return_val_if_fail (parent == NULL || parent->user_data != NULL, FALSE);
	
	/* FIXME: not sure if this should be: (GList *)iter->user_data, iter */
	//return get_next_element (GSC_COMPLETION_MODEL (tree_model)->priv->store, iter);
	return get_next_element ((GList *)iter->user_data, iter);
}

static gboolean
tree_model_iter_has_child (GtkTreeModel *tree_model,
			   GtkTreeIter  *iter)
{
	g_return_val_if_fail (GSC_IS_MODEL (tree_model), FALSE);
	g_return_val_if_fail (iter != NULL, FALSE);
	
	return FALSE;
}

static gint
tree_model_iter_n_children (GtkTreeModel *tree_model,
			    GtkTreeIter  *iter)
{
	g_return_val_if_fail (GSC_IS_MODEL (tree_model), 0);
	g_return_val_if_fail (iter == NULL || iter->user_data != NULL, 0);
	
	if (iter == NULL)
	{
		return GSC_COMPLETION_MODEL (tree_model)->priv->num;
	}
	else
	{
		return 0;
	}
}

static gboolean
tree_model_iter_nth_child (GtkTreeModel *tree_model,
			   GtkTreeIter  *iter,
			   GtkTreeIter  *parent, 
			   gint          n)
{
	g_return_val_if_fail (GSC_IS_MODEL (tree_model), FALSE);
	g_return_val_if_fail (iter != NULL, FALSE);
	g_return_val_if_fail (parent == NULL || parent->user_data != NULL, FALSE);

	if (parent != NULL)
	{
		return FALSE;
	}
	else
	{
		return get_iter_from_index (GSC_COMPLETION_MODEL (tree_model), 
		                            iter,
		                            n);
	}
}

static gboolean
tree_model_iter_parent (GtkTreeModel *tree_model,
			GtkTreeIter  *iter,
			GtkTreeIter  *child)
{
	g_return_val_if_fail (GSC_IS_MODEL (tree_model), FALSE);
	g_return_val_if_fail (iter != NULL, FALSE);
	g_return_val_if_fail (child != NULL, FALSE);
	
	iter->user_data = NULL;
	return FALSE;
}

static void
tree_model_row_inserted (GtkTreeModel *tree_model,
			 GtkTreePath  *path,
			 GtkTreeIter  *iter)
{
}

static void
tree_model_row_deleted (GtkTreeModel *tree_model,
			 GtkTreePath  *path)
{
}

static void
tree_model_iface_init (gpointer g_iface, 
                       gpointer iface_data)
{
	GtkTreeModelIface *iface = (GtkTreeModelIface *)g_iface;

	iface->get_flags = tree_model_get_flags;
	iface->get_n_columns = tree_model_get_n_columns;
	iface->get_column_type = tree_model_get_column_type;
	iface->get_iter = tree_model_get_iter;
	iface->get_path = tree_model_get_path;
	iface->get_value = tree_model_get_value;
	iface->iter_next = tree_model_iter_next;
	iface->iter_children = tree_model_iter_children;
	iface->iter_has_child = tree_model_iter_has_child;
	iface->iter_n_children = tree_model_iter_n_children;
	iface->iter_nth_child = tree_model_iter_nth_child;
	iface->iter_parent = tree_model_iter_parent;
	
	iface->row_inserted = tree_model_row_inserted;
	iface->row_deleted = tree_model_row_deleted;
}

static void
free_node (ProposalNode *node)
{
	g_object_unref (node->provider);
	
	if (node->proposal != NULL)
	{
		if (node->changed_id != 0)
		{
			g_signal_handler_disconnect (node->proposal,
						     node->changed_id);
		}
		g_object_unref (node->proposal);
	}
	
	g_slice_free (ProposalNode, node);
}

static void
gsc_completion_model_dispose (GObject *object)
{
	GscModel *model = GSC_COMPLETION_MODEL (object);

	gsc_completion_model_cancel_add_proposals (model);
	
	if (model->priv->item_queue != NULL)
	{
		g_queue_free (model->priv->item_queue);
		model->priv->item_queue = NULL;
	}
	
	if (model->priv->num_per_provider != NULL)
	{
		g_hash_table_destroy (model->priv->num_per_provider);
		model->priv->num_per_provider = NULL;
	}
	
	g_list_foreach (model->priv->store, (GFunc)free_node, NULL);
	g_list_free (model->priv->store);
	model->priv->store = NULL;

	G_OBJECT_CLASS (gsc_completion_model_parent_class)->dispose (object);
}

static void
gsc_completion_model_finalize (GObject *object)
{
	G_OBJECT_CLASS (gsc_completion_model_parent_class)->finalize (object);
}

static void
gsc_completion_model_class_init (GscModelClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	
	object_class->finalize = gsc_completion_model_finalize;
	object_class->dispose = gsc_completion_model_dispose;

	g_type_class_add_private (object_class, sizeof(GscModelPrivate));
	
	signals[ITEMS_ADDED] =
		g_signal_new ("items-added",
			      G_TYPE_FROM_CLASS (klass),
			      G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
			      G_STRUCT_OFFSET (GscModelClass, items_added),
			      NULL, 
			      NULL,
			      g_cclosure_marshal_VOID__VOID, 
			      G_TYPE_NONE,
			      0);
}

static void
free_num (gpointer data)
{
	HeaderInfo *info = (HeaderInfo *)data;

	g_hash_table_destroy (info->hash);
	g_slice_free (HeaderInfo, data);
}

static gboolean
compare_nodes (gconstpointer a,
	       gconstpointer b)
{
	ProposalNode *p1 = (ProposalNode *)a;
	ProposalNode *p2 = (ProposalNode *)b;
	return gsc_completion_proposal_equals (p1->proposal, p2->proposal);
}

static gboolean
compare_proposals (gconstpointer a,
		   gconstpointer b)
{
	GscProposal *p1 = GSC_COMPLETION_PROPOSAL (a);
	GscProposal *p2 = GSC_COMPLETION_PROPOSAL (b);
	return gsc_completion_proposal_equals (p1, p2);
}

static guint
hash_node (gconstpointer v)
{
	ProposalNode *node = (ProposalNode *)v;
	
	return gsc_completion_proposal_get_hash (node->proposal);
}

static void
gsc_completion_model_init (GscModel *self)
{
	self->priv = GSC_COMPLETION_MODEL_GET_PRIVATE (self);
	
	self->priv->column_types[GSC_COMPLETION_MODEL_COLUMN_PROVIDER] = G_TYPE_OBJECT;
	self->priv->column_types[GSC_COMPLETION_MODEL_COLUMN_PROPOSAL] = G_TYPE_OBJECT;
	self->priv->column_types[GSC_COMPLETION_MODEL_COLUMN_LABEL] = G_TYPE_STRING;
	self->priv->column_types[GSC_COMPLETION_MODEL_COLUMN_MARKUP] = G_TYPE_STRING;
	self->priv->column_types[GSC_COMPLETION_MODEL_COLUMN_ICON] = GDK_TYPE_PIXBUF;
	
	self->priv->num_per_provider = g_hash_table_new_full (g_direct_hash,
	                                                      g_direct_equal,
	                                                      NULL,
	                                                      free_num);
	
	self->priv->idle_id = 0;
	self->priv->item_queue = g_queue_new ();
	g_queue_init (self->priv->item_queue);
}

static void
num_inc (GscModel    *model,
         GscProvider *provider,
         gboolean                     header)
{
	HeaderInfo *info;
	
	info = g_hash_table_lookup (model->priv->num_per_provider, provider);
	
	++model->priv->num;
	
	if (info != NULL && !header)
	{
		++(info->num);
	}
}

static void
num_dec (GscModel    *model,
         GscProvider *provider,
         gboolean                     header)
{
	HeaderInfo *info;
	
	info = g_hash_table_lookup (model->priv->num_per_provider, provider);
	
	--model->priv->num;
		
	if (info != NULL && !header)
	{
		--(info->num);
	}
}

/* Public */
GscModel*
gsc_completion_model_new (void)
{
	return g_object_new (GSC_TYPE_MODEL, NULL);
}

static GList *
append_list (GscModel *model,
             HeaderInfo               *info,
             ProposalNode             *node,
             gboolean                 *inserted)
{
	GList *item = NULL;
	
	g_assert (info != NULL);
	g_assert (node != NULL);

	item = g_hash_table_lookup (info->hash, node);
	
	if (item == NULL)
	{
		item = g_list_append (model->priv->last, node);
		
		if (model->priv->store == NULL)
		{
			model->priv->store = item;
		}
		else
		{
			item = item->next;
		}
		
		g_hash_table_insert (info->hash, node, item);
		
		*inserted = TRUE;
		model->priv->last = item;
	}
	else
	{
		/*g_hash_table_replace (model->priv->hash_store, node, item);
		free_node (item->data);
		item->data = node;*/
		*inserted = FALSE;
	}

	return item;
}

static void
remove_node (GscModel	*model,
	     ProposalNode 		*node,
	     GList 			*store_node,
	     GtkTreePath 		*path)
{
	g_assert (store_node != NULL);
	g_assert (path != NULL);

	num_dec (model, node->provider, node->proposal == NULL);
	free_node (node);
	
	if (store_node == model->priv->last)
	{
		model->priv->last = store_node->prev;
	}

	model->priv->store = g_list_delete_link (model->priv->store,store_node);
		
	if (model->priv->store == NULL)
	{
		model->priv->last = NULL;
	}
		
	gtk_tree_model_row_deleted (GTK_TREE_MODEL (model), path);
}

static void
on_proposal_changed (GscProposal *proposal,
                     GList                       *item)
{
	GtkTreeIter iter;
	ProposalNode *node = (ProposalNode *)item->data;
	GtkTreePath *path;

	iter.user_data = node;
	path = path_from_list (node->model, item);

	gtk_tree_model_row_changed (GTK_TREE_MODEL (node->model),
	                            path,
	                            &iter);
	gtk_tree_path_free (path);
}

static gboolean
idle_append (gpointer data)
{
	GscModel *model = GSC_COMPLETION_MODEL (data);
	HeaderInfo *info;
	GtkTreePath *path;
	GList *item;
	gint i = 0;

	while (i < ITEMS_PER_CALLBACK)
	{
		ProposalNode *node = (ProposalNode *)g_queue_pop_head (model->priv->item_queue);
		GtkTreeIter iter;
		gboolean inserted;
		
		if (node == NULL)
		{
			/* If we are here we added all elements of the queue */
			g_signal_emit (model, signals[ITEMS_ADDED], 0);
			
			model->priv->idle_id = 0;
			
			return FALSE;
		}
		
		info = g_hash_table_lookup (model->priv->num_per_provider, node->provider);
		
		if (info == NULL)
		{
			info = g_slice_new (HeaderInfo);
			info->item = model->priv->last;
			info->num = 0;
			info->hash = g_hash_table_new (hash_node,
						       compare_nodes);
			
			g_hash_table_insert (model->priv->num_per_provider, node->provider, info);
		}
		
		item = append_list (model, info, node, &inserted);
		
		if (inserted)
		{
			iter.user_data = item;

			num_inc (model, node->provider, FALSE);

			path = path_from_list (model, item);
			gtk_tree_model_row_inserted (GTK_TREE_MODEL (model), path, &iter);
			gtk_tree_path_free (path);
		}
		
		node->changed_id = g_signal_connect (node->proposal, 
	                                             "changed", 
	                                             G_CALLBACK (on_proposal_changed),
	                                             item);
	
		i++;
	}

	return TRUE;
}

void
gsc_completion_model_run_add_proposals (GscModel *model)
{
	if (idle_append (model))
	{
		model->priv->idle_id =
			g_idle_add ((GSourceFunc)idle_append,
				    model);
	}
}

static gboolean
remove_old_proposals (gpointer key,
		      gpointer value,
		      gpointer user_data)
{
	ProposalNode *node = (ProposalNode *)key;
	RemoveInfo *rinfo = (RemoveInfo *)user_data;
	GList *store_node = (GList *)value;
	GList *item;
	if (rinfo->proposals)
	{
	  //item = g_list_find_custom (rinfo->proposals, node, compare_proposal_node);
		item = g_hash_table_lookup (rinfo->proposals, node->proposal);
		if (!item)
		{
			remove_node (rinfo->model, node, store_node, rinfo->path);
			return TRUE;
		}
	}
	else
	{
		remove_node (rinfo->model, node, store_node, rinfo->path);
		return TRUE;
	}

	return FALSE;
}


void
gsc_completion_model_set_proposals (GscModel	    *model,
					   GscProvider 	    *provider,
					   GList		       	    *proposals)
{
	GList *item = NULL;
	HeaderInfo *info;
	GscProposal *proposal;
	RemoveInfo rinfo;

	g_return_if_fail (GSC_IS_MODEL (model));
	g_return_if_fail (GSC_IS_PROVIDER (provider));

	info = g_hash_table_lookup (model->priv->num_per_provider, provider);
	if (info)
	{
		rinfo.path = gtk_tree_path_new_first ();
		rinfo.model = model;
		if (proposals)
		{
			rinfo.proposals = g_hash_table_new ((GHashFunc)gsc_completion_proposal_get_hash,
							    compare_proposals);
			for (item = proposals; item; item = g_list_next (item))
			{
				g_hash_table_insert (rinfo.proposals, item->data, item);
			}
		}
		else
		{
			rinfo.proposals = NULL;
		}
		
		g_hash_table_foreach_remove (info->hash, remove_old_proposals, &rinfo);

		if (rinfo.proposals)
			g_hash_table_destroy (rinfo.proposals);
		
		gtk_tree_path_free (rinfo.path);
	}

	for (item = proposals; item; item = g_list_next (item))
	{
		if (GTK_IS_PROPOSAL (item->data))
		{
			proposal = GSC_COMPLETION_PROPOSAL (item->data);
			gsc_completion_model_append (model,
							    provider,
							    proposal);
		}
	}
	
}

void
gsc_completion_model_append (GscModel    *model,
                                    GscProvider *provider,
                                    GscProposal *proposal)
{
	ProposalNode *node;
	
	g_return_if_fail (GSC_IS_MODEL (model));
	g_return_if_fail (GSC_IS_PROVIDER (provider));
	g_return_if_fail (GTK_IS_PROPOSAL (proposal));
	
	node = g_slice_new (ProposalNode);
	node->provider = g_object_ref (provider);
	node->proposal = g_object_ref (proposal);
	node->changed_id = 0;

	g_queue_push_tail (model->priv->item_queue, node);
}

void
gsc_completion_model_cancel_add_proposals (GscModel    *model)
{
	g_return_if_fail (GSC_IS_MODEL (model));

	if (model->priv->idle_id != 0)
	{
		g_source_remove (model->priv->idle_id);
		model->priv->idle_id = 0;
	}
	
	if (model->priv->item_queue != NULL)
	{
		g_queue_foreach (model->priv->item_queue,
				 (GFunc)free_node, NULL);
		g_queue_clear (model->priv->item_queue);
	}
}

void
gsc_completion_model_clear (GscModel *model)
{
	GtkTreePath *path;
	ProposalNode *node;

	g_return_if_fail (GSC_IS_MODEL (model));

	/* Clear the queue of missing elements to append */
	gsc_completion_model_cancel_add_proposals (model);
	
	path = gtk_tree_path_new_first ();
	
	while (model->priv->store)
	{
		node = (ProposalNode *)model->priv->store->data;
		remove_node (model, node, model->priv->store, path);
	}
	
	gtk_tree_path_free (path);
	
	g_hash_table_remove_all (model->priv->num_per_provider);
}

gboolean
gsc_completion_model_is_empty (GscModel *model,
                                      gboolean                  invisible)
{
	g_return_val_if_fail (GSC_IS_MODEL (model), FALSE);
	
	if (invisible)
	{
		return model->priv->num == 0;
	}
	else
	{
		return model->priv->num == 0 || 
		       gtk_tree_model_iter_n_children (GTK_TREE_MODEL (model), NULL) == 0;
	}
}

guint
gsc_completion_model_n_proposals (GscModel    *model,
                                         GscProvider *provider)
{
	HeaderInfo *info;
	
	g_return_val_if_fail (GSC_IS_MODEL (model), 0);
	g_return_val_if_fail (GSC_IS_PROVIDER (provider), 0);
	
	info = g_hash_table_lookup (model->priv->num_per_provider, provider);
	
	if (info == NULL)
	{
		return 0;
	}
	else
	{
		return info->num;
	}
}

gboolean
gsc_completion_model_iter_previous (GscModel *model,
                                           GtkTreeIter              *iter)
{
	GList *item;
	
	g_return_val_if_fail (GSC_IS_MODEL (model), FALSE);
	g_return_val_if_fail (iter != NULL, FALSE);
	g_return_val_if_fail (iter->user_data != NULL, FALSE);
	
	item = iter->user_data;
	
	item = g_list_previous (item);
	
	if (item != NULL)
	{
		iter->user_data = item;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

gboolean
gsc_completion_model_iter_last (GscModel *model,
                                       GtkTreeIter              *iter)
{
	GList *item;
	
	g_return_val_if_fail (GSC_IS_MODEL (model), FALSE);
	g_return_val_if_fail (iter != NULL, FALSE);
	
	item = model->priv->last;
	iter->user_data = item;

	if (item != NULL)
	{
		return gsc_completion_model_iter_previous (model, iter);
	}
	else
	{
		return FALSE;
	}
}
