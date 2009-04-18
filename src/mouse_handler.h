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
#ifndef __MOUSE_HANDLER_H__
#define __MOUSE_HANDLER_H__

#include <gdk/gdk.h>

#include "uni-utils.h"

typedef struct _MouseHandler MouseHandler;

struct _MouseHandler {
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

MouseHandler*   mouse_handler_new   (GdkCursor * grab_cursor);
gboolean        mouse_handler_button_press      (MouseHandler * mh,
                                                 GdkEventButton * ev);

gboolean        mouse_handler_button_release    (MouseHandler * mh,
                                                 GdkEventButton * ev);

void    mouse_handler_motion_notify     (MouseHandler * mh,
                                         GdkEventMotion * ev);

void    mouse_handler_get_drag_delta    (MouseHandler * mh, int *x, int *y);

#endif /* __MOUSE_HANDLER_H__ */
