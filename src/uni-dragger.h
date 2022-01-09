/*
 * Copyright © 2009-2018 Siyan Panayotov <contact@siyanpanayotov.com>
 *
 * Based on code by (see README for details):
 * - Björn Lindqvist <bjourne@gmail.com>
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

#ifndef __UNI_TOOL_DRAGGER_H__
#define __UNI_TOOL_DRAGGER_H__

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include "uni-cache.h"

G_BEGIN_DECLS
#define UNI_TYPE_DRAGGER             (uni_dragger_get_type ())
#define UNI_DRAGGER(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), UNI_TYPE_DRAGGER, UniDragger))
#define UNI_DRAGGER_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), UNI_TYPE_DRAGGER, UniDraggerClass))
#define UNI_IS_DRAGGER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), UNI_TYPE_DRAGGER))
#define UNI_IS_DRAGGER_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), UNI_TYPE_DRAGGER))
#define UNI_DRAGGER_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), UNI_TYPE_DRAGGER, UniDraggerClass))
typedef struct _UniDragger UniDragger;
typedef struct _UniDraggerClass UniDraggerClass;

struct _UniDragger {
    GObject parent;
    GtkWidget *view;
    UniPixbufDrawCache *cache;
    
    gboolean pressed;
    gboolean dragging;

    /* Position where the mouse was pressed. */
    int drag_base_x;
    int drag_base_y;

    /* Current position of the mouse. */
    int drag_ofs_x;
    int drag_ofs_y;
    
    
    /* Cursor to use when grabbing. */
    GdkCursor *grab_cursor;
};

struct _UniDraggerClass {
    GObjectClass parent;
};

GType       uni_dragger_get_type    (void) G_GNUC_CONST;

/* Constructors */
UniDragger* uni_dragger_new         (GtkWidget * view);

/* Actions */
gboolean    uni_dragger_button_press    (UniDragger * tool,
                                         GdkEventButton * ev);


gboolean    uni_dragger_button_release  (UniDragger * tool,
                                         GdkEventButton * ev);

gboolean    uni_dragger_motion_notify   (UniDragger * tool,
                                         GdkEventMotion * ev);


void    uni_dragger_pixbuf_changed      (UniDragger * tool,
                                         gboolean reset_fit,
                                         GdkRectangle * rect);


void    uni_dragger_paint_image         (UniDragger * tool,
                                         UniPixbufDrawOpts * opts,
                                         GdkWindow * window);

G_END_DECLS
#endif /* __UNI_TOOL_DRAGGER_H__ */
