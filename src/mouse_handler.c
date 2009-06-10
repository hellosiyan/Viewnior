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

#include <stdlib.h>
#include "mouse_handler.h"

/**
 * mouse_handler_new:
 * @grab_cursor: Cursor to use when grabbing.
 * @returns: A new #MouseHandler
 *
 * Creates a new mouse handler. Note that the mouse handler never
 * owns, the passed in @grab_cursor -- you must always free it
 * yourself.
 **/
MouseHandler *
mouse_handler_new ()
{
    MouseHandler *mh = g_new0 (MouseHandler, 1);
    mh->pressed = FALSE;
    mh->dragging = FALSE;
    mh->drag_base_x = 0;
    mh->drag_base_y = 0;
    mh->drag_ofs_x = 0;
    mh->drag_ofs_y = 0;
    mh->grab_cursor = gdk_cursor_new (GDK_FLEUR);
    return mh;
}

static gboolean
mouse_handler_grab_pointer (MouseHandler * mh,
                            GdkWindow * window, guint32 time)
{
    int mask = (GDK_POINTER_MOTION_MASK
                | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_RELEASE_MASK);
    int retval = gdk_pointer_grab (window,
                                   FALSE,
                                   mask,
                                   NULL,
                                   mh->grab_cursor,
                                   time);
    return retval == GDK_GRAB_SUCCESS;

}

/**
 * mouse_handler_button_press:
 * @mh: a #MouseHandler
 * @ev: the #GdkEventButton event to handle.
 * @returns: %TRUE if the event was handled (left mouse button was
 *      pressed), %FALSE otherwise.
 *
 * Handles a button press event. The drag base and drag offset
 * is reset to the coordinate for the button event.
 **/
gboolean
mouse_handler_button_press (MouseHandler * mh, GdkEventButton * ev)
{

    mouse_handler_grab_pointer (mh, ev->window, ev->time);
    mh->pressed = TRUE;
    mh->drag_base_x = ev->x;
    mh->drag_base_y = ev->y;
    mh->drag_ofs_x = ev->x;
    mh->drag_ofs_y = ev->y;

    return TRUE;
}

gboolean
mouse_handler_button_release (MouseHandler * mh, GdkEventButton * ev)
{
    if (ev->button != 1)
        return FALSE;
    gdk_pointer_ungrab (ev->time);
    mh->pressed = FALSE;
    mh->dragging = FALSE;
    return TRUE;
}

void
mouse_handler_motion_notify (MouseHandler * mh, GdkEventMotion * ev)
{
    if (mh->pressed)
        mh->dragging = TRUE;

    mh->drag_ofs_x = ev->x;
    mh->drag_ofs_y = ev->y;
}

/**
 * mouse_handler_get_drag_delta:
 *
 * Sets @x and @y to the distance from where the mouse was pressed to
 * the mouses current position.
 **/
void
mouse_handler_get_drag_delta (MouseHandler * mh, int *x, int *y)
{
    *x = mh->drag_base_x - mh->drag_ofs_x;
    *y = mh->drag_base_y - mh->drag_ofs_y;
}
