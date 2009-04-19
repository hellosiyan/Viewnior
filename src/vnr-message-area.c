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
#include "vnr-message-area.h"

G_DEFINE_TYPE (VnrMessageArea, vnr_message_area, GTK_TYPE_EVENT_BOX);

static gint vnr_message_area_delete (GtkWidget * widget, GdkEventAny * event);

static void
vnr_message_area_class_init (VnrMessageAreaClass * klass)
{
    GtkWidgetClass *widget_class = (GtkWidgetClass *) klass;
    widget_class->delete_event = vnr_message_area_delete;
}

GtkWidget *
vnr_message_area_new ()
{
    return (GtkWidget *) g_object_new (VNR_TYPE_MESSAGE_AREA, NULL);
}

static void
vnr_message_area_initialize(VnrMessageArea * msg_area)
{
    msg_area->hbox = gtk_hbox_new(FALSE, 12);
    gtk_container_add(GTK_CONTAINER (msg_area), msg_area->hbox);
    gtk_container_set_border_width(GTK_CONTAINER (msg_area->hbox), 12);

    msg_area->image = gtk_image_new_from_stock(GTK_STOCK_DIALOG_ERROR, GTK_ICON_SIZE_DIALOG);
    gtk_box_pack_start(GTK_BOX (msg_area->hbox), msg_area->image, FALSE, FALSE, 0);

    msg_area->message = gtk_label_new (NULL);
    gtk_label_set_line_wrap(GTK_LABEL (msg_area->message), TRUE);
    gtk_box_pack_start(GTK_BOX (msg_area->hbox), msg_area->message, FALSE, FALSE, 0);

    gtk_widget_hide_all(msg_area->hbox);
    gtk_widget_set_state(GTK_WIDGET(msg_area), GTK_STATE_SELECTED);
    msg_area->initialized = TRUE;
}

static void
vnr_message_area_init (VnrMessageArea * msg_area)
{
    msg_area->initialized = FALSE;
}

static gint
vnr_message_area_delete (GtkWidget * widget, GdkEventAny * event)
{
    gtk_widget_destroy (widget);
    return TRUE;
}

void
vnr_message_area_show_warning (VnrMessageArea *msg_area, const char *message)
{
    if(!msg_area->initialized)
    {
        vnr_message_area_initialize(msg_area);
    }
    gtk_label_set_markup(GTK_LABEL(msg_area->message),
                         g_markup_printf_escaped
                         ("<span weight=\"bold\">%s</span>",message));

    gtk_widget_show_all(GTK_WIDGET (msg_area->hbox));
}

void
vnr_message_area_hide_warning (VnrMessageArea *msg_area)
{
    gtk_widget_hide_all(GTK_WIDGET (msg_area->hbox));
}

