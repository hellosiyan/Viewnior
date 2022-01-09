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

#ifndef __UNI_CACHE_H__
#define __UNI_CACHE_H__

#include <gdk/gdk.h>

typedef struct _UniPixbufDrawOpts UniPixbufDrawOpts;
typedef struct _UniPixbufDrawCache UniPixbufDrawCache;

typedef enum {
    UNI_PIXBUF_DRAW_METHOD_SCALE = 0,
    UNI_PIXBUF_DRAW_METHOD_CONTAINS = 1,
    UNI_PIXBUF_DRAW_METHOD_SCROLL = 2
} UniPixbufDrawMethod;

/**
 * UniPixbufDrawOpts:
 *
 * Struct which holds options for how the pixbuf should be
 * drawn. Options include such things like the source rectangle in the
 * pixbuf to draw, where to draw it, which zoom to use and so on.
 **/
struct _UniPixbufDrawOpts {
    gdouble zoom;

    /* Rectangle in zoom-space coordinates of the area to draw. */
    GdkRectangle zoom_rect;

    /* Position in widget-space coordinates where to draw. */
    int widget_x;
    int widget_y;

    GdkInterpType interp;
    GdkPixbuf *pixbuf;
};

/**
 * UniPixbufDrawCache:
 *
 * Cache that ensures fast redraws by storing the last draw
 * operation. For example, when resizing a #UniImageView, the view
 * receives an expose event and must redraw the damaged region. Unless
 * fitting is %TRUE, most of the pixels it should draw are indentical
 * to the ones drawn the previous time. Redrawing them is wasteful
 * because scaling and especially bilinear scaling is very
 * slow. Therefore, PixbufDrawCache objectifies the drawing process
 * and adds a cache with the last draw from which pixels can be
 * fetched.
 *
 * This object is present purely to ensure optimal speed. A
 * #GtkIImageTool that is asked to redraw a part of the image view
 * widget could either do it by itself using gdk_pixbuf_scale() and
 * gdk_draw_pixbuf().
 **/
struct _UniPixbufDrawCache {
    GdkPixbuf *last_pixbuf;
    UniPixbufDrawOpts old;
    int check_size;
};

UniPixbufDrawCache* uni_pixbuf_draw_cache_new   (void);
void    uni_pixbuf_draw_cache_free          (UniPixbufDrawCache * cache);
void    uni_pixbuf_draw_cache_invalidate    (UniPixbufDrawCache * cache);
void    uni_pixbuf_draw_cache_draw          (UniPixbufDrawCache * cache,
                                             UniPixbufDrawOpts * opts,
                                             GdkWindow * window);

UniPixbufDrawMethod uni_pixbuf_draw_cache_get_method (UniPixbufDrawOpts * old,
                                                      UniPixbufDrawOpts *
                                                      new_);

#endif /* __UNI_CACHE_H__ */
