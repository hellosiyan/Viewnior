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
#include "vnr-toolbar.h"

G_DEFINE_TYPE (VnrToolbar, vnr_toolbar, GTK_TYPE_TOOLBAR);

static gint vnr_toolbar_delete (GtkWidget * widget, GdkEventAny * event);

static void
vnr_toolbar_class_init (VnrToolbarClass * klass)
{
    GtkWidgetClass *widget_class = (GtkWidgetClass *) klass;
    widget_class->delete_event = vnr_toolbar_delete;
}

GtkWidget *
vnr_toolbar_new ()
{
    return (GtkWidget *) g_object_new (VNR_TYPE_TOOLBAR, NULL);
}

static void
vnr_toolbar_init (VnrToolbar * toolbar)
{
    GtkToolItem *tool;
    gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);

    tool = gtk_tool_button_new_from_stock(GTK_STOCK_GO_BACK);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool, -1);

    tool = gtk_tool_button_new_from_stock(GTK_STOCK_GO_FORWARD);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool, -1);

    tool = gtk_separator_tool_item_new();
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool, -1);

    tool = gtk_tool_button_new_from_stock(GTK_STOCK_ZOOM_IN);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool, -1);

    tool = gtk_tool_button_new_from_stock(GTK_STOCK_ZOOM_OUT);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool, -1);

    tool = gtk_tool_button_new_from_stock(GTK_STOCK_ZOOM_100);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool, -1);

    tool = gtk_tool_button_new_from_stock(GTK_STOCK_ZOOM_FIT);
    gtk_toolbar_insert(GTK_TOOLBAR(toolbar), tool, -1);

    gtk_widget_show_all(GTK_WIDGET(toolbar));
}

static gint
vnr_toolbar_delete (GtkWidget * widget, GdkEventAny * event)
{
    gtk_widget_destroy (widget);
    return TRUE;
}
