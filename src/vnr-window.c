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
#include <glib/gstdio.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#ifdef HAVE_WALLPAPER
#include <gconf/gconf-client.h>
#endif /* HAVE_WALLPAPER */
#include <errno.h>
#include "vnr-window.h"
#include "uni-scroll-win.h"
#include "uni-anim-view.h"
#include "vnr-tools.h"
#include "vnr-message-area.h"

G_DEFINE_TYPE (VnrWindow, vnr_window, GTK_TYPE_WINDOW);

static void vnr_window_unfullscreen (VnrWindow *window);
static void stop_slideshow(VnrWindow *window);
static void start_slideshow(VnrWindow *window);
static void restart_slideshow(VnrWindow *window);
static void allow_slideshow(VnrWindow *window);

const gchar *ui_definition = "<ui>"
  "<menubar name=\"MainMenu\">"
    "<menu action=\"File\">"
      "<menuitem action=\"FileOpen\"/>"
      "<menuitem action=\"FileOpenDir\"/>"
      "<separator/>"
      "<menuitem action=\"FileDelete\"/>"
      "<separator/>"
      "<menuitem action=\"FileClose\"/>"
    "</menu>"
    "<menu action=\"View\">"
      "<menuitem action=\"ViewZoomIn\"/>"
      "<menuitem action=\"ViewZoomOut\"/>"
      "<menuitem action=\"ViewZoomNormal\"/>"
      "<menuitem action=\"ViewZoomFit\"/>"
      "<separator/>"
      "<menuitem name=\"Fullscreen\" action=\"ViewFullscreen\"/>"
      "<menuitem action=\"ViewResizeWindow\"/>"
    "</menu>"
    "<menu action=\"Image\">"
      "<menuitem action=\"ImageFlipVertical\"/>"
      "<menuitem action=\"ImageFlipHorizontal\"/>"
      "<separator/>"
      "<menuitem action=\"ImageRotateCW\"/>"
      "<menuitem action=\"ImageRotateCCW\"/>"
#ifdef HAVE_WALLPAPER
      "<separator/>"
      "<menuitem action=\"SetAsWallpaper\"/>"
#endif /* HAVE_WALLPAPER */
    "</menu>"
    "<menu action=\"Go\">"
      "<menuitem name=\"GoPrevious\" action=\"GoPrevious\"/>"
      "<menuitem name=\"GoNext\" action=\"GoNext\"/>"
      "<separator/>"
      "<menuitem name=\"GoFirst\" action=\"GoFirst\"/>"
      "<menuitem name=\"GoLast\" action=\"GoLast\"/>"
    "</menu>"
    "<menu action=\"Help\">"
      "<menuitem action=\"HelpAbout\"/>"
    "</menu>"
  "</menubar>"
  "<toolbar name=\"Toolbar\">"
    "<toolitem action=\"GoPrevious\"/>"
    "<toolitem action=\"GoNext\"/>"
    "<separator/>"
    "<toolitem action=\"ViewZoomIn\"/>"
    "<toolitem action=\"ViewZoomOut\"/>"
    "<toolitem action=\"ViewZoomNormal\"/>"
    "<toolitem action=\"ViewZoomFit\"/>"
    "<separator/>"
    "<toolitem action=\"ImageRotateCCW\"/>"
    "<toolitem action=\"ImageRotateCW\"/>"
  "</toolbar>"
  "<accelerator name=\"ControlEqualAccel\" action=\"ControlEqual\"/>"
  "<accelerator name=\"ControlKPAddAccel\" action=\"ControlKpAdd\"/>"
  "<accelerator name=\"ControlKPSubAccel\" action=\"ControlKpSub\"/>"
  "<accelerator name=\"DeleteAccel\" action=\"Delete\"/>"
"</ui>";

/*************************************************************/
/***** Private actions ***************************************/
/*************************************************************/

G_GNUC_UNUSED static void
dumb (GtkAction *action, gpointer user_data)
{
    printf("Dumb!\n");
}

static void
update_fs_label(VnrWindow *window)
{
    if(window->mode == VNR_WINDOW_MODE_NORMAL)
        return;

    gtk_label_set_text(GTK_LABEL(window->fs_label),
                       VNR_FILE(window->file_list->data)->display_name);
}

static void
leave_fs_cb (GtkButton *button, VnrWindow *window)
{
    vnr_window_unfullscreen (window);
}

static gboolean
next_image_src(VnrWindow *window)
{
    if(g_list_length(g_list_first(window->file_list)) <= 1)
        return FALSE;
    else
        vnr_window_next(window, FALSE);

    window->source_tag = g_timeout_add_seconds (window->timeout,
                                                (GSourceFunc)next_image_src,
                                                window);

    return FALSE;
}

static void
spin_value_change_cb (GtkSpinButton *spinbutton, VnrWindow *window)
{
    window->timeout = gtk_spin_button_get_value_as_int (spinbutton);
    restart_slideshow(window);
}

static void
toggle_show_next_cb (GtkToggleButton *togglebutton, VnrWindow *window)
{
    if(!window->slideshow)
        return;

    if(window->mode == VNR_WINDOW_MODE_FULLSCREEN)
        start_slideshow(window);
    else if(window->mode == VNR_WINDOW_MODE_SLIDESHOW)
        stop_slideshow(window);
}

static GtkWidget *
get_fs_controls(VnrWindow *window)
{
    if(window->fs_controls != NULL)
        return window->fs_controls;

    GtkWidget *box;
    GtkToolItem *item;
    GtkWidget *widget;
    GtkAdjustment *spinner_adj;



    item = gtk_tool_item_new();

    box = gtk_hbox_new(FALSE, 0);
    gtk_container_add (GTK_CONTAINER (item), box);

    widget = gtk_button_new_from_stock(GTK_STOCK_LEAVE_FULLSCREEN);
    g_signal_connect(widget, "clicked", G_CALLBACK(leave_fs_cb), window);

    gtk_box_pack_end (GTK_BOX(box), widget, FALSE, FALSE, 0);

    widget = gtk_label_new(NULL);
    gtk_label_set_ellipsize (GTK_LABEL(widget), PANGO_ELLIPSIZE_END);
    gtk_label_set_selectable (GTK_LABEL(widget), TRUE);
    window->fs_label = widget;
    gtk_box_pack_end (GTK_BOX(box), widget, TRUE, TRUE, 10);


    widget = gtk_vseparator_new();
    gtk_box_pack_start (GTK_BOX(box), widget, FALSE, FALSE, 0);

    widget = gtk_check_button_new_with_label(_("Show next image after: "));
    g_signal_connect(widget, "toggled", G_CALLBACK(toggle_show_next_cb), window);
    gtk_box_pack_start (GTK_BOX(box), widget, FALSE, FALSE, 0);
    window->toggle_btn = widget;

    spinner_adj = (GtkAdjustment *) gtk_adjustment_new (5, 1.0, 30.0, 1.0, 1.0, 0);

    widget = gtk_spin_button_new (spinner_adj, 1.0, 0);
    gtk_spin_button_set_snap_to_ticks(GTK_SPIN_BUTTON(widget), TRUE);
    gtk_spin_button_set_update_policy(GTK_SPIN_BUTTON(widget), GTK_UPDATE_ALWAYS);

    g_signal_connect(widget, "value-changed", G_CALLBACK(spin_value_change_cb), window);
    gtk_box_pack_start (GTK_BOX(box), widget, FALSE, FALSE, 0);

    widget = gtk_label_new(_(" seconds"));
    gtk_box_pack_start (GTK_BOX(box), widget, FALSE, FALSE, 0);

    gtk_tool_item_set_expand(item, TRUE);

    window->fs_controls = GTK_WIDGET(item);

    gtk_widget_show_all (window->fs_controls);
    gtk_object_ref((gpointer)window->fs_controls);
    return window->fs_controls;
}

static gboolean
scrollbars_visible (VnrWindow *win)
{
    if (!GTK_WIDGET_VISIBLE (GTK_WIDGET (UNI_SCROLL_WIN(win->scroll_view)->hscroll)) &&
        !GTK_WIDGET_VISIBLE (GTK_WIDGET (UNI_SCROLL_WIN(win->scroll_view)->vscroll)))
        return FALSE;

    return TRUE;
}

static void
vnr_window_set_drag(VnrWindow *window)
{
    gtk_drag_dest_set (GTK_WIDGET (window),
                       GTK_DEST_DEFAULT_MOTION | GTK_DEST_DEFAULT_DROP,
                       NULL, 0,
                       GDK_ACTION_COPY | GDK_ACTION_ASK);
    gtk_drag_dest_add_uri_targets (GTK_WIDGET (window));
}

static void
vnr_window_fullscreen(VnrWindow *window)
{
    GdkColor color;
    GtkAction *action;

    gdk_color_parse ("black", &color);

    gtk_widget_hide(window->menu_bar);
    gtk_window_fullscreen(GTK_WINDOW(window));

    window->mode = VNR_WINDOW_MODE_FULLSCREEN;
    action = gtk_action_group_get_action (window->actions_image,
                                          "ViewFullscreen");

    gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), TRUE);
    gtk_widget_modify_bg(window->view, GTK_STATE_NORMAL, &color);

    update_fs_label(window);
    gtk_widget_show (get_fs_controls(window));

    stop_slideshow(window);
}

static void
vnr_window_unfullscreen(VnrWindow *window)
{
    if(window->mode == VNR_WINDOW_MODE_NORMAL)
        return;

    GtkAction *action;

    stop_slideshow(window);
    window->mode = VNR_WINDOW_MODE_NORMAL;

    gtk_widget_show(window->menu_bar);
    gtk_window_unfullscreen(GTK_WINDOW(window));
    action = gtk_action_group_get_action (window->actions_image,
                                          "ViewFullscreen");

    gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), FALSE);
    gtk_widget_modify_bg(window->view, GTK_STATE_NORMAL, NULL);

    gtk_widget_hide (get_fs_controls(window));
}

static void
stop_slideshow(VnrWindow *window)
{

    if(!window->slideshow)
        return;

    window->slideshow = FALSE;
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(window->toggle_btn), FALSE);
    window->slideshow = TRUE;

    if(window->mode != VNR_WINDOW_MODE_SLIDESHOW)
        return;

    window->mode = VNR_WINDOW_MODE_FULLSCREEN;

    g_source_remove (window->source_tag);
}

static void
start_slideshow(VnrWindow *window)
{
    if(!window->slideshow)
        return;

    if(window->mode == VNR_WINDOW_MODE_SLIDESHOW)
        return;

    window->mode = VNR_WINDOW_MODE_SLIDESHOW;

    window->source_tag = g_timeout_add_seconds (window->timeout,
                                                (GSourceFunc)next_image_src,
                                                window);

    window->slideshow = FALSE;
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(window->toggle_btn), TRUE);
    window->slideshow = TRUE;
}

static void
restart_slideshow(VnrWindow *window)
{
    if(!window->slideshow)
        return;

    if(window->mode != VNR_WINDOW_MODE_SLIDESHOW)
        return;

    g_source_remove (window->source_tag);
    window->source_tag = g_timeout_add_seconds (window->timeout,
                                                (GSourceFunc)next_image_src,
                                                window);
}

static void
allow_slideshow(VnrWindow *window)
{
    if(window->slideshow)
        return;

    window->slideshow = TRUE;

    gtk_widget_set_sensitive(window->toggle_btn, TRUE);
}

void
deny_slideshow(VnrWindow *window)
{
    if(!window->slideshow)
        return;

    window->slideshow = FALSE;

    gtk_widget_set_sensitive(window->toggle_btn, FALSE);
}

static void
rotate_pixbuf(VnrWindow *window, GdkPixbufRotation angle)
{
    GdkPixbuf *result;

    gdk_window_set_cursor(GTK_WIDGET(window)->window, gdk_cursor_new(GDK_WATCH));
    /* This makes the cursor show NOW */
    gtk_main_iteration_do (FALSE);

    result = gdk_pixbuf_rotate_simple(UNI_IMAGE_VIEW(window->view)->pixbuf,
                                      angle);

    if(result == NULL)
    {
        vnr_message_area_show_warning(VNR_MESSAGE_AREA(window->msg_area),
                                      _("Not enough virtual memory."),
                                      FALSE);
        return;
    }

    uni_anim_view_set_static(UNI_ANIM_VIEW(window->view), result);

    gdk_window_set_cursor(GTK_WIDGET(window)->window, gdk_cursor_new(GDK_LEFT_PTR));
    g_object_unref(result);
}

static void
flip_pixbuf(VnrWindow *window, gboolean horizontal)
{
    GdkPixbuf *result;

    gdk_window_set_cursor(GTK_WIDGET(window)->window, gdk_cursor_new(GDK_WATCH));
    /* This makes the cursor show NOW */
    gtk_main_iteration_do (FALSE);

    result = gdk_pixbuf_flip(UNI_IMAGE_VIEW(window->view)->pixbuf,
                             horizontal);

    if(result == NULL)
    {
        vnr_message_area_show_warning(VNR_MESSAGE_AREA(window->msg_area),
                                      _("Not enough virtual memory."),
                                      FALSE);
        return;
    }

    uni_anim_view_set_static(UNI_ANIM_VIEW(window->view), result);

    gdk_window_set_cursor(GTK_WIDGET(window)->window, gdk_cursor_new(GDK_LEFT_PTR));
    g_object_unref(result);
}

/*************************************************************/
/***** Private signal handlers *******************************/
/*************************************************************/

static void
vnr_window_drag_data_received (GtkWidget *widget,
                               GdkDragContext *context,
                               gint x, gint y,
                               GtkSelectionData *selection_data,
                               guint info, guint time)
{
    GSList *uri_list = NULL;

    if (!gtk_targets_include_uri (&selection_data->target, 1))
        return;

    if (context->suggested_action == GDK_ACTION_COPY)
    {
        uri_list = vnr_tools_parse_uri_string_list_to_file_list ((gchar *) selection_data->data);
        g_return_if_fail(uri_list != NULL);
        vnr_window_open_from_list(VNR_WINDOW (widget), uri_list);
    }
}

static void
menu_bar_allocate_cb (GtkWidget *widget, GtkAllocation *alloc, VnrWindow *window)
{
    /* widget is the VnrMenuBar */
    g_signal_handlers_disconnect_by_func(widget, menu_bar_allocate_cb, window);
    if(!vnr_message_area_get_visible(VNR_MESSAGE_AREA(window->msg_area)))
        vnr_window_open(window, TRUE);
}

static void
vnr_window_cmd_flip_horizontal(GtkAction *action, gpointer user_data)
{
    flip_pixbuf(VNR_WINDOW(user_data), TRUE);
}

static void
vnr_window_cmd_flip_vertical(GtkAction *action, gpointer user_data)
{
    flip_pixbuf(VNR_WINDOW(user_data), FALSE);
}

static void
vnr_window_cmd_rotate_cw(GtkAction *action, gpointer user_data)
{
    rotate_pixbuf(VNR_WINDOW(user_data), GDK_PIXBUF_ROTATE_CLOCKWISE);
}

static void
vnr_window_cmd_rotate_ccw(GtkAction *action, gpointer user_data)
{
    rotate_pixbuf(VNR_WINDOW(user_data), GDK_PIXBUF_ROTATE_COUNTERCLOCKWISE);
}

static void
vnr_window_cmd_zoom_in (GtkAction *action, gpointer user_data)
{
    g_return_if_fail (VNR_IS_WINDOW (user_data));

    if (VNR_WINDOW(user_data)->view)
        uni_image_view_zoom_in(UNI_IMAGE_VIEW(VNR_WINDOW(user_data)->view));
}

static void
vnr_window_cmd_zoom_out (GtkAction *action, gpointer user_data)
{
    g_return_if_fail (VNR_IS_WINDOW (user_data));

    if (VNR_WINDOW(user_data)->view)
        uni_image_view_zoom_out(UNI_IMAGE_VIEW(VNR_WINDOW(user_data)->view));
}

static void
vnr_window_cmd_normal_size (GtkAction *action, gpointer user_data)
{
    g_return_if_fail (VNR_IS_WINDOW (user_data));

    if (VNR_WINDOW(user_data)->view)
        uni_image_view_set_zoom(UNI_IMAGE_VIEW(VNR_WINDOW(user_data)->view), 1);
}

static void
vnr_window_cmd_fit (GtkAction *action, gpointer user_data)
{
    g_return_if_fail (VNR_IS_WINDOW (user_data));

    if (VNR_WINDOW(user_data)->view)
        uni_image_view_set_fitting(UNI_IMAGE_VIEW(VNR_WINDOW(user_data)->view), UNI_FITTING_FULL);
}

static void
vnr_window_cmd_next (GtkAction *action, gpointer user_data)
{
    g_return_if_fail (VNR_IS_WINDOW (user_data));

    if (VNR_WINDOW(user_data)->view)
        vnr_window_next(VNR_WINDOW(user_data), TRUE);
}

static void
vnr_window_cmd_first (GtkAction *action, gpointer user_data)
{
    g_return_if_fail (VNR_IS_WINDOW (user_data));

    if (VNR_WINDOW(user_data)->view)
        vnr_window_first(VNR_WINDOW(user_data));
}

static void
vnr_window_cmd_last (GtkAction *action, gpointer user_data)
{
    g_return_if_fail (VNR_IS_WINDOW (user_data));

    if (VNR_WINDOW(user_data)->view)
        vnr_window_last(VNR_WINDOW(user_data));
}

static void
vnr_window_cmd_prev (GtkAction *action, gpointer user_data)
{
    g_return_if_fail (VNR_IS_WINDOW (user_data));

    if (VNR_WINDOW(user_data)->view)
        vnr_window_prev(VNR_WINDOW(user_data));
}

static void
file_open_dialog_response_cb (GtkWidget *dialog,
                              gint response_id,
                              VnrWindow *window)
{
    if (response_id == GTK_RESPONSE_ACCEPT)
    {
        GSList *uri_list = NULL;
        uri_list = gtk_file_chooser_get_filenames (GTK_FILE_CHOOSER (dialog));
        g_return_if_fail(uri_list != NULL);
        vnr_window_open_from_list(window, uri_list);
    }

    gtk_widget_destroy (dialog);
}

static void
vnr_window_cmd_resize (GtkAction *action, gpointer user_data)
{
    VnrWindow *win;
    GdkPixbufAnimation *pixbuf;
    gint img_h, img_w;          /* Width and Height of the pixbuf */

    g_return_if_fail (VNR_IS_WINDOW (user_data));
    win = VNR_WINDOW(user_data);

    pixbuf = uni_anim_view_get_anim(UNI_ANIM_VIEW(win->view));

    img_w = gdk_pixbuf_animation_get_width (pixbuf);
    img_h = gdk_pixbuf_animation_get_height (pixbuf);

    vnr_tools_fit_to_size (&img_w, &img_h, win->max_width, win->max_height);

    gtk_window_resize (GTK_WINDOW (win), img_w, img_h+win->menus->allocation.height);
}

static void
vnr_window_cmd_open(GtkAction *action, gpointer user_data)
{
    g_return_if_fail (VNR_IS_WINDOW (user_data));
    VnrWindow *window;
    window = VNR_WINDOW(user_data);

    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new (_("Open Image"),
                          GTK_WINDOW(user_data),
                          GTK_FILE_CHOOSER_ACTION_OPEN,
                          GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                          GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                          NULL);

    gtk_window_set_modal (GTK_WINDOW(dialog), FALSE);
    gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialog), TRUE);

    gchar *filename;
    if(window->file_list != NULL)
    {
        filename = g_path_get_dirname (VNR_FILE(window->file_list->data)->uri);
        gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER(dialog), filename);
    }

    g_signal_connect (dialog, "response",
                      G_CALLBACK (file_open_dialog_response_cb),
                      window);

    gtk_widget_show_all (GTK_WIDGET(dialog));
}

static void
vnr_window_cmd_open_dir(GtkAction *action, gpointer user_data)
{
    g_return_if_fail (VNR_IS_WINDOW (user_data));
    VnrWindow *window;
    window = VNR_WINDOW(user_data);

    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new (_("Open Folder"),
                          GTK_WINDOW(user_data),
                          GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                          GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                          GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                          NULL);

    gtk_window_set_modal (GTK_WINDOW(dialog), FALSE);
    gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialog), TRUE);

    gchar *filename;
    if(window->file_list != NULL)
    {
        filename = g_path_get_dirname (VNR_FILE(window->file_list->data)->uri);
        gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER(dialog), filename);
    }

    g_signal_connect (dialog, "response",
                      G_CALLBACK (file_open_dialog_response_cb),
                      window);

    gtk_widget_show_all (GTK_WIDGET(dialog));
}

static void
vnr_window_cmd_about (GtkAction *action, gpointer user_data)
{
    VnrWindow *window;

    g_return_if_fail (VNR_IS_WINDOW (user_data));

    static const char *authors[] = {
        "Programming & icon design",
        "\tSiyan Panayotov <xsisqox@gmail.com>",
        "\nRefer to source code from GtkImageView",
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

#ifdef HAVE_WALLPAPER
static void
vnr_set_wallpaper(GtkAction *action, gpointer user_data)
{
    gconf_client_set_string (VNR_WINDOW(user_data)->client,
                 "/desktop/gnome/background/picture_filename",
                 VNR_FILE(VNR_WINDOW(user_data)->file_list->data)->uri,
                 NULL);

}
#endif /* HAVE_WALLPAPER */

static void
vnr_window_cmd_fullscreen (GtkAction *action, gpointer user_data)
{
    VnrWindow *window;
    gboolean fullscreen;

    g_return_if_fail (VNR_IS_WINDOW (user_data));
    window = VNR_WINDOW(user_data);

    fullscreen = gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action));

    if (fullscreen)
        vnr_window_fullscreen (window);
    else
        vnr_window_unfullscreen (window);
}

/* Modified version of eog's eog_window_key_press */
static gint
vnr_window_key_press (GtkWidget *widget, GdkEventKey *event)
{

    GtkContainer *tbcontainer = GTK_CONTAINER (VNR_WINDOW (widget)->toolbar);
    gint result = FALSE;

    switch(event->keyval){
        case GDK_Left:
            if (event->state & GDK_MOD1_MASK)
            {
                vnr_window_cmd_prev (NULL, VNR_WINDOW (widget));
                result = TRUE;
                break;
            } /* else fall-trough is intended */
        case GDK_Up:
            if (scrollbars_visible (VNR_WINDOW (widget)))
            {
                /* break to let scrollview handle the key */
                break;
            }
            if (tbcontainer->focus_child != NULL)
                break;

            vnr_window_cmd_prev (NULL, VNR_WINDOW (widget));
            result = TRUE;
            break;
        case GDK_Right:
            if (event->state & GDK_MOD1_MASK)
            {
                vnr_window_cmd_next (NULL, VNR_WINDOW (widget));
                result = TRUE;
                break;
            } /* else fall-trough is intended */
        case GDK_Down:
            if (scrollbars_visible (VNR_WINDOW (widget)))
            {
                /* break to let scrollview handle the key */
                break;
            }
            if (tbcontainer->focus_child != NULL)
                break;

            vnr_window_cmd_next (NULL, VNR_WINDOW (widget));
            result = TRUE;
            break;
        case GDK_Escape:
            if(VNR_WINDOW (widget)->mode != VNR_WINDOW_MODE_NORMAL)
                vnr_window_unfullscreen(VNR_WINDOW (widget));
            break;
        case GDK_space:
            if(VNR_WINDOW (widget)->mode == VNR_WINDOW_MODE_FULLSCREEN)
            {
                start_slideshow(VNR_WINDOW (widget));
                result = TRUE;
            }
            else if(VNR_WINDOW (widget)->mode == VNR_WINDOW_MODE_SLIDESHOW)
            {
                stop_slideshow(VNR_WINDOW (widget));
                result = TRUE;
            }

            break;
    }

    if (result == FALSE && GTK_WIDGET_CLASS (vnr_window_parent_class)->key_press_event)
        result = (* GTK_WIDGET_CLASS (vnr_window_parent_class)->key_press_event) (widget, event);

    return result;
}

static void
vnr_window_cmd_delete(GtkAction *action, gpointer user_data)
{
    GtkWidget *dlg;
    const gchar *file_path;
    gchar *markup, *prompt, *warning;
    gboolean restart_slideshow = FALSE;

    g_return_if_fail (VNR_IS_WINDOW (user_data));
    VnrWindow *window = VNR_WINDOW(user_data);

    if(window->mode == VNR_WINDOW_MODE_SLIDESHOW)
    {
       stop_slideshow(window);
       restart_slideshow = TRUE;
    }

    g_return_if_fail (window->file_list != NULL);

    file_path = VNR_FILE(window->file_list->data)->uri;

    warning = _("If you delete an item, it will be permanently lost.");

    /* I18N: The '%s' is replaced with the name of the file to be deleted. */
    prompt = g_strdup_printf (_("Are you sure you want to\n"
                                "permanently delete \"%s\"?"),
                              VNR_FILE(window->file_list->data)->display_name);
    markup = g_strdup_printf ("<span weight=\"bold\" size=\"larger\">%s</span>\n\n%s",
                              prompt, warning);


    dlg = gtk_message_dialog_new(GTK_WINDOW(user_data),
                                 GTK_DIALOG_MODAL,
                                 GTK_MESSAGE_WARNING,
                                 GTK_BUTTONS_NONE,
                                 NULL);

    gtk_message_dialog_set_markup (GTK_MESSAGE_DIALOG (dlg),
                                   markup);

    gtk_dialog_add_buttons (GTK_DIALOG (dlg),
                            GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                            GTK_STOCK_DELETE, GTK_RESPONSE_YES,
                            NULL);

    if( gtk_dialog_run(GTK_DIALOG(dlg)) == GTK_RESPONSE_YES )
    {
        if( g_unlink(file_path) != 0 )
        {
            /* I18N: The '%s' is replaced with error message. */
            vnr_message_area_show_warning(VNR_MESSAGE_AREA (window->msg_area),
                                          g_strdup_printf (_("Error deleting image: %s"),
                                           g_strerror(errno)),
                                          FALSE);
            restart_slideshow = FALSE;
        }
        else
        {
            GList *new, *next;

            next = g_list_next(window->file_list);
            if(next == NULL)
                next = g_list_first(window->file_list);

            if(g_list_length(g_list_first(window->file_list)) != 1)
                new = g_list_delete_link (window->file_list, window->file_list);
            else
            {
                g_list_free(window->file_list);
                next = NULL;
            }

            if(next == NULL)
            {
                vnr_window_close(window);
                gtk_action_group_set_sensitive(window->actions_collection, FALSE);
                deny_slideshow(window);
                vnr_message_area_show_warning(VNR_MESSAGE_AREA (window->msg_area),
                                              _("The given locations contain no images."), TRUE);
                restart_slideshow = FALSE;
            }
            else
            {
                vnr_window_set_list(window, next, FALSE);
                gdk_window_set_cursor(GTK_WIDGET(dlg)->window, gdk_cursor_new(GDK_WATCH));
                gtk_main_iteration_do (FALSE);

                vnr_window_close(window);
                vnr_window_open(window, FALSE);
                gdk_window_set_cursor(GTK_WIDGET(dlg)->window, gdk_cursor_new(GDK_LEFT_PTR));
            }
        }
    }
    if(restart_slideshow)
       start_slideshow(window);

    g_free(prompt);
    g_free(markup);
    gtk_widget_destroy( dlg );
}

static gint
vnr_window_delete (GtkWidget * widget, GdkEventAny * event)
{
    gtk_widget_destroy (widget);
    return TRUE;
}

static const GtkActionEntry action_entries_window[] = {
    { "File",  NULL, N_("_File") },
    { "View",  NULL, N_("_View") },
    { "Image",  NULL, N_("_Image") },
    { "Go",    NULL, N_("_Go") },
    { "Help",  NULL, N_("_Help") },

    { "FileOpen", GTK_STOCK_FILE, N_("Open _Image..."), "<control>O",
      N_("Open an Image"),
      G_CALLBACK (vnr_window_cmd_open) },
    { "FileOpenDir", GTK_STOCK_DIRECTORY, N_("Open _Folder..."), "<control>F",
      N_("Open a Folder"),
      G_CALLBACK (vnr_window_cmd_open_dir) },
    { "FileClose", GTK_STOCK_CLOSE, N_("_Close"), "<control>W",
      N_("Close window"),
      G_CALLBACK (gtk_main_quit) },
    { "HelpAbout", GTK_STOCK_ABOUT, N_("_About"), NULL,
      N_("About this application"),
      G_CALLBACK (vnr_window_cmd_about) }
};

static const GtkActionEntry action_entries_image[] = {
#ifdef HAVE_WALLPAPER
    { "SetAsWallpaper", NULL, N_("Set as _Wallpaper"), "<control>F8",
      N_("Set the selected image as the desktop background"),
      G_CALLBACK (vnr_set_wallpaper) },
#endif /* HAVE_WALLPAPER */
    { "FileDelete", GTK_STOCK_DELETE, N_("_Delete"), NULL,
      N_("Delete the current file"),
      G_CALLBACK (vnr_window_cmd_delete) },
    { "Delete", NULL, N_("_Delete"), "Delete",
      N_("Delete the current file"),
      G_CALLBACK (vnr_window_cmd_delete) },
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
    { "ViewResizeWindow", NULL, N_("_Adjust window size"), NULL,
      N_("Adjust window size to fit the image"),
      G_CALLBACK (vnr_window_cmd_resize) },
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

static const GtkActionEntry action_entries_static_image[] = {
    { "ImageRotateCW", "object-rotate-right", N_("Rotate _Clockwise"), "<control>R",
      N_("Rotate image clockwise"),
      G_CALLBACK (vnr_window_cmd_rotate_cw) },
    { "ImageRotateCCW", "object-rotate-left", N_("Rotate _Anti-clockwise"), "<control><shift>R",
      N_("Rotate image anti-clockwise"),
      G_CALLBACK (vnr_window_cmd_rotate_ccw) },
    { "ImageFlipVertical", "object-flip-vertical", N_("Flip _Vertical"), NULL,
      N_("Flip image vertically"),
      G_CALLBACK (vnr_window_cmd_flip_vertical) },
    { "ImageFlipHorizontal", "object-flip-horizontal", N_("Flip _Horizontal"), NULL,
      N_("Flip image horizontally"),
      G_CALLBACK (vnr_window_cmd_flip_horizontal) },
};

static const GtkToggleActionEntry toggle_entries_image[] = {
    { "ViewFullscreen", GTK_STOCK_FULLSCREEN, N_("Full _Screen"), "F11",
      N_("Show in fullscreen mode"),
      G_CALLBACK (vnr_window_cmd_fullscreen) },
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

/*************************************************************/
/***** Stuff that deals with the type ************************/
/*************************************************************/
static void
vnr_window_class_init (VnrWindowClass * klass)
{
    GtkWidgetClass *widget_class = (GtkWidgetClass *) klass;
    widget_class->delete_event = vnr_window_delete;
    widget_class->key_press_event = vnr_window_key_press;
    widget_class->drag_data_received = vnr_window_drag_data_received;
}

GtkWindow *
vnr_window_new ()
{
    return (GtkWindow *) g_object_new (VNR_TYPE_WINDOW, NULL);
}

static void
vnr_window_init (VnrWindow * window)
{
    GError *error = NULL;
    window->file_list = NULL;
    window->fs_controls = NULL;
    window->timeout = 5;
    window->slideshow = TRUE;

#ifdef HAVE_WALLPAPER
    window->client = gconf_client_get_default ();
#endif /* HAVE_WALLPAPER */

    window->mode = VNR_WINDOW_MODE_NORMAL;

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

    window->actions_static_image = gtk_action_group_new("MenuActionsStaticImage");


    gtk_action_group_set_translation_domain (window->actions_static_image,
                                             GETTEXT_PACKAGE);

    gtk_action_group_add_actions (window->actions_static_image,
                                  action_entries_static_image,
                                  G_N_ELEMENTS (action_entries_static_image),
                                  window);

    gtk_ui_manager_insert_action_group (window->ui_mngr,
                                        window->actions_static_image, 0);


    window->actions_image = gtk_action_group_new("MenuActionsImage");


    gtk_action_group_set_translation_domain (window->actions_image,
                                             GETTEXT_PACKAGE);

    gtk_action_group_add_actions (window->actions_image,
                                  action_entries_image,
                                  G_N_ELEMENTS (action_entries_image),
                                  window);
    gtk_action_group_add_toggle_actions (window->actions_image,
                                         toggle_entries_image,
                                         G_N_ELEMENTS (toggle_entries_image),
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

    if (!gtk_ui_manager_add_ui_from_string (window->ui_mngr,
                                            ui_definition, -1,
                                            &error)) {
            g_error ("building menus failed: %s\n", error->message);
            g_error_free (error);
    }

    gtk_action_group_set_sensitive(window->actions_collection, FALSE);
    gtk_action_group_set_sensitive(window->actions_image, FALSE);
    gtk_action_group_set_sensitive(window->actions_static_image, FALSE);

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
    gtk_toolbar_insert (GTK_TOOLBAR (window->toolbar),
                        GTK_TOOL_ITEM(get_fs_controls(window)), -1);
    gtk_box_pack_start (GTK_BOX (window->menus), window->toolbar, FALSE,FALSE,0);

    gtk_ui_manager_ensure_update (window->ui_mngr);
    gtk_widget_show_all(window->menus);

    gtk_widget_hide(get_fs_controls(window));

    window->msg_area = vnr_message_area_new();
    VNR_MESSAGE_AREA(window->msg_area)->vnr_win = window;
    gtk_box_pack_start (GTK_BOX (window->layout), window->msg_area, FALSE,FALSE,0);
    gtk_widget_show(GTK_WIDGET (window->msg_area));

    window->view = uni_anim_view_new ();
    GTK_WIDGET_SET_FLAGS(window->view, GTK_CAN_FOCUS);
    window->scroll_view = uni_scroll_win_new (UNI_IMAGE_VIEW (window->view));
    gtk_box_pack_end (GTK_BOX (window->layout), window->scroll_view, TRUE,TRUE,0);
    gtk_widget_show_all(GTK_WIDGET (window->scroll_view));

    gtk_widget_grab_focus(window->view);

    vnr_window_set_drag(window);

    g_signal_connect (G_OBJECT (window), "destroy",
                      G_CALLBACK (gtk_main_quit), NULL);

    g_signal_connect (G_OBJECT (window->menus), "size-allocate",
                      G_CALLBACK (menu_bar_allocate_cb), window);

    gtk_window_add_accel_group (GTK_WINDOW (window),
                gtk_ui_manager_get_accel_group (window->ui_mngr));

}

/*************************************************************/
/***** Actions ***********************************************/
/*************************************************************/
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
    update_fs_label(win);

    pixbuf = gdk_pixbuf_animation_new_from_file (file->uri, &error);

    if (error != NULL)
    {
        vnr_message_area_show_warning(VNR_MESSAGE_AREA (win->msg_area), error->message, TRUE);
        return FALSE;
    }

    if(vnr_message_area_get_visible(VNR_MESSAGE_AREA(win->msg_area)))
    {
        vnr_message_area_hide(VNR_MESSAGE_AREA(win->msg_area));
    }

    gtk_action_group_set_sensitive(win->actions_image, TRUE);

    if(fit_to_screen)
    {
        gint img_h, img_w;          /* Width and Height of the pixbuf */

        img_w = gdk_pixbuf_animation_get_width (pixbuf);
        img_h = gdk_pixbuf_animation_get_height (pixbuf);

        vnr_tools_fit_to_size (&img_w, &img_h, win->max_width, win->max_height);

        gtk_window_resize (GTK_WINDOW (win), img_w, img_h+win->menus->allocation.height);
    }

    /* Return TRUE if the image is static */
    if ( uni_anim_view_set_anim (UNI_ANIM_VIEW (win->view), pixbuf) )
        gtk_action_group_set_sensitive(win->actions_static_image, TRUE);
    else
        gtk_action_group_set_sensitive(win->actions_static_image, FALSE);

    g_object_unref(pixbuf);
    return TRUE;
}

void
vnr_window_open_from_list(VnrWindow *window, GSList *uri_list)
{
    GList *file_list = NULL;
    GError *error = NULL;

    if (g_slist_length(uri_list) == 1)
    {
        vnr_file_load_single_uri (uri_list->data, &file_list, &error);
    }
    else
    {
        vnr_file_load_uri_list (uri_list, &file_list, &error);
    }

    if(error != NULL && file_list != NULL)
    {
        vnr_window_close(window);
        gtk_action_group_set_sensitive(window->actions_collection, FALSE);
        deny_slideshow(window);
        vnr_message_area_show_warning(VNR_MESSAGE_AREA (window->msg_area),
                                      error->message, TRUE);

        vnr_window_set_list(window, file_list, TRUE);
    }
    else if(error != NULL)
    {
        vnr_window_close(window);
        deny_slideshow(window);
        vnr_message_area_show_warning(VNR_MESSAGE_AREA (window->msg_area),
                                      error->message, TRUE);
    }
    else if(file_list == NULL)
    {
        vnr_window_close(window);
        gtk_action_group_set_sensitive(window->actions_collection, FALSE);
        deny_slideshow(window);
        vnr_message_area_show_warning(VNR_MESSAGE_AREA (window->msg_area),
                                      _("The given locations contain no images."), TRUE);
    }
    else
    {
        vnr_window_set_list(window, file_list, TRUE);
        gdk_window_set_cursor(GTK_WIDGET(window)->window, gdk_cursor_new(GDK_WATCH));
        /* This makes the cursor show NOW */
        gtk_main_iteration_do (FALSE);

        vnr_window_close(window);
        vnr_window_open(window, FALSE);
        gdk_window_set_cursor(GTK_WIDGET(window)->window, gdk_cursor_new(GDK_LEFT_PTR));
    }
}

void
vnr_window_close(VnrWindow *win)
{
    gtk_window_set_title (GTK_WINDOW (win), "Viewnior");
    uni_anim_view_set_anim (UNI_ANIM_VIEW (win->view), NULL);
    gtk_action_group_set_sensitive(win->actions_image, FALSE);
    gtk_action_group_set_sensitive(win->actions_static_image, FALSE);
}

void
vnr_window_set_list (VnrWindow *win, GList *list, gboolean free_current)
{
    if (free_current == TRUE && win->file_list != NULL)
        g_list_free (win->file_list);
    if (g_list_length(g_list_first(list)) != 1)
    {
        gtk_action_group_set_sensitive(win->actions_collection, TRUE);
        allow_slideshow(win);
    }
    else
    {
        gtk_action_group_set_sensitive(win->actions_collection, FALSE);
        deny_slideshow(win);
    }
    g_assert(list != NULL);
    win->file_list = list;
}

gboolean
vnr_window_next (VnrWindow *win, gboolean rem_timeout){
    GList *next;
    if(win->mode == VNR_WINDOW_MODE_SLIDESHOW && rem_timeout)
        g_source_remove (win->source_tag);

    next = g_list_next(win->file_list);
    if(next == NULL)
    {
        next = g_list_first(win->file_list);
    }

    win->file_list = next;

    gdk_window_set_cursor(GTK_WIDGET(win)->window, gdk_cursor_new(GDK_WATCH));
    /* This makes the cursor show NOW */
    gtk_main_iteration_do (FALSE);

    vnr_window_open(win, FALSE);
    gdk_window_set_cursor(GTK_WIDGET(win)->window, gdk_cursor_new(GDK_LEFT_PTR));

    if(win->mode == VNR_WINDOW_MODE_SLIDESHOW && rem_timeout)
        win->source_tag = g_timeout_add_seconds (win->timeout,
                                                 (GSourceFunc)next_image_src,
                                                 win);

    return TRUE;
}

gboolean
vnr_window_prev (VnrWindow *win){
    GList *prev;

    if(win->mode == VNR_WINDOW_MODE_SLIDESHOW)
        g_source_remove (win->source_tag);

    prev = g_list_previous(win->file_list);
    if(prev == NULL)
    {
        prev = g_list_last(win->file_list);
    }

    win->file_list = prev;

    gdk_window_set_cursor(GTK_WIDGET(win)->window, gdk_cursor_new(GDK_WATCH));
    /* This makes the cursor show NOW */
    gtk_main_iteration_do (FALSE);

    vnr_window_open(win, FALSE);
    gdk_window_set_cursor(GTK_WIDGET(win)->window, gdk_cursor_new(GDK_LEFT_PTR));

    if(win->mode == VNR_WINDOW_MODE_SLIDESHOW)
        win->source_tag = g_timeout_add_seconds (win->timeout,
                                                 (GSourceFunc)next_image_src,
                                                 win);

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
