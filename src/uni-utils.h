/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; coding: utf-8 -*-
 *
 * Copyright © 2007 Björn Lindqvist <bjourne@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */
#ifndef __UNI_UTILS_H__
#define __UNI_UTILS_H__

#include <gdk/gdk.h>

#define CHECK_SIZE  8
#define CHECK_LIGHT 0x00cccccc
#define CHECK_DARK  0x00808080

typedef struct {
    int width;
    int height;
} Size;

void    uni_pixbuf_scale_blend          (GdkPixbuf * src,
                                         GdkPixbuf * dst,
                                         int dst_x,
                                         int dst_y,
                                         int dst_width,
                                         int dst_height,
                                         gdouble offset_x,
                                         gdouble offset_y,
                                         gdouble zoom,
                                         GdkInterpType interp, int check_x, int check_y);

void    uni_draw_rect                   (GdkDrawable * drawable,
                                         GdkGC * gc, gboolean filled, GdkRectangle * rect);

void    uni_rectangle_get_rects_around  (GdkRectangle * outer,
                                         GdkRectangle * inner,
                                         GdkRectangle around[4]);

#endif /* __UNI_UTILS_H__ */
