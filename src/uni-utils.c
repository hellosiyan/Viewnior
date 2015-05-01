/*
 * Copyright © 2009-2015 Siyan Panayotov <contact@siyanpanayotov.com>
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

#include "uni-utils.h"

/**
 * uni_pixbuf_scale_blend:
 *
 * A utility function that either scales or composites color depending
 * on the number of channels in the source image. The last four
 * parameters are only used in the composite color case.
 **/
void
uni_pixbuf_scale_blend (GdkPixbuf * src,
                        GdkPixbuf * dst,
                        int dst_x,
                        int dst_y,
                        int dst_width,
                        int dst_height,
                        gdouble offset_x,
                        gdouble offset_y,
                        gdouble zoom,
                        GdkInterpType interp, int check_x, int check_y)
{
    if (gdk_pixbuf_get_has_alpha (src))
        gdk_pixbuf_composite_color (src, dst,
                                    dst_x, dst_y, dst_width, dst_height,
                                    offset_x, offset_y,
                                    zoom, zoom,
                                    interp,
                                    255,
                                    check_x, check_y,
                                    CHECK_SIZE, CHECK_LIGHT, CHECK_DARK);
    else
        gdk_pixbuf_scale (src, dst,
                          dst_x, dst_y, dst_width, dst_height,
                          offset_x, offset_y, zoom, zoom, interp);
}

/**
 * uni_draw_rect:
 *
 * This function is a fixed version of gdk_draw_rectangle. The GDK
 * function is broken in that drawing a the rectangle (0,0)-[0,0] will
 * draw a pixel at position (0,0).
 **/
void
uni_draw_rect (GdkDrawable * drawable,
               GdkGC * gc, gboolean filled, GdkRectangle * rect)
{
    if (rect->width <= 0 || rect->height <= 0)
        return;
    gdk_draw_rectangle (drawable, gc, filled,
                        rect->x, rect->y, rect->width - 1, rect->height - 1);
}

void
uni_rectangle_get_rects_around (GdkRectangle * outer,
                                GdkRectangle * inner, GdkRectangle around[4])
{
    /* Top */
    around[0] = (GdkRectangle)
    {
    outer->x, outer->y, outer->width, inner->y - outer->y};
    /* Left */
    around[1] = (GdkRectangle)
    {
    outer->x, inner->y, inner->x - outer->x, inner->height};
    /* Right */
    around[2] = (GdkRectangle)
    {
    inner->x + inner->width,
            inner->y,
            (outer->x + outer->width) - (inner->x + inner->width),
            inner->height};
    /* Bottom */
    around[3] = (GdkRectangle)
    {
    outer->x,
            inner->y + inner->height,
            outer->width,
            (outer->y + outer->height) - (inner->y + inner->height)};
}
