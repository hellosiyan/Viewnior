/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; coding: utf-8 -*-
 *
 * Copyright © 2007-2009 Björn Lindqvist <bjourne@gmail.com>
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
