/*
 * Copyright © 2009 Siyan Panayotov <xsisqox@gmail.com>
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

#include <gtk/gtk.h>
#include "vnr-window.h"
#include "uni-scroll-win.h"
#include "uni-anim-view.h"
#include "vnr-tools.h"
#include "vnr-message-area.h"

G_DEFINE_TYPE (VnrWindow, vnr_window, GTK_TYPE_WINDOW);

static gint vnr_window_delete (GtkWidget * widget, GdkEventAny * event);


static void
vnr_window_class_init (VnrWindowClass * klass)
{
    GtkWidgetClass *widget_class = (GtkWidgetClass *) klass;
    widget_class->delete_event = vnr_window_delete;
}

GtkWidget *
vnr_window_new ()
{
    return (GtkWidget *) g_object_new (VNR_TYPE_WINDOW, NULL);
}

static void
vnr_window_init (VnrWindow * window)
{
    gtk_window_set_title ((GtkWindow *) window, "Viewnior");
    gtk_window_set_icon_name ((GtkWindow *) window, "viewnior");

    window->max_width = gdk_screen_width () * 0.8;
    window->max_height = gdk_screen_height () * 0.8;

    window->layout = gtk_vbox_new(FALSE,0);
    gtk_container_add (GTK_CONTAINER (window), window->layout);
    gtk_widget_show(window->layout);

    window->msg_area = vnr_message_area_new();
    gtk_box_pack_start (GTK_BOX (window->layout), window->msg_area, FALSE,FALSE,0);
    gtk_widget_show(GTK_WIDGET (window->msg_area));

    window->view = uni_anim_view_new ();
    window->scroll_view = uni_scroll_win_new (UNI_IMAGE_VIEW (window->view));
    gtk_box_pack_end (GTK_BOX (window->layout), window->scroll_view, TRUE,TRUE,0);
    gtk_widget_show_all(GTK_WIDGET (window->scroll_view));

    g_signal_connect (G_OBJECT (window), "destroy",
                      G_CALLBACK (gtk_main_quit), NULL);
}

static gint
vnr_window_delete (GtkWidget * widget, GdkEventAny * event)
{
    gtk_widget_destroy (widget);
    return TRUE;
}

gboolean
vnr_window_open (VnrWindow * win, const char *path)
{
    GdkPixbufAnimation *pixbuf;
    GError *error = NULL;
    gint img_h, img_w;          /* Width and Height of the pixbuf */

    gtk_window_set_title (GTK_WINDOW (win), g_path_get_basename (path));

    pixbuf = gdk_pixbuf_animation_new_from_file (path, &error);

    if (error != NULL)
    {
        /* Warn about the error! */
        vnr_message_area_show_warning(VNR_MESSAGE_AREA (win->msg_area), error->message);
        return FALSE;
    }


    img_w = gdk_pixbuf_animation_get_width (pixbuf);
    img_h = gdk_pixbuf_animation_get_height (pixbuf);

    fit_to_size (&img_w, &img_h, win->max_width, win->max_height);

    gtk_window_resize (GTK_WINDOW (win), (img_w < 200) ? 200 : img_w,
                       (img_h < 200) ? 200 : img_h);

    uni_anim_view_set_anim (UNI_ANIM_VIEW (win->view), pixbuf);

    return TRUE;
}
