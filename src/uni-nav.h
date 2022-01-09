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

#ifndef __UNI_NAV_H__
#define __UNI_NAV_H__
/**
 * #UniNav is a popup window that shows a map of a
 * #UniImageView. The user can then zoom around in the area.
 **/

#include <gdk/gdk.h>
#include <gtk/gtk.h>

#include "uni-image-view.h"

G_BEGIN_DECLS

#define UNI_TYPE_NAV                (uni_nav_get_type ())
#define UNI_NAV(obj)                (G_TYPE_CHECK_INSTANCE_CAST ((obj), UNI_TYPE_NAV, UniNav))
#define UNI_NAV_CLASS(klass)        (G_TYPE_CHECK_CLASS_CAST ((klass), UNI_TYPE_NAV, UniNavClass))
#define UNI_IS_NAV(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), UNI_TYPE_NAV))
#define UNI_IS_NAV_CLASS(klass)     (G_TYPE_CHECK_CLASS_TYPE ((klass), UNI_TYPE_NAV))
#define UNI_NAV_GET_CLASS(obj)      (G_TYPE_CHECK_INSTANCE_GET_CLASS ((obj), UNI_TYPE_NAV, UniNavClass))
/**
 * UNI_NAV_MAX_WIDTH:
 *
 * The maximum width of the drawing area in the widget.
 **/
#define UNI_NAV_MAX_WIDTH             150
/**
 * UNI_NAV_MAX_HEIGHT:
 *
 * The maximum height of the drawing area in the widget.
 **/
#define UNI_NAV_MAX_HEIGHT          150
typedef struct _UniNav UniNav;
typedef struct _UniNavClass UniNavClass;

struct _UniNav {
    GtkWindow parent;

    /* A GtkDrawingArea to draw the image in. */
    GtkWidget *preview;

    /* The UniImageView that is navigated. */
    UniImageView *view;

    /* A downsampled version of the UniImageView's pixbuf to display. */
    GdkPixbuf *pixbuf;

    /* The last drawn XOR rectangle. */
    GdkRectangle last_rect;

    /* Center coordinate of where UniNav is mapped. */
    int center_x;
    int center_y;

    /* To draw the preview square. */
    GdkGC *gc;

    /* A flag indicating whether the pixbuf needs to be recreated when
       the navigator is shown. */
    gboolean update_when_shown;
};

struct _UniNavClass {
    GtkWindowClass parent_class;
};

GType       uni_nav_get_type        (void) G_GNUC_CONST;

/* Constructors */
GtkWidget*  uni_nav_new             (UniImageView * view);

/* Actions */
void        uni_nav_grab            (UniNav * nav);
void        uni_nav_release         (UniNav * nav);

/* Runner function */
void        uni_nav_show_and_grab   (UniNav * nav,
                                     int center_x, int center_y);

G_END_DECLS
#endif /* __UNI_NAV_H__ */
