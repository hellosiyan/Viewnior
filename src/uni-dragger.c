/*
 * Copyright © 2009-2010 Siyan Panayotov <xsisqox@gmail.com>
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

#include <math.h>
#include <stdlib.h>
#include "uni-dragger.h"
#include "uni-image-view.h"

G_DEFINE_TYPE (UniDragger, uni_dragger, G_TYPE_OBJECT);


/* Drag 'n Drop */
static GtkTargetEntry target_table[] = {
	{ "text/uri-list", 0, 0},
};

/*************************************************************/
/***** Actions ***********************************************/
/*************************************************************/

gboolean
uni_dragger_button_press (UniDragger * tool, GdkEventButton * ev)
{
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
        
    if ( pow(dx, 2) + pow(dy, 2) > 7 && 
    		UNI_IMAGE_VIEW(tool->view)->vadj->upper <= UNI_IMAGE_VIEW(tool->view)->vadj->page_size && 
    		UNI_IMAGE_VIEW(tool->view)->hadj->upper <= UNI_IMAGE_VIEW(tool->view)->hadj->page_size ) 
    {
		mouse_handler_button_release (mouse_handler, (GdkEventButton*)ev);
    	gtk_drag_begin (GTK_WIDGET(tool->view),
                gtk_target_list_new(target_table, G_N_ELEMENTS(target_table)),
                GDK_ACTION_COPY,
                1,
                (GdkEvent*)ev);
		return TRUE;
    }

    GdkRectangle viewport;
    uni_image_view_get_viewport (UNI_IMAGE_VIEW (tool->view), &viewport);

    int offset_x = viewport.x + dx;
    int offset_y = viewport.y + dy;

    uni_image_view_set_offset (UNI_IMAGE_VIEW (tool->view), offset_x,
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

static void
uni_dragger_finalize (GObject * object)
{
    UniDragger *dragger = UNI_DRAGGER (object);
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
    tool->mouse_handler = mouse_handler_new ();
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
