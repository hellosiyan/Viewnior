/*
 * Copyright © 2009 Siyan Panayotov <xsisqox@gmail.com>
 *
 * This file is part of Viewnior.
 *
 * Viewnior is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Viewnior is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Viewnior.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __VNR_TOOLBAR_H__
#define __VNR_TOOLBAR_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include "vnr-window.h"

G_BEGIN_DECLS

typedef struct _VnrToolbar VnrToolbar;
typedef struct _VnrToolbarClass VnrToolbarClass;

#define VNR_TYPE_TOOLBAR             (vnr_toolbar_get_type ())
#define VNR_TOOLBAR(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), VNR_TYPE_TOOLBAR, VnrToolbar))
#define VNR_TOOLBAR_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass),  VNR_TYPE_TOOLBAR, VnrToolbarClass))
#define VNR_IS_TOOLBAR(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VNR_TYPE_TOOLBAR))
#define VNR_IS_TOOLBAR_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass),  VNR_TYPE_TOOLBAR))
#define VNR_TOOLBAR_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj),  VNR_TYPE_TOOLBAR, VnrToolbarClass))

struct _VnrToolbar {
    GtkToolbar parent;
};

struct _VnrToolbarClass {
    GtkToolbarClass parent_class;
};

GType       vnr_toolbar_get_type (void) G_GNUC_CONST;

GtkWidget*  vnr_toolbar_new      (void);

G_END_DECLS
#endif /* __VNR_TOOLBAR_H__ */

