/* Copyright (C) 2007 - Jesús Barbero <chuchiperriman@gmail.com>
 *  
 *  This file is part of gtksourcecompletion.
 *
 *  gtksourcecompletion is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  gtksourcecompletion is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __DOCUMENTWORDS_PROVIDER_H__
#define __DOCUMENTWORDS_PROVIDER_H__

#include <glib.h>
#include <glib-object.h>
#include "gsc-provider.h"

G_BEGIN_DECLS

#define GSC_TYPE_DOCUMENTWORDS_PROVIDER (gsc_documentwords_provider_get_type ())
#define GSC_DOCUMENTWORDS_PROVIDER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GSC_TYPE_DOCUMENTWORDS_PROVIDER, GscDocumentwordsProvider))
#define GSC_DOCUMENTWORDS_PROVIDER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GSC_TYPE_DOCUMENTWORDS_PROVIDER, GscDocumentwordsProviderClass))
#define GSC_IS_DOCUMENTWORDS_PROVIDER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GSC_TYPE_DOCUMENTWORDS_PROVIDER))
#define GSC_IS_DOCUMENTWORDS_PROVIDER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GSC_TYPE_DOCUMENTWORDS_PROVIDER))
#define GSC_DOCUMENTWORDS_PROVIDER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GSC_TYPE_DOCUMENTWORDS_PROVIDER, GscDocumentwordsProviderClass))

#define GSC_DOCUMENTWORDS_PROVIDER_NAME "GscDocumentwordsProvider"

typedef struct _GscDocumentwordsProvider GscDocumentwordsProvider;
typedef struct _GscDocumentwordsProviderClass GscDocumentwordsProviderClass;
typedef struct _GscDocumentwordsProviderPrivate GscDocumentwordsProviderPrivate;

struct _GscDocumentwordsProvider {
	GObject parent;
	GscDocumentwordsProviderPrivate *priv;	
};

struct _GscDocumentwordsProviderClass {
	GObjectClass parent;
};

/**
 * GscDocumentwordsProviderSortType:
 * @GSC_DOCUMENTWORDS_PROVIDER_SORT_NONE: Does not sort the proposals
 * @GSC_DOCUMENTWORDS_PROVIDER_SORT_BY_LENGTH: Sort the proposals by label 
 * lenght. Sets the small words first an large words last.
 **/
typedef enum{
	GSC_DOCUMENTWORDS_PROVIDER_SORT_NONE,
	GSC_DOCUMENTWORDS_PROVIDER_SORT_BY_LENGTH
} GscDocumentwordsProviderSortType;

/**
 * gsc_documentwords_provider_new:
 * @view: #GtkTextView where the provider must search for words
 * Returns The new #GscDocumentwordsProvider
 *
 */
GscDocumentwordsProvider* 
gsc_documentwords_provider_new(GtkTextView *view);

/**
 * gsc_documentwords_provider_set_sort_type:
 * @prov: The #GscDocumentwordsProvider
 * @sort_type: The #GscDocumentwordsProviderSortType for the completion proposal.
 *
 * This method sets the sort type for the completion proposals list.
 */
void
gsc_documentwords_provider_set_sort_type(GscDocumentwordsProvider *prov,
					 GscDocumentwordsProviderSortType sort_type);

/**
 * gsc_documentwords_provider_get_sort_type:
 * @prov: The #GscDocumentwordsProvider
 *
 * Returns The current sort type.
 */
GscDocumentwordsProviderSortType
gsc_documentwords_provider_get_sort_type(GscDocumentwordsProvider *prov);

GType 
gsc_documentwords_provider_get_type ();

G_END_DECLS

#endif

