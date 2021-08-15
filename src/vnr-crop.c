/*
 * Copyright © 2009-2018 Siyan Panayotov <contact@siyanpanayotov.com>
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

#include "vnr-crop.h"
#include "vnr-tools.h"
#include "uni-utils.h"
#include "uni-image-view.h"

#define CROP_UI_PATH PACKAGE_DATA_DIR"/viewnior/vnr-crop-dialog.ui"

G_DEFINE_TYPE (VnrCrop, vnr_crop, G_TYPE_OBJECT);

static void spin_x_cb       (GtkSpinButton *spinbutton, VnrCrop *crop);
static void spin_width_cb   (GtkSpinButton *spinbutton, VnrCrop *crop);
static void spin_y_cb       (GtkSpinButton *spinbutton, VnrCrop *crop);
static void spin_height_cb  (GtkSpinButton *spinbutton, VnrCrop *crop);

static gboolean drawable_expose_cb (GtkWidget *widget,
                                    GdkEventExpose *event, VnrCrop *crop);
static gboolean drawable_button_press_cb (GtkWidget *widget,
                                          GdkEventButton *event, VnrCrop *crop);
static gboolean drawable_button_release_cb (GtkWidget *widget,
                                            GdkEventButton *event, VnrCrop *crop);
static gboolean drawable_motion_cb (GtkWidget *widget,
                                    GdkEventMotion *event, VnrCrop *crop);

/*************************************************************/
/***** Private actions ***************************************/
/*************************************************************/
static void
vnr_crop_draw_rectangle(VnrCrop *crop)
{
    if(crop->do_redraw)
        gdk_draw_rectangle (gtk_widget_get_window(crop->image), crop->gc, FALSE,
                            crop->sub_x, crop->sub_y,
                            crop->sub_width, crop->sub_height);
}

static inline void
vnr_crop_clear_rectangle(VnrCrop *crop)
{
    vnr_crop_draw_rectangle (crop);
}

static void
vnr_crop_check_sub_y(VnrCrop *crop)
{
    if(gtk_spin_button_get_value(crop->spin_height)
        + gtk_spin_button_get_value(crop->spin_y)
        == crop->vnr_win->current_image_height)
    {
        crop->sub_y = (int)crop->height - (int)crop->sub_height;
    }
}

static void
vnr_crop_check_sub_x(VnrCrop *crop)
{
    if(gtk_spin_button_get_value(crop->spin_width)
        + gtk_spin_button_get_value(crop->spin_x)
        == crop->vnr_win->current_image_width)
    {
        crop->sub_x = (int)crop->width - (int)crop->sub_width;
    }
}

static void
vnr_crop_update_spin_button_values (VnrCrop *crop)
{
    gtk_spin_button_set_value (crop->spin_height, crop->sub_height / crop->zoom);
    gtk_spin_button_set_value (crop->spin_width, crop->sub_width / crop->zoom);

    gtk_spin_button_set_value (crop->spin_x, crop->sub_x / crop->zoom);
    gtk_spin_button_set_value (crop->spin_y, crop->sub_y / crop->zoom);
}

static GtkWidget *
vnr_crop_build_dialog (VnrCrop *crop)
{
    GtkBuilder *builder;
    GtkWidget *window;
    GdkPixbuf *original;
    GdkPixbuf *preview;
    GError *error = NULL;

    builder = gtk_builder_new ();
    gtk_builder_add_from_file (builder, CROP_UI_PATH, &error);

    if (error != NULL)
    {
        g_warning ("%s\n", error->message);
        g_object_unref(builder);
        return NULL;
    }

    window = GTK_WIDGET (gtk_builder_get_object (builder, "crop-dialog"));
    gtk_window_set_transient_for(GTK_WINDOW(window), GTK_WINDOW(crop->vnr_win));

    original = uni_image_view_get_pixbuf(UNI_IMAGE_VIEW(crop->vnr_win->view));

    gdouble width, height;
    gint max_width, max_height;

    width = crop->vnr_win->current_image_width;
    height = crop->vnr_win->current_image_height;

    {
        GdkScreen *screen;
        GdkRectangle monitor;
        screen = gtk_window_get_screen (GTK_WINDOW (crop->vnr_win));
        gdk_screen_get_monitor_geometry (screen,
                                        gdk_screen_get_monitor_at_window (screen,
                                        gtk_widget_get_window (GTK_WIDGET (crop->vnr_win))),
                                        &monitor);

        max_width = monitor.width * 0.9 - 100;
        max_height = monitor.height * 0.9 - 200;
    }

    vnr_tools_fit_to_size_double(&width, &height, max_width, max_height);
    crop->width = width;
    crop->height = height;
    crop->zoom = ( width/crop->vnr_win->current_image_width
                   + height/crop->vnr_win->current_image_height )/2;

    preview = gdk_pixbuf_new (gdk_pixbuf_get_colorspace (original),
                             gdk_pixbuf_get_has_alpha (original),
                             gdk_pixbuf_get_bits_per_sample (original),
                             width, height);

    uni_pixbuf_scale_blend(original, preview, 0, 0, width, height, 0, 0,
                           crop->zoom, GDK_INTERP_BILINEAR, 0, 0);
    crop->preview_pixbuf = preview;

    crop->image = GTK_WIDGET (gtk_builder_get_object (builder, "main-image"));
    gtk_widget_set_size_request(crop->image, width, height);

    crop->spin_x = GTK_SPIN_BUTTON (gtk_builder_get_object (builder, "spin-x"));
    gtk_spin_button_set_range (crop->spin_x, 0, crop->vnr_win->current_image_width - 1);
    gtk_spin_button_set_increments (crop->spin_x, 1, 10);

    crop->spin_y = GTK_SPIN_BUTTON (gtk_builder_get_object (builder, "spin-y"));
    gtk_spin_button_set_range (crop->spin_y, 0, crop->vnr_win->current_image_height - 1);
    gtk_spin_button_set_increments (crop->spin_y, 1, 10);

    crop->spin_width = GTK_SPIN_BUTTON (gtk_builder_get_object (builder, "spin-width"));
    gtk_spin_button_set_range (crop->spin_width, 1, crop->vnr_win->current_image_width);
    gtk_spin_button_set_increments (crop->spin_width, 1, 10);
    gtk_spin_button_set_value (crop->spin_width, crop->vnr_win->current_image_width);

    crop->spin_height = GTK_SPIN_BUTTON (gtk_builder_get_object (builder, "spin-height"));
    gtk_spin_button_set_range (crop->spin_height, 1, crop->vnr_win->current_image_height);
    gtk_spin_button_set_increments (crop->spin_height, 1, 10);
    gtk_spin_button_set_value (crop->spin_height, crop->vnr_win->current_image_height);

    gtk_widget_set_events (crop->image, GDK_BUTTON_PRESS_MASK|GDK_BUTTON_RELEASE_MASK|GDK_BUTTON_MOTION_MASK);

    g_signal_connect (crop->image, "expose-event",
                      G_CALLBACK (drawable_expose_cb), crop);
    g_signal_connect (crop->image, "button-press-event",
                      G_CALLBACK (drawable_button_press_cb), crop);
    g_signal_connect (crop->image, "button-release-event",
                      G_CALLBACK (drawable_button_release_cb), crop);
    g_signal_connect (crop->image, "motion-notify-event",
                      G_CALLBACK (drawable_motion_cb), crop);

    g_signal_connect (crop->spin_width, "value-changed",
                      G_CALLBACK (spin_width_cb), crop);
    g_signal_connect (crop->spin_x, "value-changed",
                      G_CALLBACK (spin_x_cb), crop);
    g_signal_connect (crop->spin_height, "value-changed",
                      G_CALLBACK (spin_height_cb), crop);
    g_signal_connect (crop->spin_y, "value-changed",
                      G_CALLBACK (spin_y_cb), crop);

    g_object_unref(builder);

    return window;
}

/*************************************************************/
/***** Private signal handlers *******************************/
/*************************************************************/
static void
spin_x_cb (GtkSpinButton *spinbutton, VnrCrop *crop)
{
    if(crop->drawing_rectangle)
        return;

    vnr_crop_clear_rectangle (crop);

    gboolean old_do_redraw = crop->do_redraw;
    crop->do_redraw = FALSE;
    gtk_spin_button_set_range (crop->spin_width, 1,
                               crop->vnr_win->current_image_width
                                - gtk_spin_button_get_value(spinbutton));
    crop->do_redraw = old_do_redraw;

    crop->sub_x = gtk_spin_button_get_value (spinbutton) * crop->zoom;

    vnr_crop_check_sub_x(crop);

    vnr_crop_draw_rectangle (crop);
}

static void
spin_width_cb (GtkSpinButton *spinbutton, VnrCrop *crop)
{
    if(crop->drawing_rectangle)
        return;

    vnr_crop_clear_rectangle (crop);

    crop->sub_width = gtk_spin_button_get_value (spinbutton) * crop->zoom;

    if(crop->sub_width <1)
        crop->sub_width = 1;

    vnr_crop_draw_rectangle (crop);
}

static void
spin_y_cb (GtkSpinButton *spinbutton, VnrCrop *crop)
{
    if(crop->drawing_rectangle)
        return;

    vnr_crop_clear_rectangle (crop);

    gboolean old_do_redraw = crop->do_redraw;
    crop->do_redraw = FALSE;
    gtk_spin_button_set_range (crop->spin_height, 1,
                               crop->vnr_win->current_image_height
                                - gtk_spin_button_get_value(spinbutton));
    crop->do_redraw = old_do_redraw;

    crop->sub_y = gtk_spin_button_get_value(spinbutton) * crop->zoom;

    vnr_crop_check_sub_y(crop);

    vnr_crop_draw_rectangle (crop);
}

static void
spin_height_cb (GtkSpinButton *spinbutton, VnrCrop *crop)
{
    if(crop->drawing_rectangle)
        return;

    vnr_crop_clear_rectangle (crop);

    crop->sub_height = gtk_spin_button_get_value(spinbutton) * crop->zoom;

    if(crop->sub_height <1)
        crop->sub_height = 1;

    vnr_crop_draw_rectangle (crop);
}

static gboolean
drawable_expose_cb (GtkWidget *widget, GdkEventExpose *event, VnrCrop *crop)
{
    GdkWindow *window = gtk_widget_get_window(widget);
    gdk_draw_pixbuf (window, NULL, crop->preview_pixbuf,
                     0, 0, 0, 0, -1, -1, GDK_RGB_DITHER_NORMAL, 0, 0);

    crop->gc = gdk_gc_new(window);
    gdk_gc_set_function (crop->gc, GDK_INVERT);
    gdk_gc_set_line_attributes (crop->gc,
                                2,
                                GDK_LINE_SOLID, GDK_CAP_BUTT, GDK_JOIN_MITER);

    if(crop->sub_width == -1)
    {
        crop->sub_x = 0;
        crop->sub_y = 0;
        crop->sub_width = crop->width;
        crop->sub_height = crop->height;
    }
    vnr_crop_clear_rectangle (crop);

    return FALSE;
}

static gboolean
drawable_button_press_cb (GtkWidget *widget, GdkEventButton *event, VnrCrop *crop)
{
    if(event->button == 1)
    {
        crop->drawing_rectangle = TRUE;
        crop->start_x =  event->x;
        crop->start_y =  event->y;
    }

    return FALSE;
}

static gboolean
drawable_button_release_cb (GtkWidget *widget, GdkEventButton *event, VnrCrop *crop)
{
    if(event->button == 1)
    {
        crop->drawing_rectangle = FALSE;

        gtk_spin_button_set_range(crop->spin_width, 1,
                                  (crop->width - crop->sub_x) / crop->zoom);
        gtk_spin_button_set_range(crop->spin_height, 1,
                                  (crop->height - crop->sub_y) / crop->zoom);

        vnr_crop_update_spin_button_values (crop);
    }
    return FALSE;
}

static gboolean
drawable_motion_cb (GtkWidget *widget, GdkEventMotion *event, VnrCrop *crop)
{
    if(!crop->drawing_rectangle)
        return FALSE;

    gdouble x, y;
    x = event->x;
    y = event->y;

    x = CLAMP(x, 0, crop->width);
    y = CLAMP(y, 0, crop->height);

    vnr_crop_clear_rectangle (crop);

    if(x > crop->start_x)
    {
        crop->sub_x = crop->start_x;
        crop->sub_width = x - crop->start_x;
    }
    else if(x == crop->start_x)
    {
        crop->sub_x = x;
        crop->sub_width = 1;
    }
    else
    {
        crop->sub_x = x;
        crop->sub_width = crop->start_x - x;
    }

    if(y > crop->start_y)
    {
        crop->sub_y = crop->start_y;
        crop->sub_height = y - crop->start_y;
    }
    else if(y == crop->start_y)
    {
        crop->sub_y = y;
        crop->sub_height = 1;
    }
    else
    {
        crop->sub_y = y;
        crop->sub_height = crop->start_y - y;
    }

    crop->drawing_rectangle = FALSE;
    crop->do_redraw= FALSE;

    vnr_crop_update_spin_button_values (crop);

    crop->drawing_rectangle = TRUE;
    crop->do_redraw= TRUE;

    vnr_crop_draw_rectangle (crop);

    return FALSE;
}

/*************************************************************/
/***** Stuff that deals with the type ************************/
/*************************************************************/
static void
vnr_crop_dispose (GObject *gobject)
{
    VnrCrop *self = VNR_CROP (gobject);

    if (self->preview_pixbuf != NULL)
        g_object_unref (self->preview_pixbuf);
    if (self->gc != NULL)
        g_object_unref (self->gc);

    G_OBJECT_CLASS (vnr_crop_parent_class)->dispose (gobject);
}

static void
vnr_crop_class_init (VnrCropClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

    gobject_class->dispose = vnr_crop_dispose;
}

GObject *
vnr_crop_new (VnrWindow *vnr_win)
{
    VnrCrop *crop;

    crop = g_object_new (VNR_TYPE_CROP, NULL);

    crop->vnr_win = vnr_win;

    return (GObject *) crop;
}

static void
vnr_crop_init (VnrCrop *crop)
{
    crop->drawing_rectangle = FALSE;
    crop->do_redraw = TRUE;

    crop->sub_x = -1;
    crop->sub_y = -1;
    crop->sub_height = -1;
    crop->sub_width = -1;
    crop->height = -1;
    crop->width = -1;

    crop->gc = NULL;
    crop->image = NULL;
    crop->spin_x = NULL;
    crop->spin_y = NULL;
    crop->spin_width = NULL;
    crop->spin_height = NULL;
    crop->preview_pixbuf = NULL;
}

/*************************************************************/
/***** Actions ***********************************************/
/*************************************************************/
gboolean
vnr_crop_run (VnrCrop *crop)
{
    GtkWidget *dialog;
    gint crop_dialog_response;

    dialog = vnr_crop_build_dialog(crop);

    if(dialog == NULL)
        return FALSE;

    crop_dialog_response = gtk_dialog_run (GTK_DIALOG (dialog));

    crop->area.x = gtk_spin_button_get_value_as_int (crop->spin_x);
    crop->area.y = gtk_spin_button_get_value_as_int (crop->spin_y);
    crop->area.width = gtk_spin_button_get_value_as_int (crop->spin_width);
    crop->area.height = gtk_spin_button_get_value_as_int (crop->spin_height);

    gtk_widget_destroy (dialog);

    if(crop->area.x == 0 && crop->area.y == 0
       && crop->area.width == crop->vnr_win->current_image_width
       && crop->area.height == crop->vnr_win->current_image_height)
    {
        return FALSE;
    }
    else
    {
        return (crop_dialog_response == GTK_RESPONSE_ACCEPT);
    }
}
