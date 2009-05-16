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

#include <libintl.h>
#include <glib/gi18n.h>
#define _(String) gettext (String)

#include <config.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include "vnr-window.h"
#include "uni-scroll-win.h"
#include "uni-anim-view.h"
#include "vnr-tools.h"
#include "vnr-message-area.h"

G_DEFINE_TYPE (VnrWindow, vnr_window, GTK_TYPE_WINDOW);

static gint vnr_window_delete (GtkWidget * widget, GdkEventAny * event);
static gint vnr_window_key_press (GtkWidget *widget, GdkEventKey *event);


static void
vnr_window_class_init (VnrWindowClass * klass)
{
    GtkWidgetClass *widget_class = (GtkWidgetClass *) klass;
    widget_class->delete_event = vnr_window_delete;
    widget_class->key_press_event = vnr_window_key_press;
}

GtkWindow *
vnr_window_new ()
{
    return (GtkWindow *) g_object_new (VNR_TYPE_WINDOW, NULL);
}

static gboolean
scrollbars_visible (VnrWindow *win)
{
    if (!GTK_WIDGET_VISIBLE (GTK_WIDGET (UNI_SCROLL_WIN(win->scroll_view)->hscroll)) &&
        !GTK_WIDGET_VISIBLE (GTK_WIDGET (UNI_SCROLL_WIN(win->scroll_view)->vscroll)))
        return FALSE;

    return TRUE;
}

int dumb(void){
    printf("Me so dumb!\n");
    return 0;
}

static void
vnr_window_cmd_zoom_in (GtkAction *action, gpointer user_data)
{
    g_return_if_fail (VNR_IS_WINDOW (user_data));

    if (VNR_WINDOW(user_data)->view) {
        uni_image_view_zoom_in(UNI_IMAGE_VIEW(VNR_WINDOW(user_data)->view));
    }
}

static void
vnr_window_cmd_zoom_out (GtkAction *action, gpointer user_data)
{
    g_return_if_fail (VNR_IS_WINDOW (user_data));

    if (VNR_WINDOW(user_data)->view) {
        uni_image_view_zoom_out(UNI_IMAGE_VIEW(VNR_WINDOW(user_data)->view));
    }
}

static void
vnr_window_cmd_normal_size (GtkAction *action, gpointer user_data)
{
    g_return_if_fail (VNR_IS_WINDOW (user_data));

    if (VNR_WINDOW(user_data)->view) {
        uni_image_view_set_zoom(UNI_IMAGE_VIEW(VNR_WINDOW(user_data)->view), 1);
    }
}

static void
vnr_window_cmd_fit (GtkAction *action, gpointer user_data)
{
    g_return_if_fail (VNR_IS_WINDOW (user_data));

    if (VNR_WINDOW(user_data)->view) {
        uni_image_view_set_fitting(UNI_IMAGE_VIEW(VNR_WINDOW(user_data)->view), TRUE);
    }
}

static void
vnr_window_cmd_next (GtkAction *action, gpointer user_data)
{
    g_return_if_fail (VNR_IS_WINDOW (user_data));

    if (VNR_WINDOW(user_data)->view) {
        vnr_window_next(VNR_WINDOW(user_data));
    }
}

static void
vnr_window_cmd_first (GtkAction *action, gpointer user_data)
{
    g_return_if_fail (VNR_IS_WINDOW (user_data));

    if (VNR_WINDOW(user_data)->view) {
        vnr_window_first(VNR_WINDOW(user_data));
    }
}

static void
vnr_window_cmd_last (GtkAction *action, gpointer user_data)
{
    g_return_if_fail (VNR_IS_WINDOW (user_data));

    if (VNR_WINDOW(user_data)->view) {
        vnr_window_last(VNR_WINDOW(user_data));
    }
}

static void
vnr_window_cmd_prev (GtkAction *action, gpointer user_data)
{
    g_return_if_fail (VNR_IS_WINDOW (user_data));

    if (VNR_WINDOW(user_data)->view) {
        vnr_window_prev(VNR_WINDOW(user_data));
    }
}

static void
vnr_window_cmd_about (GtkAction *action, gpointer user_data)
{
    VnrWindow *window;

    g_return_if_fail (VNR_IS_WINDOW (user_data));

    static const char *authors[] = {
        "Siyan Panayotov <xsisqox@gmail.com>",
        NULL
    };

    const char *license[] = {
          ("Viewnior is free software: you can redistribute it and/or modify "
           "it under the terms of the GNU General Public License as published by "
           "the Free Software Foundation, either version 3 of the License, or "
           "(at your option) any later version.\n"),
          ("Viewnior is distributed in the hope that it will be useful, "
           "but WITHOUT ANY WARRANTY; without even the implied warranty of "
           "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
           "GNU General Public License for more details.\n"),
          ("You should have received a copy of the GNU General Public License "
           "along with Viewnior.  If not, see <http://www.gnu.org/licenses/>.")
    };

    char *license_trans;

    license_trans = g_strconcat (_(license[0]), "\n", _(license[1]), "\n",
                     _(license[2]), "\n", NULL);

    window = VNR_WINDOW (user_data);

    gtk_show_about_dialog (GTK_WINDOW (window),
                   "program-name", "Viewnior",
                   "version", VERSION,
                   "copyright", "Copyright \xc2\xa9 2009 Siyan Panayotov <xsisqox@gmail.com>",
                   "comments",_("Elegant Image Viewer"),
                   "authors", authors,
                   "logo-icon-name", "viewnior",
                   "wrap-license", TRUE,
                   "license", license_trans,
                   NULL);

    g_free (license_trans);
}

static const GtkActionEntry action_entries_window[] = {
    { "File",  NULL, N_("_File") },
    { "View",  NULL, N_("_View") },
    { "Image", NULL, N_("_Image") },
    { "Go",    NULL, N_("_Go") },
    { "Help",  NULL, N_("_Help") },

    { "FileClose", GTK_STOCK_CLOSE, N_("_Close"), "<control>W",
      N_("Close window"),
      G_CALLBACK (gtk_main_quit) },
    { "HelpManual", GTK_STOCK_HELP, N_("_Contents"), "F1",
      N_("Help on this application"),
      G_CALLBACK (dumb) },
    { "HelpAbout", GTK_STOCK_ABOUT, N_("_About"), NULL,
      N_("About this application"),
      G_CALLBACK (vnr_window_cmd_about) }
};

static const GtkActionEntry action_entries_image[] = {
    { "SetAsWallpaper", NULL, N_("Set as _Desktop Background"), NULL,
      N_("Set the selected image as the desktop background"),
      G_CALLBACK (dumb) },
    { "ImageMoveToTrash", "user-trash", N_("Move to _Trash"), NULL,
      N_("Move the selected image to the trash folder"),
      G_CALLBACK (dumb) },
    { "ViewZoomIn", GTK_STOCK_ZOOM_IN, N_("_Zoom In"), "<control>plus",
      N_("Enlarge the image"),
      G_CALLBACK (vnr_window_cmd_zoom_in) },
    { "ViewZoomOut", GTK_STOCK_ZOOM_OUT, N_("Zoom _Out"), "<control>minus",
      N_("Shrink the image"),
      G_CALLBACK (vnr_window_cmd_zoom_out) },
    { "ViewZoomNormal", GTK_STOCK_ZOOM_100, N_("_Normal Size"), "<control>0",
      N_("Show the image at its normal size"),
      G_CALLBACK (vnr_window_cmd_normal_size) },
    { "ViewZoomFit", GTK_STOCK_ZOOM_FIT, N_("Best _Fit"), NULL,
      N_("Fit the image to the window"),
      G_CALLBACK (vnr_window_cmd_fit) },
    { "ControlEqual", GTK_STOCK_ZOOM_IN, N_("_Zoom In"), "<control>equal",
      N_("Shrink the image"),
      G_CALLBACK (vnr_window_cmd_zoom_in) },
    { "ControlKpAdd", GTK_STOCK_ZOOM_IN, N_("_Zoom In"), "<control>KP_Add",
      N_("Shrink the image"),
      G_CALLBACK (vnr_window_cmd_zoom_in) },
    { "ControlKpSub", GTK_STOCK_ZOOM_OUT, N_("Zoom _Out"), "<control>KP_Subtract",
      N_("Shrink the image"),
      G_CALLBACK (vnr_window_cmd_zoom_out) },
};

static const GtkActionEntry action_entries_collection[] = {
    { "GoPrevious", GTK_STOCK_GO_BACK, N_("_Previous Image"), "<Alt>Left",
      N_("Go to the previous image of the collection"),
      G_CALLBACK (vnr_window_cmd_prev) },
    { "GoNext", GTK_STOCK_GO_FORWARD, N_("_Next Image"), "<Alt>Right",
      N_("Go to the next image of the collection"),
      G_CALLBACK (vnr_window_cmd_next) },
    { "GoFirst", GTK_STOCK_GOTO_FIRST, N_("_First Image"), "<Alt>Home",
      N_("Go to the first image of the collection"),
      G_CALLBACK (vnr_window_cmd_first) },
    { "GoLast", GTK_STOCK_GOTO_LAST, N_("_Last Image"), "<Alt>End",
      N_("Go to the last image of the collection"),
      G_CALLBACK (vnr_window_cmd_last) },
};

static gint
vnr_window_key_press (GtkWidget *widget, GdkEventKey *event)
{

    GtkContainer *tbcontainer = GTK_CONTAINER (VNR_WINDOW (widget)->toolbar);
    gint result = FALSE;

    switch(event->keyval){
        case GDK_Left:
        if (event->state & GDK_MOD1_MASK) {
            vnr_window_cmd_prev (NULL, VNR_WINDOW (widget));
            result = TRUE;
            break;
        } /* else fall-trough is intended */
    case GDK_Up:
        if (scrollbars_visible (VNR_WINDOW (widget))) {
            /* break to let scrollview handle the key */
            break;
        }
        if (tbcontainer->focus_child != NULL)
            break;

        vnr_window_cmd_prev (NULL, VNR_WINDOW (widget));
        result = TRUE;
        break;
    case GDK_Right:
        if (event->state & GDK_MOD1_MASK) {
            vnr_window_cmd_next (NULL, VNR_WINDOW (widget));
            result = TRUE;
            break;
        } /* else fall-trough is intended */
    case GDK_Down:
        if (scrollbars_visible (VNR_WINDOW (widget))) {
            /* break to let scrollview handle the key */
            break;
        }
        if (tbcontainer->focus_child != NULL)
            break;

        vnr_window_cmd_next (NULL, VNR_WINDOW (widget));
        result = TRUE;
        break;
    }

    if (result == FALSE && GTK_WIDGET_CLASS (vnr_window_parent_class)->key_press_event) {
        result = (* GTK_WIDGET_CLASS (vnr_window_parent_class)->key_press_event) (widget, event);
    }

    return result;
}


static void
menu_bar_allocate_cb (GtkWidget *widget, GtkAllocation *alloc, VnrWindow *window)
{
    /* widget is the VnrMenuBar */
    g_signal_handlers_disconnect_by_func(widget, menu_bar_allocate_cb, window);
    vnr_window_open(window, TRUE);
}

static void
vnr_window_init (VnrWindow * window)
{
    GError *error = NULL;
    window->file_list = NULL;

    gtk_window_set_title ((GtkWindow *) window, "Viewnior");
    gtk_window_set_default_icon_name ("viewnior");

    window->max_width = gdk_screen_width () * 0.7;
    window->max_height = gdk_screen_height () * 0.7;

    /* Build MENUBAR and TOOLBAR */
    window->ui_mngr = gtk_ui_manager_new();

    window->actions_window = gtk_action_group_new("MenuActionsWindow");


    gtk_action_group_set_translation_domain (window->actions_window,
                                             GETTEXT_PACKAGE);

    gtk_action_group_add_actions (window->actions_window,
                                  action_entries_window,
                                  G_N_ELEMENTS (action_entries_window),
                                  window);

    gtk_ui_manager_insert_action_group (window->ui_mngr,
                                        window->actions_window, 0);


    window->actions_image = gtk_action_group_new("MenuActionsImage");


    gtk_action_group_set_translation_domain (window->actions_image,
                                             GETTEXT_PACKAGE);

    gtk_action_group_add_actions (window->actions_image,
                                  action_entries_image,
                                  G_N_ELEMENTS (action_entries_image),
                                  window);

    gtk_ui_manager_insert_action_group (window->ui_mngr,
                                        window->actions_image, 0);


    window->actions_collection = gtk_action_group_new("MenuActionsCollection");


    gtk_action_group_set_translation_domain (window->actions_collection,
                                             GETTEXT_PACKAGE);

    gtk_action_group_add_actions (window->actions_collection,
                                  action_entries_collection,
                                  G_N_ELEMENTS (action_entries_collection),
                                  window);

    gtk_ui_manager_insert_action_group (window->ui_mngr,
                                        window->actions_collection, 0);

    if (!gtk_ui_manager_add_ui_from_file (window->ui_mngr,
                                          VNR_DATA_DIR"/viewnior-ui.xml",
                                          &error)) {
            g_warning ("building menus failed: %s\n", error->message);
            g_error_free (error);
            g_return_if_reached();
    }

    gtk_action_group_set_sensitive(window->actions_collection, FALSE);
    gtk_action_group_set_sensitive(window->actions_image, FALSE);

    /* Continue with layout */

    window->layout = gtk_vbox_new(FALSE,0);
    gtk_container_add (GTK_CONTAINER (window), window->layout);
    gtk_widget_show(window->layout);

    window->menus = gtk_vbox_new(FALSE,0);
    gtk_box_pack_start (GTK_BOX (window->layout), window->menus, FALSE,FALSE,0);

    window->menu_bar = gtk_ui_manager_get_widget (window->ui_mngr, "/MainMenu");
    g_assert(GTK_IS_WIDGET(window->menu_bar));
    gtk_box_pack_start (GTK_BOX (window->menus), window->menu_bar, FALSE,FALSE,0);

    window->toolbar = gtk_ui_manager_get_widget (window->ui_mngr, "/Toolbar");
    g_assert(GTK_IS_WIDGET(window->toolbar));
    gtk_toolbar_set_style(GTK_TOOLBAR(window->toolbar), GTK_TOOLBAR_ICONS);
    gtk_box_pack_start (GTK_BOX (window->menus), window->toolbar, FALSE,FALSE,0);

    gtk_widget_show_all(window->menus);

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

    g_signal_connect (G_OBJECT (window->menus), "size-allocate",
                      G_CALLBACK (menu_bar_allocate_cb), window);

    gtk_window_add_accel_group (GTK_WINDOW (window),
                gtk_ui_manager_get_accel_group (window->ui_mngr));

    gtk_widget_grab_focus(window->view);
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

    if(win->file_list == NULL)
        return FALSE;

    file = VNR_FILE(win->file_list->data);

    gtk_window_set_title (GTK_WINDOW (win), file->display_name);

    pixbuf = gdk_pixbuf_animation_new_from_file (file->uri, &error);

    if (error != NULL)
    {
        gtk_action_group_set_sensitive(win->actions_image, FALSE);
        vnr_message_area_show_warning(VNR_MESSAGE_AREA (win->msg_area), error->message);
        return FALSE;
    }
    else
    {
        gtk_action_group_set_sensitive(win->actions_image, TRUE);
    }

    if(fit_to_screen)
    {
        gint img_h, img_w;          /* Width and Height of the pixbuf */

        img_w = gdk_pixbuf_animation_get_width (pixbuf);
        img_h = gdk_pixbuf_animation_get_height (pixbuf);

        vnr_tools_fit_to_size (&img_w, &img_h, win->max_width, win->max_height);

        gtk_window_resize (GTK_WINDOW (win), img_w, img_h+win->menus->allocation.height);
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
    if (g_list_length(list) != 1)
        gtk_action_group_set_sensitive(win->actions_collection, TRUE);
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

gboolean
vnr_window_first (VnrWindow *win){
    GList *prev;

    prev = g_list_first(win->file_list);

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

gboolean
vnr_window_last (VnrWindow *win){
    GList *prev;

    prev = g_list_last(win->file_list);

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
