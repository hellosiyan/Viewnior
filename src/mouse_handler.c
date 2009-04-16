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
mouse_handler_new (GdkCursor * grab_cursor)
{
    MouseHandler *mh = g_new0 (MouseHandler, 1);
    mh->pressed = FALSE;
    mh->dragging = FALSE;
    mh->drag_base_x = 0;
    mh->drag_base_y = 0;
    mh->drag_ofs_x = 0;
    mh->drag_ofs_y = 0;
    mh->grab_cursor = grab_cursor;
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
 * Handles a button press event. If left mouse button is pressed an
 * attempt to grab the pointer is made. The drag base and drag offset
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

    /*if(ev->type == GDK_2BUTTON_PRESS && ev->button == 1){
       printf("ZOOM!\n");
       }else if(ev->type == GDK_BUTTON_PRESS){
       if(ev->button == 1){
       mouse_handler_grab_pointer (mh, ev->window, ev->time);
       mh->pressed = TRUE;
       mh->drag_base_x = ev->x;
       mh->drag_base_y = ev->y;
       mh->drag_ofs_x = ev->x;
       mh->drag_ofs_y = ev->y;
       }else if(ev->button == 3){
       printf("Menu here!\n");                      
       }
       } */


    /*switch(ev->button){
       case 1:
       mouse_handler_grab_pointer (mh, ev->window, ev->time);
       mh->pressed = TRUE;
       mh->drag_base_x = ev->x;
       mh->drag_base_y = ev->y;
       mh->drag_ofs_x = ev->x;
       mh->drag_ofs_y = ev->y;
       break;
       case 2:
       break;
       case 3:
       printf("Menu here!\n");
       } */

    /* if (ev->type==GDK_2BUTTON_PRESS ||
       ev->type==GDK_3BUTTON_PRESS ) {
       printf("I feel %s clicked with button %d\n",
       ev->type==GDK_2BUTTON_PRESS ? "double" : "triple",
       ev->button);
       } */

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
