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

/**
 * SECTION:gtkimagetooldragger
 * @see_also: #UniImageView, #GtkIImageTool, #GtkImageToolSelector
 * @short_description: Default image tool for paning the image
 *
 * <para>
 *   UniDragger is the default image tool for #UniImageView.
 *   Its only feature is that it can drag the image around.
 * </para>
 **/
#include <stdlib.h>
#include "uni-dragger.h"

/*************************************************************/
/***** Static stuff ******************************************/
/*************************************************************/
/**
 * uni_dragger_is_draggable:
 *
 * Returns %TRUE if the view can be dragged when the cursor is at the
 * position specified by @x and @y.
 **/
/*static gboolean
uni_dragger_is_draggable (UniDragger *dragger,
                                     int                  x,
                                     int                  y)
{
    UniImageView *view = dragger->view;
    GdkRectangle draw_rect;
    if (!uni_image_view_get_draw_rect (view, &draw_rect))
        return FALSE;
    
    gdouble zoom = uni_image_view_get_zoom (view);
    GdkPixbuf *pixbuf = uni_image_view_get_pixbuf (view);

    int pb_w = gdk_pixbuf_get_width (pixbuf);
    int pb_h = gdk_pixbuf_get_height (pixbuf);

    int zoom_w = (int) (pb_w * zoom + 0.5);
    int zoom_h = (int) (pb_h * zoom + 0.5);

    int alloc_w = GTK_WIDGET (view)->allocation.width;
    int alloc_h = GTK_WIDGET (view)->allocation.height;
    
    if (uni_rectangle_contains (draw_rect, x, y) &&
        (zoom_w > alloc_w || zoom_h > alloc_h))
        return TRUE;
    return FALSE;
}*/


G_DEFINE_TYPE (UniDragger, uni_dragger, G_TYPE_OBJECT);

/*************************************************************/
/***** Actions ***********************************************/
/*************************************************************/
/*static GdkCursor*
cursor_at_point (GtkIImageTool *tool,
                 int            x,
                 int            y)
{
    UniDragger *dragger = UNI_DRAGGER (tool);
    if (uni_dragger_is_draggable (dragger, x, y))
        return dragger->open_hand;
    return NULL;
}*/

gboolean
uni_dragger_button_press (UniDragger * tool, GdkEventButton * ev)
{
    /*if (!uni_dragger_is_draggable (dragger, ev->x, ev->y))
       return FALSE; */
    return mouse_handler_button_press (tool->mouse_handler, ev);
}

gboolean
uni_dragger_button_release (UniDragger * tool, GdkEventButton * ev)
{
    return mouse_handler_button_release (tool->mouse_handler, ev);
}

gboolean
uni_dragger_motion_notify (UniDragger * tool, GdkEventMotion * ev)
{
    MouseHandler *mouse_handler = tool->mouse_handler;

    mouse_handler_motion_notify (mouse_handler, ev);
    if (!mouse_handler->dragging)
        return FALSE;

    int dx, dy;
    mouse_handler_get_drag_delta (mouse_handler, &dx, &dy);
    if (abs (dx) < 1 && abs (dy) < 1)
        return FALSE;

    GdkRectangle viewport;
    uni_image_view_get_viewport ((UniImageView *) tool->view, &viewport);

    int offset_x = viewport.x + dx;
    int offset_y = viewport.y + dy;

    uni_image_view_set_offset ((UniImageView *) tool->view, offset_x,
                               offset_y, FALSE);

    mouse_handler->drag_base_x = mouse_handler->drag_ofs_x;
    mouse_handler->drag_base_y = mouse_handler->drag_ofs_y;

    return TRUE;
}

void
uni_dragger_pixbuf_changed (UniDragger * tool,
                            gboolean reset_fit, GdkRectangle * rect)
{
    uni_pixbuf_draw_cache_invalidate (tool->cache);
}

void
uni_dragger_paint_image (UniDragger * tool,
                         UniPixbufDrawOpts * opts, GdkDrawable * drawable)
{
    uni_pixbuf_draw_cache_draw (tool->cache, opts, drawable);
}

/*************************************************************/
/***** Stuff that deals with the type ************************/
/*************************************************************/
/* static void
gtk_iimage_tool_interface_init (gpointer g_iface,
                                gpointer iface_data)
{
    GtkIImageToolClass *klass = (GtkIImageToolClass *) g_iface;
    klass->button_press = button_press;
    klass->button_release = button_release;
    klass->motion_notify = motion_notify;
    klass->pixbuf_changed = pixbuf_changed;
    klass->paint_image = paint_image;
}*/

static void
uni_dragger_finalize (GObject * object)
{
    UniDragger *dragger = UNI_DRAGGER (object);
    gdk_cursor_unref (dragger->open_hand);
    gdk_cursor_unref (dragger->closed_hand);
    g_free (dragger->mouse_handler);
    uni_pixbuf_draw_cache_free (dragger->cache);

    /* Chain up */
    G_OBJECT_CLASS (uni_dragger_parent_class)->finalize (object);
}

enum {
    PROP_IMAGE_VIEW = 1
};

static void
uni_dragger_set_property (GObject * object,
                          guint prop_id,
                          const GValue * value, GParamSpec * pspec)
{
    UniDragger *dragger = UNI_DRAGGER (object);
    if (prop_id == PROP_IMAGE_VIEW)
        dragger->view = g_value_get_object (value);
    else
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
}

static void
uni_dragger_class_init (UniDraggerClass * klass)
{
    GObjectClass *object_class = (GObjectClass *) klass;
    object_class->finalize = uni_dragger_finalize;
    object_class->set_property = uni_dragger_set_property;

    GParamSpec *pspec = g_param_spec_object ("view",
                                             "Image View",
                                             "Image View to navigate",
                                             UNI_TYPE_IMAGE_VIEW,
                                             G_PARAM_CONSTRUCT_ONLY |
                                             G_PARAM_WRITABLE);
    g_object_class_install_property (object_class, PROP_IMAGE_VIEW, pspec);
}

static void
uni_dragger_init (UniDragger * tool)
{
    tool->open_hand = gdk_cursor_new (GDK_ARROW);
    tool->closed_hand = gdk_cursor_new (GDK_FLEUR);
    tool->mouse_handler = mouse_handler_new (tool->closed_hand);
    tool->view = NULL;
    tool->cache = uni_pixbuf_draw_cache_new ();
}

/**
 * uni_dragger_new:
 * @view: a #UniImageView
 * @returns: a new #UniDragger
 *
 * Creates and returns a new dragger tool.
 **/
UniDragger *
uni_dragger_new (GtkWidget * view)
{
    g_return_val_if_fail (view != NULL, NULL);
    UniDragger *data;

    data = UNI_DRAGGER (g_object_new (UNI_TYPE_DRAGGER, "view", view, NULL));

    return data;
}
