/*
 * Copyright © 2009 Siyan Panayotov <xsisqox@gmail.com>
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

#ifndef __UNI_IMAGE_VIEW_H__
#define __UNI_IMAGE_VIEW_H__

#include <gdk/gdk.h>
#include <gtk/gtk.h>

#include "mouse_handler.h"
#include "uni-dragger.h"

G_BEGIN_DECLS
#define UNI_TYPE_IMAGE_VIEW             (uni_image_view_get_type ())
#define UNI_IMAGE_VIEW(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), UNI_TYPE_IMAGE_VIEW, UniImageView))
#define UNI_IMAGE_VIEW_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), UNI_TYPE_IMAGE_VIEW, UniImageViewClass))
#define UNI_IS_IMAGE_VIEW(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), UNI_TYPE_IMAGE_VIEW))
#define UNI_IS_IMAGE_VIEW_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), UNI_TYPE_IMAGE_VIEW))
#define UNI_IMAGE_VIEW_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), UNI_TYPE_IMAGE_VIEW, UniImageViewClass))
typedef struct _UniImageView UniImageView;
typedef struct _UniImageViewClass UniImageViewClass;

/**
 * GtkImageTransp:
 *
 * This enum defines the valid transparency settings for how the image
 * view should draw transparent parts of alpha images. Their primary
 * use is as a value for the first parameter to the
 * uni_image_view_set_transp() method.
 *
 * Their interpretation is as follows:
 *
 *
 * GTK_IMAGE_TRANSP_COLOR : Use a single color.
 * GTK_IMAGE_TRANSP_BACKGROUND : Use the background color of the widget
 * GTK_IMAGE_TRANSP_GRID : Use light and dark gray checkerboard pattern.
 **/
typedef enum {
    GTK_IMAGE_TRANSP_COLOR = 0,
    GTK_IMAGE_TRANSP_BACKGROUND,
    GTK_IMAGE_TRANSP_GRID
} GtkImageTransp;

/**
 * UniImageView:
 *
 * UniImageView is the main class in the library. All of its fields
 * are private, they are only shown here for completeness. Use
 * uni_image_view_new() to instantiate UniImageView objects.
 **/
struct _UniImageView {
    GtkWidget parent;

    gboolean is_rendering;
    GdkInterpType interp;
    gboolean fitting;
    GdkPixbuf *pixbuf;
    gdouble zoom;
    /* Offset in zoom space coordinates of the image area in the
       widget. */
    gdouble offset_x;
    gdouble offset_y;
    gboolean show_cursor;
    GdkCursor *void_cursor;
    GtkAdjustment *hadj;
    GtkAdjustment *vadj;

    GObject *tool;

    GtkImageTransp transp;
};

struct _UniImageViewClass {
    GtkWidgetClass parent_class;

    /* Keybinding signals. */
    void (*set_zoom)    (UniImageView * view, gdouble zoom);
    void (*zoom_in)     (UniImageView * view);
    void (*zoom_out)    (UniImageView * view);

    void (*set_fitting) (UniImageView * view, gboolean fitting);
    void (*scroll)      (UniImageView * view,
                         GtkScrollType xscroll, GtkScrollType yscroll);

    /* Non-keybinding signals. */
    void (*set_scroll_adjustments) (UniImageView * view,
                                    GtkAdjustment * hadj,
                                    GtkAdjustment * vadj);

    void (*zoom_changed)            (UniImageView * view);
    void (*pixbuf_changed)          (UniImageView * view);
};

GType   uni_image_view_get_type (void) G_GNUC_CONST;

/* Constructors */
GtkWidget*  uni_image_view_new  (void);

/* Read-only properties */
gboolean    uni_image_view_get_viewport     (UniImageView * view,
                                             GdkRectangle * rect);

gboolean    uni_image_view_get_draw_rect    (UniImageView * view,
                                             GdkRectangle * rect);

/* Write-only properties */
void        uni_image_view_set_offset       (UniImageView * view,
                                             gdouble x, gdouble y,
                                             gboolean invalidate);

/* Read-write properties */
gboolean    uni_image_view_get_fitting  (UniImageView * view);
void        uni_image_view_set_fitting  (UniImageView * view,
                                         gboolean fitting);

GdkPixbuf*  uni_image_view_get_pixbuf   (UniImageView * view);
void        uni_image_view_set_pixbuf   (UniImageView * view,
                                         GdkPixbuf * pixbuf,
                                         gboolean reset_fit);

gdouble     uni_image_view_get_zoom     (UniImageView * view);
void        uni_image_view_set_zoom     (UniImageView * view, gdouble zoom);

/* Actions */
void        uni_image_view_zoom_in      (UniImageView * view);
void        uni_image_view_zoom_out     (UniImageView * view);
void        uni_image_view_damage_pixels(UniImageView * view,
                                         GdkRectangle * rect);

G_END_DECLS
#endif /* __UNI_IMAGE_VIEW_H__ */
