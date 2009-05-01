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

GtkWindow *
vnr_window_new ()
{
    return (GtkWindow *) g_object_new (VNR_TYPE_WINDOW, NULL);
}

static void
vnr_window_init (VnrWindow * window)
{
    window->file_list = NULL;

    gtk_window_set_title ((GtkWindow *) window, "Viewnior");
    gtk_window_set_icon_name ((GtkWindow *) window, "viewnior");

    window->max_width = gdk_screen_width () * 0.8;
    window->max_height = gdk_screen_height () * 0.8;

    window->layout = gtk_vbox_new(FALSE,0);
    gtk_container_add (GTK_CONTAINER (window), window->layout);
    gtk_widget_show(window->layout);

    window->msg_area = vnr_message_area_new();
    VNR_MESSAGE_AREA(window->msg_area)->vnr_win = window;
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
vnr_window_open (VnrWindow * win, gboolean fit_to_screen)
{
    VnrFile *file;
    GdkPixbufAnimation *pixbuf;
    GError *error = NULL;

    file = VNR_FILE(win->file_list->data);

    //printf("OPENING:%s\n", file->uri);

    gtk_window_set_title (GTK_WINDOW (win), file->display_name);

    pixbuf = gdk_pixbuf_animation_new_from_file (file->uri, &error);
    //pixbuf = gdk_pixbuf_animation_new_from_file (g_filename_from_uri(file->uri,NULL,NULL), &error);

    if (error != NULL)
    {
        vnr_message_area_show_warning(VNR_MESSAGE_AREA (win->msg_area), error->message);
        return FALSE;
    }

    if(fit_to_screen)
    {
        gint img_h, img_w;          /* Width and Height of the pixbuf */

        img_w = gdk_pixbuf_animation_get_width (pixbuf);
        img_h = gdk_pixbuf_animation_get_height (pixbuf);

        vnr_tools_fit_to_size (&img_w, &img_h, win->max_width, win->max_height);

        gtk_window_resize (GTK_WINDOW (win), (img_w < 200) ? 200 : img_w,
                           (img_h < 200) ? 200 : img_h);
    }

    uni_anim_view_set_anim (UNI_ANIM_VIEW (win->view), pixbuf);

    g_object_unref(pixbuf);

    return TRUE;
}

void
vnr_window_close(VnrWindow *win)
{
    gtk_window_set_title (GTK_WINDOW (win), "Viewnior");
    uni_anim_view_set_anim (UNI_ANIM_VIEW (win->view), NULL);
}

void
vnr_window_set_list (VnrWindow *win, GList *list)
{
    g_assert(list != NULL);
    win->file_list = list;
}

gboolean
vnr_window_next (VnrWindow *win){
    GList *next;

    next = g_list_next(win->file_list);
    if(next == NULL)
    {
        next = g_list_first(win->file_list);
    }

    if(vnr_message_area_get_visible(VNR_MESSAGE_AREA(win->msg_area)))
    {
        vnr_message_area_hide(VNR_MESSAGE_AREA(win->msg_area));
    }

    win->file_list = next;

    gdk_window_set_cursor(GTK_WIDGET(win)->window, gdk_cursor_new(GDK_WATCH));
    /* This makes the cursor show NOW */
    gtk_main_iteration_do (FALSE);

    vnr_window_open(win, FALSE);
    gdk_window_set_cursor(GTK_WIDGET(win)->window, gdk_cursor_new(GDK_LEFT_PTR));
    return TRUE;
}

gboolean
vnr_window_prev (VnrWindow *win){
    GList *prev;

    prev = g_list_previous(win->file_list);
    if(prev == NULL)
    {
        prev = g_list_last(win->file_list);
    }

    if(vnr_message_area_get_visible(VNR_MESSAGE_AREA(win->msg_area)))
    {
        vnr_message_area_hide(VNR_MESSAGE_AREA(win->msg_area));
    }

    win->file_list = prev;

    gdk_window_set_cursor(GTK_WIDGET(win)->window, gdk_cursor_new(GDK_WATCH));
    /* This makes the cursor show NOW */
    gtk_main_iteration_do (FALSE);

    vnr_window_open(win, FALSE);
    gdk_window_set_cursor(GTK_WIDGET(win)->window, gdk_cursor_new(GDK_LEFT_PTR));
    return TRUE;
}
