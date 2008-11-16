/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 *  gsc-popup.h
 *
 *  Copyright (C) 2007 - Chuchiperriman <chuchiperriman@gmail.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef GSC_POPUP_H
#define GSC_POPUP_H

#include <gtk/gtk.h>
#include "gsc-proposal.h"

G_BEGIN_DECLS

/**
 * GscPopupPositionType:
 * @GSC_POPUP_POSITION_CURSOR: Places the popup up/down the GtkTextView cursor
 * @GSC_POPUP_POSITION_CENTER_SCREEN: Places the popup centered in the screen
 * @GSC_POPUP_POSITION_CENTER_PARENT: Places the popup centered in the popup 
 * parent (the parent window)
 **/
typedef enum{
	GSC_POPUP_POSITION_CURSOR,
	GSC_POPUP_POSITION_CENTER_SCREEN,
	GSC_POPUP_POSITION_CENTER_PARENT
}GscPopupPositionType;

/**
 * GscPopupFilterType:
 * @GSC_POPUP_FILTER_NONE: Show the popup without the filter entry
 * @GSC_POPUP_FILTER_TREE: Show the popup with the filter entry visible
 * @GSC_POPUP_FILTER_TREE_HIDDEN: Doesn't show the popup but you can set the 
 * filter text and filter the contents without showing the filter entry.
 * and set the focus on this entry.
 **/
typedef enum{
	GSC_POPUP_FILTER_NONE,
	GSC_POPUP_FILTER_TREE,
	GSC_POPUP_FILTER_TREE_HIDDEN
}GscPopupFilterType;

typedef enum {
	KEYS_INFO,
	KEYS_PAGE_NEXT,
	KEYS_PAGE_PREV,
	KEYS_LAST
}KeysType;

/*
 * Type checking and casting macros
 */
#define GSC_TYPE_POPUP              (gsc_popup_get_type())
#define GSC_POPUP(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), GSC_TYPE_POPUP, GscPopup))
#define GSC_POPUP_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), GSC_TYPE_POPUP, GscPopupClass))
#define GSC_IS_POPUP(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), GSC_TYPE_POPUP))
#define GSC_IS_POPUP_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), GSC_TYPE_POPUP))
#define GSC_POPUP_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), GSC_TYPE_POPUP, GscPopupClass))

#define DEFAULT_PAGE "Default"

typedef struct _GscPopupPriv GscPopupPriv;
typedef struct _GscPopup GscPopup;
typedef struct _GscPopupClass GscPopupClass;

struct _GscPopupClass
{
	GtkWindowClass parent_class;
  
	void (* proposal_selected)(GscPopup *popup,
				   GscProposal *proposal);
};

struct _GscPopup
{
  GtkWindow parent;
  
  GscPopupPriv *priv;

};

GType gsc_popup_get_type (void) G_GNUC_CONST;

/**
 * gsc_popup_select_first:
 * @self: The #GscPopup
 *
 * See #gsc_tree_select_first
 *
 * Returns
 */
gboolean
gsc_popup_select_first(GscPopup *self);

/**
 * gsc_popup_select_last:
 * @self: The #GscPopup
 *
 * See #gsc_tree_select_last
 *
 * Returns
 */
gboolean 
gsc_popup_select_last(GscPopup *self);

/**
 * gsc_popup_select_previous:
 * @self: The #GscPopup
 *
 * See #gsc_tree_select_previous
 *
 * Returns
 */
gboolean
gsc_popup_select_previous(GscPopup *self, 
			  gint rows);

/**
 * gsc_popup_select_next:
 * @self: The #GscPopup
 *
 * See #gsc_tree_select_next
 *
 * Returns
 */
gboolean
gsc_popup_select_next(GscPopup *self, 
		      gint rows);

/**
 * gsc_popup_get_selected_proposal:
 * @self: The #GscPopup
 *
 * See #gsc_tree_get_selected_proposal. Not free the proposal!
 *
 * Returns
 */
gboolean
gsc_popup_get_selected_proposal(GscPopup *self,
				GscProposal **proposal);

/**
 * gsc_popup_new:
 *
 * Returns The new #GscPopup
 */
GtkWidget*
gsc_popup_new ();

/**
 * gsc_popup_clear:
 * @self: The #GscPopup
 *
 * Clear all proposals in all pages. It frees all completion proposals.
 *
 */
void
gsc_popup_clear(GscPopup *self);

/**
 * gsc_popup_add_data:
 * @self: The #GscPopup
 * @data: The #GscProposal to add.
 *
 * The popup frees the proposal when it will be cleaned.
 *
 */
void
gsc_popup_add_data(GscPopup *self,
		   GscProposal* data);

/**
 * gsc_popup_has_proposals:
 * @self: The #GscPopup
 *
 * Returns TRUE if the popup has almost one element.
 */
gboolean
gsc_popup_has_proposals(GscPopup *self);

/**
 * gsc_popup_toggle_proposal_info:
 * @self: The #GscPopup
 *
 * This toggle the state of the info dialog. If the info is visible
 * then it hide the info dialog. If the dialog is hidden then it 
 * shows the info dialog.
 *
 */
void
gsc_popup_toggle_proposal_info(GscPopup *self);

/**
 * gsc_popup_page_next:
 * @self: The #GscPopup
 *
 * Shows the next completion page. If it is the last page then 
 * shows the first one.
 *
 */
void
gsc_popup_page_next(GscPopup *self);

/**
 * gsc_popup_page_previous:
 * @self: The #GscPopup
 *
 * Shows the previous completion page. If it is the first page then
 * shows the last one.
 *
 */
void
gsc_popup_page_previous(GscPopup *self);

/**
 * gsc_popup_set_current_info:
 * @self: The #GscPopup
 * @info: Markup with the info to be shown into the info window
 */
void
gsc_popup_set_current_info(GscPopup *self,
			   gchar *info);

/**
 * gsc_popup_get_num_active_pages:
 * @self: The #GscPopup
 *
 * Returns The number of active pages (pages with proposals)
 */
gint
gsc_popup_get_num_active_pages(GscPopup *self);

/**
 * gsc_popup_get_filter_widget:
 * @self: The #GscPopup
 *
 * This is the widget (Normally GtkEntry) where the user
 * write the filter. This function is used internally for the
 * completion components. You will not use this function, usually.
 *
 * Returns The internal filter widget
 */
GtkWidget*
gsc_popup_get_filter_widget(GscPopup *self);

/**
 * gsc_popup_set_filter_type:
 * @self: The #GscPopup
 * @filter_type: The filter type
 *
 * Sets the current filter type to be used when the popup will
 * be shown.
 *
 */
void
gsc_popup_set_filter_type(GscPopup *self,
			  GscPopupFilterType filter_type);

GscPopupFilterType
gsc_popup_get_filter_type(GscPopup *self);

/**
 * gsc_popup_set_filter_text:
 * @self: The #GscPopup
 * @text: The filter text
 *
 * Sets the filter text. This function refilter the completion proposals
 *
 */
void
gsc_popup_set_filter_text(GscPopup *self,
			  const gchar* text);

const gchar* 
gsc_popup_get_filter_text(GscPopup *self);

/**
 * gsc_popup_set_key:
 * @self: The #GscPopup
 * @type: Key what you want to set
 * @keys: Keys to be assigned to the key type
 *
 * Sets the keys for the key type. By example you can set the info keys 
 * by passing KEYS_INFO type and "<control>i" keys.
 *
 */
void
gsc_popup_set_key(GscPopup *self, KeysType type, const gchar* keys);

/**
 * gsc_popup_get_key:
 * @self: The #GscPopup
 * @type: The key type what you want to get
 *
 * Returns: New allocated string representation with #gtk_accelerator_name
 */
gchar*
gsc_popup_get_key(GscPopup *self, KeysType type);

/**
 * gsc_popup_manage_key:
 * @self: The #GscPopup
 * @event: Key event to be managed
 *
 * Manage the event keys. If it is Return, it will select the current proposal, 
 * if it is a Down key then selects the next proposal etc.
 *
 * Returns: TRUE if the key has been used.
 */
gboolean
gsc_popup_manage_key(GscPopup *self,
		     GdkEventKey *event);

/**
 * gsc_popup_show_or_update:
 * @widget: The #GscPopup
 *
 * Show the completion popup or update if it is visible because 
 * using gtk_widget_show only works if the popup is not visible.
 *
 */
void
gsc_popup_show_or_update(GtkWidget *widget);

void
gsc_popup_bottom_bar_set_visible(GscPopup *self, gboolean visible);

gboolean
gsc_popup_bottom_bar_get_visible(GscPopup *self);
			   
G_END_DECLS

#endif 
