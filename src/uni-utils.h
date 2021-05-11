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

#ifndef __UNI_UTILS_H__
#define __UNI_UTILS_H__

#include <gdk/gdk.h>
#include "vnr-prefs.h"

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

void    uni_draw_rect                   (GdkWindow * window,
                                         GdkGC * gc, gboolean filled, GdkRectangle * rect);

void    uni_rectangle_get_rects_around  (GdkRectangle * outer,
                                         GdkRectangle * inner,
                                         GdkRectangle around[4]);

VnrPrefsDesktop uni_detect_desktop_environment ();

#endif /* __UNI_UTILS_H__ */
