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

#include "uni-nav.h"
#include "uni-utils.h"


G_DEFINE_TYPE (UniNav, uni_nav, GTK_TYPE_WINDOW);

/*************************************************************/
/***** Static stuff ******************************************/
/*************************************************************/
static gdouble
uni_nav_get_zoom (UniNav * nav)
{
    GdkPixbuf *pixbuf = uni_image_view_get_pixbuf (nav->view);

    /* If there is no image, we can't get it's width and height */
    if (!pixbuf)
    {
        return 0.0;
    }

    int img_width = gdk_pixbuf_get_width (pixbuf);
    int img_height = gdk_pixbuf_get_height (pixbuf);

    gdouble width_zoom = (gdouble) UNI_NAV_MAX_WIDTH / (gdouble) img_width;
    gdouble height_zoom = (gdouble) UNI_NAV_MAX_HEIGHT / (gdouble) img_height;
    return MIN (width_zoom, height_zoom);
}


static Size
uni_nav_get_preview_size (UniNav * nav)
{
    GdkPixbuf *pixbuf = uni_image_view_get_pixbuf (nav->view);
    if (!pixbuf)
        return (Size)
    {
    UNI_NAV_MAX_WIDTH, UNI_NAV_MAX_HEIGHT};
    int img_width = gdk_pixbuf_get_width (pixbuf);
    int img_height = gdk_pixbuf_get_height (pixbuf);

    gdouble zoom = uni_nav_get_zoom (nav);

    Size s;
    s.width = (int) (img_width * zoom + 0.5);
    s.height = (int) (img_height * zoom + 0.5);
    return s;
}

static gdouble
uni_nav_get_zoom2nav_factor (UniNav * nav)
{
    gdouble nav_zoom = uni_nav_get_zoom (nav);
    gdouble view_zoom = nav->view->zoom;
    return view_zoom / nav_zoom;
}

static GdkRectangle
gtk_image_get_current_rectangle (UniNav * nav)
{
    GdkRectangle rect = {0, 0, 0, 0};
    uni_image_view_get_viewport (nav->view, &rect);

    /* Convert Zoom space to Nav space coordinates. */
    gdouble zoom2nav_factor = uni_nav_get_zoom2nav_factor (nav);
    rect.x = (gdouble) rect.x / zoom2nav_factor;
    rect.y = (gdouble) rect.y / zoom2nav_factor;
    rect.width = (gdouble) rect.width / zoom2nav_factor;
    rect.height = (gdouble) rect.height / zoom2nav_factor;

    return rect;
}

static void
uni_nav_draw_rectangle (UniNav * nav, gboolean clear_last)
{
    GdkWindow * window;
    GdkRectangle rect;

    window = gtk_widget_get_window (nav->preview);
    rect = gtk_image_get_current_rectangle (nav);

    /* Clear the last drawn rectangle. */
    if (clear_last)
        gdk_draw_rectangle (window, nav->gc, FALSE,
                            nav->last_rect.x, nav->last_rect.y,
                            nav->last_rect.width, nav->last_rect.height);

    gdk_draw_rectangle (window, nav->gc, FALSE,
                        rect.x, rect.y, rect.width, rect.height);
    nav->last_rect = rect;
}

static void
uni_nav_update_position (UniNav * nav)
{
    /* If the Navigation is opened, we don't have to move it again! */
    if (gtk_widget_get_visible (GTK_WIDGET (nav)))
    {
        return;
    }

    int off_x, off_y, x, y;
    GdkRectangle rect;

    /* Calculate position of popup. */
    Size pw = uni_nav_get_preview_size (nav);

    rect = gtk_image_get_current_rectangle (nav);

    /* 3 is the rectangle's line width, defined in nav->gc */
    off_x = rect.x + rect.width / 2 + 3;
    off_y = rect.y + rect.height / 2 + 3;

    x = nav->center_x - off_x;
    y = nav->center_y - off_y;

    /* Popup shoudn't be out of the screen */
    x = CLAMP (x, 0, gdk_screen_width () - pw.width);
    y = CLAMP (y, 0, gdk_screen_height () - pw.height);
    gtk_window_move (GTK_WINDOW (nav), x, y);
}

static void
uni_nav_update_pixbuf (UniNav * nav)
{
    if (nav->pixbuf)
    {
        g_object_unref (nav->pixbuf);
        nav->pixbuf = NULL;
    }
    GdkPixbuf *pixbuf = uni_image_view_get_pixbuf (nav->view);
    if (!pixbuf)
        return;

    Size pw = uni_nav_get_preview_size (nav);

    nav->pixbuf = gdk_pixbuf_new (gdk_pixbuf_get_colorspace (pixbuf),
                                  gdk_pixbuf_get_has_alpha (pixbuf),
                                  8, pw.width, pw.height);
    uni_pixbuf_scale_blend (pixbuf, nav->pixbuf,
                            0, 0, pw.width, pw.height,
                            0, 0,
                            uni_nav_get_zoom (nav),
                            GDK_INTERP_BILINEAR, 0, 0);
    // Lower the flag so the pixbuf isn't recreated more than
    // necessarily.
    nav->update_when_shown = FALSE;
}


/*************************************************************/
/***** Private signal handlers *******************************/
/*************************************************************/
static gboolean
uni_nav_expose_drawing_area (GtkWidget * widget,
                             GdkEventExpose * ev, UniNav * nav)
{
    GdkWindow * window;
    GtkStyle * style;

    if (!nav->pixbuf)
        return FALSE;

    window = gtk_widget_get_window (nav->preview);
    style = gtk_widget_get_style (nav->preview);

    gdk_draw_pixbuf (window, style->white_gc, nav->pixbuf,
                     0, 0, 0, 0, -1, -1, GDK_RGB_DITHER_MAX, 0, 0);
    uni_nav_draw_rectangle (nav, FALSE);
    uni_nav_update_position (nav);
    return TRUE;
}

/**
 * uni_nav_key_press:
 *
 * Keyboard events are caught just to forward them to the
 * #UniImageView which responds to them. That way, keyboard navigation
 * in #UniNav behaves consistently with #UniImageView.
 **/
static int
uni_nav_key_press (GtkWidget * widget, GdkEventKey * ev)
{
    UniNav *nav = UNI_NAV (widget);
    int retval = gtk_bindings_activate (GTK_OBJECT (nav->view),
                                        ev->keyval,
                                        ev->state);
    uni_nav_draw_rectangle (nav, TRUE);
    return retval;
}

static int
uni_nav_motion_notify (GtkWidget * widget, GdkEventMotion * ev)
{
    UniNav *nav = UNI_NAV (widget);
    int mx, my;
    gdk_window_get_pointer (gtk_widget_get_window (widget), &mx, &my, NULL);

    /* Make coordinates relative to window. */
    mx -= 4;
    my -= 4;

    /* Convert Nav space to Zoom space coordinates. */
    gdouble zoom2nav_factor = uni_nav_get_zoom2nav_factor (nav);
    GdkRectangle rect;
    uni_image_view_get_viewport (nav->view, &rect);

    /* Convert Zoom space to Nav space coordinates. */
    rect.width = (gdouble) rect.width / zoom2nav_factor;
    rect.height = (gdouble) rect.height / zoom2nav_factor;

    /* Subtract half of the XOR rectangles size from the coordinates. */
    mx -= (rect.width / 2);
    my -= (rect.height / 2);

    /* Do the adjusted coordinate conversion. */
    int zoom_x_ofs = mx * zoom2nav_factor;
    int zoom_y_ofs = my * zoom2nav_factor;

    uni_image_view_set_offset (nav->view, zoom_x_ofs, zoom_y_ofs, TRUE);
    uni_nav_draw_rectangle (nav, TRUE);

    return TRUE;
}

static void
uni_nav_pixbuf_changed (UniNav * nav)
{
    Size pw = uni_nav_get_preview_size (nav);

    // Set the new size and position of the preview.
    gtk_widget_set_size_request (GTK_WIDGET (nav->preview),
                                 pw.width, pw.height);
    uni_nav_update_position (nav);

    // If the widget is showing, then create the downsampled pixbuf.
    // Otherwise, just set a flag so that it is done later.
    nav->update_when_shown = TRUE;
    if (!gtk_widget_get_visible (GTK_WIDGET(nav)))
        return;
    uni_nav_update_pixbuf (nav);
    gtk_widget_queue_draw (GTK_WIDGET (nav));
}

/**
 * uni_nav_zoom_changed:
 *
 * Callback that is called whenever the zoom of the #UniImageView
 * changes.
 *
 * This function is needed because the zoom of the view may change
 * after a call to uni_image_view_set_zoom() due to the widget being
 * queued for size allocation.
 **/
static void
uni_nav_zoom_changed (UniNav * nav)
{
    uni_nav_draw_rectangle (nav, TRUE);
}

/**
 * uni_nav_button_released:
 *
 * Callback that is called whenever a mouse button is released. If LMB
 * is released, the nav is hidden.
 **/
static void
uni_nav_button_released (UniNav * nav, GdkEventButton * ev)
{
    if (ev->button != 1)
        return;
    uni_nav_release (nav);
    gtk_widget_hide (GTK_WIDGET (nav));
}

static void
uni_nav_realize (GtkWidget * widget)
{
    GTK_WIDGET_CLASS (uni_nav_parent_class)->realize (widget);
    UniNav *nav = UNI_NAV (widget);
    nav->gc = gdk_gc_new (gtk_widget_get_window (widget));
    gdk_gc_set_function (nav->gc, GDK_INVERT);
    gdk_gc_set_line_attributes (nav->gc,
                                3,
                                GDK_LINE_SOLID, GDK_CAP_BUTT, GDK_JOIN_MITER);
}

static void
uni_nav_unrealize (GtkWidget * widget)
{
    g_object_unref (UNI_NAV (widget)->gc);
    GTK_WIDGET_CLASS (uni_nav_parent_class)->unrealize (widget);
}

/*************************************************************/
/***** Stuff that deals with the type ************************/
/*************************************************************/
static void
uni_nav_init (UniNav * nav)
{
    nav->view = NULL;
    nav->gc = NULL;
    nav->last_rect = (GdkRectangle)
    {
    -1, -1, -1, -1};
    nav->update_when_shown = FALSE;

    GtkWidget *out_frame = gtk_frame_new (NULL);
    gtk_frame_set_shadow_type (GTK_FRAME (out_frame), GTK_SHADOW_OUT);
    gtk_container_add (GTK_CONTAINER (nav), out_frame);

    nav->preview = gtk_drawing_area_new ();
    gtk_container_add (GTK_CONTAINER (out_frame), nav->preview);
    g_signal_connect (G_OBJECT (nav->preview),
                      "expose_event",
                      G_CALLBACK (uni_nav_expose_drawing_area), nav);
}

static void
uni_nav_finalize (GObject * object)
{
    UniNav *nav = UNI_NAV (object);
    if (nav->pixbuf)
    {
        g_object_unref (nav->pixbuf);
        nav->pixbuf = NULL;
    }

    /* Chain up. */
    G_OBJECT_CLASS (uni_nav_parent_class)->finalize (object);
}

enum {
    PROP_IMAGE_VIEW = 1
};

static void
uni_nav_set_property (GObject * object,
                      guint prop_id, const GValue * value, GParamSpec * pspec)
{
    UniNav *nav = UNI_NAV (object);
    if (prop_id == PROP_IMAGE_VIEW)
    {
        nav->view = g_value_get_object (value);

        /* Give initial state to the widget. */
        uni_nav_pixbuf_changed (nav);

        g_signal_connect_swapped (G_OBJECT (nav->view), "pixbuf_changed",
                                  G_CALLBACK (uni_nav_pixbuf_changed), nav);
    }
    else
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
}

static void
uni_nav_class_init (UniNavClass * klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->finalize = uni_nav_finalize;
    object_class->set_property = uni_nav_set_property;

    GParamSpec *pspec = g_param_spec_object ("view",
                                             "Image View",
                                             "Image View to navigate",
                                             UNI_TYPE_IMAGE_VIEW,
                                             G_PARAM_CONSTRUCT_ONLY |
                                             G_PARAM_WRITABLE);
    g_object_class_install_property (object_class, PROP_IMAGE_VIEW, pspec);

    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
    widget_class->key_press_event = uni_nav_key_press;
    widget_class->motion_notify_event = uni_nav_motion_notify;
    widget_class->realize = uni_nav_realize;
    widget_class->unrealize = uni_nav_unrealize;
}

/**
 * uni_nav_new:
 * @view: a #UniImageView.
 * @returns: a new #UniNav showing the view.
 *
 * Creates a new #UniNav for showing thumbnails of the view. The
 * default values are is pixbuf=NULL.
 **/
GtkWidget *
uni_nav_new (UniImageView * view)
{
    g_return_val_if_fail (view, NULL);

    gpointer data = g_object_new (UNI_TYPE_NAV,
                                  "type", GTK_WINDOW_POPUP,
                                  /* The window must be non-resizable, otherwise it will not
                                     respond correctly to size requests which shrinks it. */
                                  "resizable", FALSE,
                                  "view", view,
                                  NULL);
    return GTK_WIDGET (data);
}

/*************************************************************/
/***** Actions ***********************************************/
/*************************************************************/
void
uni_nav_grab (UniNav * nav)
{
    GtkWidget *preview = nav->preview;
    GdkWindow *window;

    gtk_grab_add (preview);

    GdkCursor *cursor = gdk_cursor_new (GDK_FLEUR);
    int mask = (GDK_POINTER_MOTION_MASK
                | GDK_POINTER_MOTION_HINT_MASK
                | GDK_BUTTON_RELEASE_MASK | GDK_EXTENSION_EVENTS_ALL);
    window = gtk_widget_get_window (preview);
    gdk_pointer_grab (window, TRUE, mask, window, cursor, 0);
    gdk_cursor_unref (cursor);

    /* Capture keyboard events. */
    gdk_keyboard_grab (window, TRUE, GDK_CURRENT_TIME);
    gtk_widget_grab_focus (preview);
}

void
uni_nav_release (UniNav * nav)
{
    gdk_pointer_ungrab (GDK_CURRENT_TIME);

    /* Release keyboard focus. */
    gdk_keyboard_ungrab (GDK_CURRENT_TIME);
    gtk_grab_remove (nav->preview);
}



/*************************************************************/
/***** Runner function ***************************************/
/*************************************************************/
/**
 * uni_nav_show_and_grab:
 * @nav: a #UniNav
 * @center_x: x coordinate of center position
 * @center_y: y coordinate of center position
 *
 * Show the #UniNav centered around the point (@center_x,
 * @center_y) and grab mouse and keyboard events. The grab continues
 * until a button release event is received which causes the widget to
 * hide.
 **/
void
uni_nav_show_and_grab (UniNav * nav, int center_x, int center_y)
{
    nav->center_x = center_x;
    nav->center_y = center_y;
    uni_nav_update_position (nav);

    if (nav->update_when_shown)
        uni_nav_update_pixbuf (nav);


    /* Connect signals and run! */
    gtk_widget_show_all (GTK_WIDGET (nav));
    uni_nav_grab (nav);

    g_signal_connect (G_OBJECT (nav), "button-release-event",
                      G_CALLBACK (uni_nav_button_released), NULL);
    g_signal_connect_swapped (G_OBJECT (nav->view), "zoom_changed",
                              G_CALLBACK (uni_nav_zoom_changed), nav);
}
