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
#include <errno.h>
#include "vnr-window.h"
#include "uni-scroll-win.h"
#include "uni-anim-view.h"
#include "vnr-tools.h"
#include "vnr-message-area.h"
#include "vnr-properties-dialog.h"

/* Timeout to hide the toolbar in fullscreen mode */
#define FULLSCREEN_TIMEOUT 3000

G_DEFINE_TYPE (VnrWindow, vnr_window, GTK_TYPE_WINDOW);

static void vnr_window_unfullscreen (VnrWindow *window);
static void stop_slideshow(VnrWindow *window);
static void start_slideshow(VnrWindow *window);
static void restart_slideshow(VnrWindow *window);
static void allow_slideshow(VnrWindow *window);

static void leave_fs_cb (GtkButton *button, VnrWindow *window);
static void toggle_show_next_cb (GtkToggleButton *togglebutton, VnrWindow *window);
static void spin_value_change_cb (GtkSpinButton *spinbutton, VnrWindow *window);
static void save_image_cb (GtkWidget *widget, VnrWindow *window);
static void zoom_changed_cb (UniImageView *view, VnrWindow *window);
static gboolean fullscreen_timeout_cb (VnrWindow *window);
static gboolean leave_image_area_cb(GtkWidget * widget, GdkEventCrossing * ev, VnrWindow *window);
static gboolean fullscreen_motion_cb(GtkWidget * widget, GdkEventMotion * ev, VnrWindow *window);

const gchar *ui_definition = "<ui>"
  "<menubar name=\"MainMenu\">"
    "<menu action=\"File\">"
      "<menuitem action=\"FileOpen\"/>"
      "<menuitem action=\"FileOpenDir\"/>"
      "<separator/>"
      "<menuitem action=\"FileSave\"/>"
      "<separator/>"
      "<menuitem action=\"FileProperties\"/>"
      "<separator/>"
      "<menuitem action=\"FileClose\"/>"
    "</menu>"
    "<menu action=\"Edit\">"
      "<menuitem action=\"FileDelete\"/>"
      "<separator/>"
      "<menuitem action=\"EditPreferences\"/>"
    "</menu>"
    "<menu action=\"View\">"
      "<menuitem action=\"ViewZoomIn\"/>"
      "<menuitem action=\"ViewZoomOut\"/>"
      "<menuitem action=\"ViewZoomNormal\"/>"
      "<menuitem action=\"ViewZoomFit\"/>"
      "<separator/>"
      "<menuitem name=\"Fullscreen\" action=\"ViewFullscreen\"/>"
      "<menuitem name=\"Slideshow\" action=\"ViewSlideshow\"/>"
      "<separator/>"
      "<menuitem action=\"ViewResizeWindow\"/>"
    "</menu>"
    "<menu action=\"Image\">"
      "<menuitem action=\"ImageFlipVertical\"/>"
      "<menuitem action=\"ImageFlipHorizontal\"/>"
      "<separator/>"
      "<menuitem action=\"ImageRotateCW\"/>"
      "<menuitem action=\"ImageRotateCCW\"/>"
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
  "<popup name=\"PopupMenu\">"
    "<menuitem name=\"GoPrevious\" action=\"GoPrevious\"/>"
    "<menuitem name=\"GoNext\" action=\"GoNext\"/>"
    "<separator/>"
    "<menuitem action=\"ViewZoomIn\"/>"
    "<menuitem action=\"ViewZoomOut\"/>"
    "<menuitem action=\"ViewZoomNormal\"/>"
    "<menuitem action=\"ViewZoomFit\"/>"
    "<separator/>"
    "<menuitem name=\"Fullscreen\" action=\"ViewFullscreen\"/>"
    "<separator/>"
    "<menuitem action=\"FileProperties\"/>"
  "</popup>"
  "<accelerator name=\"ControlEqualAccel\" action=\"ControlEqual\"/>"
  "<accelerator name=\"ControlKPAddAccel\" action=\"ControlKpAdd\"/>"
  "<accelerator name=\"ControlKPSubAccel\" action=\"ControlKpSub\"/>"
  "<accelerator name=\"DeleteAccel\" action=\"Delete\"/>"
"</ui>";

/*************************************************************/
/***** Private actions ***************************************/
/*************************************************************/

static void
vnr_window_hide_cursor(VnrWindow *window)
{
   GdkBitmap *empty;
   GdkColor black = { 0, 0, 0, 0 };
   static char bits[] = { 0x00 };

   empty = gdk_bitmap_create_from_data (NULL, bits, 1, 1);

    gdk_window_set_cursor (GTK_WIDGET(window)->window,
                           gdk_cursor_new_from_pixmap (empty, empty, &black,
                                                       &black, 0, 0));
    window->cursor_is_hidden = TRUE;
    gdk_flush();
}

static void
vnr_window_show_cursor(VnrWindow *window)
{
    gdk_window_set_cursor (GTK_WIDGET(window)->window, gdk_cursor_new(GDK_LEFT_PTR));
    window->cursor_is_hidden = FALSE;
    gdk_flush();
}

static void
update_fs_filename_label(VnrWindow *window)
{
    if(window->mode == VNR_WINDOW_MODE_NORMAL)
        return;

    gtk_label_set_text(GTK_LABEL(window->fs_filename_label),
                       VNR_FILE(window->file_list->data)->display_name);
}

static gboolean
next_image_src(VnrWindow *window)
{
    if(g_list_length(g_list_first(window->file_list)) <= 1)
        return FALSE;
    else
        vnr_window_next(window, FALSE);

    window->ss_source_tag = g_timeout_add_seconds (window->ss_timeout,
                                                   (GSourceFunc)next_image_src,
                                                   window);

    return FALSE;
}

static void
fullscreen_unset_timeout(VnrWindow *window)
{
    if(window->fs_source != NULL)
    {
        g_source_unref (window->fs_source);
        g_source_destroy (window->fs_source);
        window->fs_source = NULL;
    }
}

static void
fullscreen_set_timeout(VnrWindow *window)
{
    fullscreen_unset_timeout(window);

    window->fs_source = g_timeout_source_new (FULLSCREEN_TIMEOUT);
    g_source_set_callback (window->fs_source,
                           (GSourceFunc)fullscreen_timeout_cb,
                           window, NULL);

    g_source_attach (window->fs_source, NULL);
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

    /* Tool item, that contains the hbox */
    item = gtk_tool_item_new();
    gtk_tool_item_set_expand(item, TRUE);

    box = gtk_hbox_new(FALSE, 0);
    gtk_container_add (GTK_CONTAINER (item), box);

    widget = gtk_button_new_from_stock(GTK_STOCK_LEAVE_FULLSCREEN);
    g_signal_connect(widget, "clicked", G_CALLBACK(leave_fs_cb), window);
    gtk_box_pack_end (GTK_BOX(box), widget, FALSE, FALSE, 0);

    /* Create label for the current image's filename */
    widget = gtk_label_new(NULL);
    gtk_label_set_ellipsize (GTK_LABEL(widget), PANGO_ELLIPSIZE_END);
    gtk_label_set_selectable (GTK_LABEL(widget), TRUE);
    window->fs_filename_label = widget;
    gtk_box_pack_end (GTK_BOX(box), widget, TRUE, TRUE, 10);


    widget = gtk_vseparator_new();
    gtk_box_pack_start (GTK_BOX(box), widget, FALSE, FALSE, 0);

    widget = gtk_check_button_new_with_label(_("Show next image after: "));
    g_signal_connect (widget, "toggled", G_CALLBACK(toggle_show_next_cb),
                      window);
    gtk_box_pack_start (GTK_BOX(box), widget, FALSE, FALSE, 0);
    window->toggle_btn = widget;

    /* Create spin button to adjust slideshow's timeout */
    //spinner_adj = (GtkAdjustment *) gtk_adjustment_new (5, 1.0, 30.0, 1.0, 1.0, 0);
    spinner_adj = (GtkAdjustment *) gtk_adjustment_new (window->prefs->slideshow_timeout, 1.0, 30.0, 1.0, 1.0, 0);
    widget = gtk_spin_button_new (spinner_adj, 1.0, 0);
    gtk_spin_button_set_snap_to_ticks (GTK_SPIN_BUTTON(widget), TRUE);
    gtk_spin_button_set_update_policy (GTK_SPIN_BUTTON(widget),
                                       GTK_UPDATE_ALWAYS);
    g_signal_connect (widget, "value-changed",
                      G_CALLBACK(spin_value_change_cb), window);
    gtk_box_pack_start (GTK_BOX(box), widget, FALSE, FALSE, 0);
    window->ss_timeout_widget = widget;

    window->fs_seconds_label = gtk_label_new(ngettext(" second", " seconds", 5));
    gtk_box_pack_start (GTK_BOX(box), window->fs_seconds_label, FALSE, FALSE, 0);

    window->fs_controls = GTK_WIDGET(item);

    gtk_widget_show_all (window->fs_controls);
    return window->fs_controls;
}

static gboolean
scrollbars_visible (VnrWindow *window)
{
    if (!GTK_WIDGET_VISIBLE (GTK_WIDGET (UNI_SCROLL_WIN(window->scroll_view)->hscroll)) &&
        !GTK_WIDGET_VISIBLE (GTK_WIDGET (UNI_SCROLL_WIN(window->scroll_view)->vscroll)))
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

    if (window->prefs->fit_on_fullscreen)
        uni_image_view_set_zoom_mode (UNI_IMAGE_VIEW(window->view),
                                      VNR_PREFS_ZOOM_FIT);

    update_fs_filename_label(window);
    gtk_widget_hide_all (window->toolbar);

    stop_slideshow(window);

    /* Reset timeouts for the toolbar autohide when the mouse
     * moves over the UniImageviewer.
     * "after" because it must be called after the uniImageView's
     * callback (when the image is dragged).*/
    g_signal_connect_after (window->view,
                            "motion-notify-event",
                            G_CALLBACK (fullscreen_motion_cb),
                            window);

    /* Never hide the toolbar, while the mouse is over it */
    g_signal_connect (window->toolbar,
                      "enter-notify-event",
                      G_CALLBACK (leave_image_area_cb),
                      window);

    g_signal_connect (window->msg_area,
                      "enter-notify-event",
                      G_CALLBACK (leave_image_area_cb),
                      window);

    fullscreen_set_timeout(window);
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

    if (window->prefs->fit_on_fullscreen)
        uni_image_view_set_zoom_mode (UNI_IMAGE_VIEW(window->view),
                                      window->prefs->zoom);

    /* First show everything (the toolbar might be hided)
     * and then hide only the fullscreen controls */
    gtk_widget_show_all(window->toolbar);
    gtk_widget_hide (get_fs_controls(window));

    g_signal_handlers_disconnect_by_func(window->view,
                                         G_CALLBACK(fullscreen_motion_cb),
                                         window);

    g_signal_handlers_disconnect_by_func(window->toolbar,
                                         G_CALLBACK(leave_image_area_cb),
                                         window);

    g_signal_handlers_disconnect_by_func(window->msg_area,
                                         G_CALLBACK(leave_image_area_cb),
                                         window);

    fullscreen_unset_timeout(window);
    vnr_window_show_cursor(window);
}

static void
stop_slideshow(VnrWindow *window)
{
    if(!window->slideshow)
        return;

    if(window->mode != VNR_WINDOW_MODE_SLIDESHOW)
        return;

    GtkAction *action;

    action = gtk_action_group_get_action (window->actions_collection,
                                          "ViewSlideshow");

    window->slideshow = FALSE;
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(window->toggle_btn), FALSE);
    gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), FALSE);
    window->slideshow = TRUE;

    window->mode = VNR_WINDOW_MODE_FULLSCREEN;

    g_source_remove (window->ss_source_tag);
}

static void
start_slideshow(VnrWindow *window)
{
    if(!window->slideshow)
        return;

    if(window->mode == VNR_WINDOW_MODE_SLIDESHOW)
        return;

    window->mode = VNR_WINDOW_MODE_SLIDESHOW;

    window->ss_source_tag = g_timeout_add_seconds (window->ss_timeout,
                                                   (GSourceFunc)next_image_src,
                                                   window);

    GtkAction *action;

    action = gtk_action_group_get_action (window->actions_collection,
                                          "ViewSlideshow");

    window->slideshow = FALSE;
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(window->toggle_btn), TRUE);
    gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (action), TRUE);
    window->slideshow = TRUE;
}

static void
restart_slideshow(VnrWindow *window)
{
    if(!window->slideshow)
        return;

    if(window->mode != VNR_WINDOW_MODE_SLIDESHOW)
        return;

    g_source_remove (window->ss_source_tag);
    window->ss_source_tag = g_timeout_add_seconds (window->ss_timeout,
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

    if(!window->cursor_is_hidden)
        gdk_window_set_cursor(GTK_WIDGET(window)->window,
                              gdk_cursor_new(GDK_WATCH));
    /* This makes the cursor show NOW */
    gdk_flush();

    result = gdk_pixbuf_rotate_simple(UNI_IMAGE_VIEW(window->view)->pixbuf,
                                      angle);

    if(result == NULL)
    {
        vnr_message_area_show(VNR_MESSAGE_AREA(window->msg_area),
                              TRUE, _("Not enough virtual memory."),
                              FALSE);
        return;
    }

    uni_anim_view_set_static(UNI_ANIM_VIEW(window->view), result);

    if(!window->cursor_is_hidden)
        gdk_window_set_cursor(GTK_WIDGET(window)->window,
                              gdk_cursor_new(GDK_LEFT_PTR));
    g_object_unref(result);

    window->current_image_width = gdk_pixbuf_get_width (result);
    window->current_image_height = gdk_pixbuf_get_height (result);

    if(GTK_WIDGET_VISIBLE(window->props_dlg))
        vnr_properties_dialog_update_image(VNR_PROPERTIES_DIALOG(window->props_dlg));

    /* Extra conditions. Rotating 180 degrees is also flipping horizontal and vertical */
    if((window->modifications & (4))^((angle==GDK_PIXBUF_ROTATE_CLOCKWISE)<<2))
        window->modifications ^= 3;

    window->modifications ^= 4;
    gtk_action_group_set_sensitive(window->action_save, window->modifications);

    if(window->modifications == 0 && window->prefs->behavior_modify != VNR_PREFS_MODIFY_IGNORE)
    {
        vnr_message_area_hide(VNR_MESSAGE_AREA(window->msg_area));
        return;
    }

    if(window->writable_format_name == NULL)
        vnr_message_area_show(VNR_MESSAGE_AREA(window->msg_area),
                              TRUE,
                              _("Image modifications cannot be saved.\nWriting in this format is not supported."),
                              FALSE);
    else if(window->prefs->behavior_modify == VNR_PREFS_MODIFY_SAVE)
        save_image_cb(NULL, window);
    else if(window->prefs->behavior_modify == VNR_PREFS_MODIFY_ASK)
        vnr_message_area_show_with_button(VNR_MESSAGE_AREA(window->msg_area),
                                          FALSE,
                                          _("Save modifications?\nThis will overwrite the image and may reduce it's quality!"),
                                          FALSE, GTK_STOCK_SAVE,
                                          G_CALLBACK(save_image_cb));
}

static void
flip_pixbuf(VnrWindow *window, gboolean horizontal)
{
    GdkPixbuf *result;

    if(!window->cursor_is_hidden)
        gdk_window_set_cursor (GTK_WIDGET(window)->window,
                               gdk_cursor_new(GDK_WATCH));
    /* This makes the cursor show NOW */
    gdk_flush();

    result = gdk_pixbuf_flip(UNI_IMAGE_VIEW(window->view)->pixbuf,
                             horizontal);

    if(result == NULL)
    {
        vnr_message_area_show(VNR_MESSAGE_AREA(window->msg_area),
                              TRUE, _("Not enough virtual memory."),
                              FALSE);
        return;
    }

    uni_anim_view_set_static(UNI_ANIM_VIEW(window->view), result);

    if(GTK_WIDGET_VISIBLE(window->props_dlg))
        vnr_properties_dialog_update_image(VNR_PROPERTIES_DIALOG(window->props_dlg));

    if(!window->cursor_is_hidden)
        gdk_window_set_cursor (GTK_WIDGET(window)->window,
                               gdk_cursor_new(GDK_LEFT_PTR));
    g_object_unref(result);

    /* Extra conditions. Rotating 180 degrees is also flipping horizontal and vertical */
    window->modifications ^= (window->modifications&4)?1+horizontal:2-horizontal;

    gtk_action_group_set_sensitive(window->action_save, window->modifications);

    if(window->modifications == 0)
    {
        vnr_message_area_hide(VNR_MESSAGE_AREA(window->msg_area));
        return;
    }

    if(window->writable_format_name == NULL)
        vnr_message_area_show(VNR_MESSAGE_AREA(window->msg_area),
                              TRUE,
                              _("Image modifications cannot be saved.\nWriting in this format is not supported."),
                              FALSE);
    else if(window->prefs->behavior_modify == VNR_PREFS_MODIFY_SAVE)
        save_image_cb(NULL, window);
    else if(window->prefs->behavior_modify == VNR_PREFS_MODIFY_ASK)
        vnr_message_area_show_with_button(VNR_MESSAGE_AREA(window->msg_area),
                                          FALSE,
                                          _("Save modifications?\nThis will overwrite the image and may reduce it's quality!"),
                                          FALSE, GTK_STOCK_SAVE,
                                          G_CALLBACK(save_image_cb));
}

/*************************************************************/
/***** Private signal handlers *******************************/
/*************************************************************/

static gboolean
leave_image_area_cb(GtkWidget * widget, GdkEventCrossing * ev, VnrWindow *window)
{
    fullscreen_unset_timeout (window);
    return FALSE;
}

static gboolean
fullscreen_motion_cb(GtkWidget * widget, GdkEventMotion * ev, VnrWindow *window)
{
    if(window->disable_autohide)
        return FALSE;

    /* Show the toolbar only when the moves moves to the top
     * of the UniImageView */
    if (ev->y < 20 && !GTK_WIDGET_VISIBLE (window->toolbar))
        gtk_widget_show_all (GTK_WIDGET (window->toolbar));

    if(window->cursor_is_hidden)
        vnr_window_show_cursor(window);

    fullscreen_set_timeout(window);
    return FALSE;
}

/* Hides the toolbar */
static gboolean
fullscreen_timeout_cb (VnrWindow *window)
{
    fullscreen_unset_timeout (window);

    if(window->disable_autohide)
        return FALSE;

    gtk_widget_hide_all(window->toolbar);
    vnr_window_hide_cursor(window);
    return FALSE;
}

static void
spin_value_change_cb (GtkSpinButton *spinbutton, VnrWindow *window)
{
    int new_value = gtk_spin_button_get_value_as_int (spinbutton);

    if(new_value != window->prefs->slideshow_timeout)
        vnr_prefs_set_slideshow_timeout(window->prefs, new_value);

    gtk_label_set_text (GTK_LABEL(window->fs_seconds_label),
                        ngettext(" second", " seconds", new_value));
    window->ss_timeout = new_value;
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

static void
save_image_cb (GtkWidget *widget, VnrWindow *window)
{
    GError *error = NULL;
    if(!window->cursor_is_hidden)
        gdk_window_set_cursor(GTK_WIDGET(window)->window, gdk_cursor_new(GDK_WATCH));
    /* This makes the cursor show NOW */
    gdk_flush();

    if(window->prefs->behavior_modify == VNR_PREFS_MODIFY_ASK)
        vnr_message_area_hide(VNR_MESSAGE_AREA(window->msg_area));

    if(g_strcmp0(window->writable_format_name, "jpeg" ) == 0)
    {
        gchar *quality;
        quality = g_strdup_printf ("%i", window->prefs->jpeg_quality);

        gdk_pixbuf_save (uni_image_view_get_pixbuf(UNI_IMAGE_VIEW(window->view)),
                         VNR_FILE(window->file_list->data)->path, "jpeg",
                         &error, "quality", quality, NULL);
        g_free(quality);
    }
    else if(g_strcmp0(window->writable_format_name, "png" ) == 0)
    {
        gchar *compression;
        compression = g_strdup_printf ("%i", window->prefs->png_compression);

        gdk_pixbuf_save (uni_image_view_get_pixbuf(UNI_IMAGE_VIEW(window->view)),
                         VNR_FILE(window->file_list->data)->path, "png",
                         &error, "compression", compression, NULL);
        g_free(compression);
    }
    else
    {
        gdk_pixbuf_save (uni_image_view_get_pixbuf(UNI_IMAGE_VIEW(window->view)),
                         VNR_FILE(window->file_list->data)->path,
                         window->writable_format_name, &error, NULL);
    }

    if(!window->cursor_is_hidden)
        gdk_window_set_cursor(GTK_WIDGET(window)->window, gdk_cursor_new(GDK_LEFT_PTR));

    if(error != NULL)
    {
        vnr_message_area_show(VNR_MESSAGE_AREA(window->msg_area), TRUE,
                                               error->message, FALSE);
        return;
    }
    window->modifications = 0;

    gtk_action_group_set_sensitive(window->action_save, FALSE);

    if(window->prefs->behavior_modify != VNR_PREFS_MODIFY_ASK)
        zoom_changed_cb(UNI_IMAGE_VIEW(window->view), window);

    if(GTK_WIDGET_VISIBLE(window->props_dlg))
        vnr_properties_dialog_update(VNR_PROPERTIES_DIALOG(window->props_dlg));
}

static void
leave_fs_cb (GtkButton *button, VnrWindow *window)
{
    vnr_window_unfullscreen (window);
}

static void
window_realize_cb(GtkWidget *widget, gpointer user_data)
{
    g_signal_handlers_disconnect_by_func(widget, window_realize_cb, user_data);

    if(!vnr_message_area_is_critical(VNR_MESSAGE_AREA(VNR_WINDOW(widget)->msg_area)))
    {
        GdkScreen *screen;
        GdkRectangle monitor;
        screen = gtk_window_get_screen (GTK_WINDOW (widget));
        gdk_screen_get_monitor_geometry (screen,
                                         gdk_screen_get_monitor_at_window (screen,
                                            widget->window),
                                         &monitor);

        VNR_WINDOW(widget)->max_width = monitor.width * 0.9 - 100;
        VNR_WINDOW(widget)->max_height = monitor.height * 0.9 - 100;

        vnr_window_open(VNR_WINDOW(widget), TRUE);
    }
}

static void
zoom_changed_cb (UniImageView *view, VnrWindow *window)
{
    gint position, total;
    char *buf = NULL;

    /* Change the info, only if there is an image
     * (vnr_window_close isn't called on the current image) */
    if(gtk_action_group_get_sensitive (window->actions_image))
    {
        get_position_of_element_in_list(window->file_list, &position, &total);
        buf = g_strdup_printf ("%s%s - %i/%i - %i%%", (window->modifications)?"*":"",
                               VNR_FILE(window->file_list->data)->display_name,
                               position, total,
                               (int)(view->zoom*100.));

        gtk_window_set_title (GTK_WINDOW(window), buf);
        g_free(buf);
    }
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
vnr_window_cmd_preferences(GtkAction *action, gpointer user_data)
{
    vnr_prefs_show_dialog(VNR_WINDOW(user_data)->prefs);
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
    uni_image_view_zoom_in(UNI_IMAGE_VIEW(VNR_WINDOW(user_data)->view));
}

static void
vnr_window_cmd_zoom_out (GtkAction *action, gpointer user_data)
{
    uni_image_view_zoom_out(UNI_IMAGE_VIEW(VNR_WINDOW(user_data)->view));
}

static void
vnr_window_cmd_normal_size (GtkAction *action, gpointer user_data)
{
    uni_image_view_set_zoom(UNI_IMAGE_VIEW(VNR_WINDOW(user_data)->view), 1);
}

static void
vnr_window_cmd_fit (GtkAction *action, gpointer user_data)
{
    uni_image_view_set_fitting(UNI_IMAGE_VIEW(VNR_WINDOW(user_data)->view), UNI_FITTING_FULL);
}

static void
vnr_window_cmd_next (GtkAction *action, gpointer user_data)
{
    vnr_window_next(VNR_WINDOW(user_data), TRUE);
}

static void
vnr_window_cmd_first (GtkAction *action, gpointer user_data)
{
    vnr_window_first(VNR_WINDOW(user_data));
}

static void
vnr_window_cmd_last (GtkAction *action, gpointer user_data)
{
    vnr_window_last(VNR_WINDOW(user_data));
}

static void
vnr_window_cmd_prev (GtkAction *action, gpointer user_data)
{
    vnr_window_prev(VNR_WINDOW(user_data));
}

static void
vnr_window_cmd_resize (GtkAction *action, VnrWindow *window)
{
    gint img_h, img_w;          /* Width and Height of the pixbuf */

    img_w = window->current_image_width;
    img_h = window->current_image_height;

    vnr_tools_fit_to_size (&img_w, &img_h, window->max_width, window->max_height);

    gtk_window_resize (GTK_WINDOW (window), img_w, img_h + window->menus->allocation.height);
}

static void
vnr_window_cmd_properties (GtkAction *action, VnrWindow *window)
{
    vnr_properties_dialog_show(VNR_PROPERTIES_DIALOG (window->props_dlg));
}

static void
vnr_window_cmd_open(GtkAction *action, VnrWindow *window)
{
    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new (_("Open Image"),
                          GTK_WINDOW(window),
                          GTK_FILE_CHOOSER_ACTION_OPEN,
                          GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                          GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                          NULL);

    gtk_window_set_modal (GTK_WINDOW(dialog), FALSE);
    gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialog), TRUE);

    gchar *dirname;
    if(window->file_list != NULL)
    {
        dirname = g_path_get_dirname (VNR_FILE(window->file_list->data)->path);
        gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER(dialog), dirname);
        g_free(dirname);
    }

    g_signal_connect (dialog, "response",
                      G_CALLBACK (file_open_dialog_response_cb),
                      window);

    gtk_widget_show_all (GTK_WIDGET(dialog));
}

static void
vnr_window_cmd_open_dir(GtkAction *action, VnrWindow *window)
{
    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new (_("Open Folder"),
                          GTK_WINDOW(window),
                          GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                          GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                          GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                          NULL);

    gtk_window_set_modal (GTK_WINDOW(dialog), FALSE);
    gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(dialog), TRUE);

    gchar *dirname;
    if(window->file_list != NULL)
    {
        dirname = g_path_get_dirname (VNR_FILE(window->file_list->data)->path);
        gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER(dialog), dirname);
        g_free(dirname);
    }

    g_signal_connect (dialog, "response",
                      G_CALLBACK (file_open_dialog_response_cb),
                      window);

    gtk_widget_show_all (GTK_WIDGET(dialog));
}

static void
vnr_window_cmd_about (GtkAction *action, VnrWindow *window)
{
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

static void
vnr_window_cmd_fullscreen (GtkAction *action, VnrWindow *window)
{
    gboolean fullscreen;

    fullscreen = gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action));

    if (fullscreen)
        vnr_window_fullscreen (window);
    else
        vnr_window_unfullscreen (window);
}

static void
vnr_window_cmd_slideshow (GtkAction *action, VnrWindow *window)
{
    if(!window->slideshow)
        return;

    g_assert(window != NULL && VNR_IS_WINDOW(window));

    gboolean slideshow;

    slideshow = gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action));

    if (slideshow && window->mode != VNR_WINDOW_MODE_SLIDESHOW)
    {
        /* ! Uncomment to force Fullscreen along with Slideshow */
        if(window->mode == VNR_WINDOW_MODE_NORMAL)
        {
            vnr_window_fullscreen (window);
        }
        start_slideshow(window);
    }
    else if(window->mode == VNR_WINDOW_MODE_SLIDESHOW)
    {
        /* ! Uncomment to force Fullscreen along with Slideshow */
        vnr_window_unfullscreen (window);
        stop_slideshow(window);
    }
}

static void
vnr_window_cmd_delete(GtkAction *action, VnrWindow *window)
{
    GtkWidget *dlg = NULL;
    const gchar *file_path;
    gchar *markup, *prompt, *warning;
    gboolean restart_slideshow = FALSE;
    gboolean restart_autohide_timeout = FALSE;
    gboolean cursor_was_hidden = FALSE;

    /* Used to get rid of the "may be used uninitialised" warning */
    markup = prompt = warning = NULL;

    if(window->mode == VNR_WINDOW_MODE_SLIDESHOW)
    {
       stop_slideshow(window);
       restart_slideshow = TRUE;
    }

    if(window->cursor_is_hidden)
    {
        cursor_was_hidden = TRUE;
        vnr_window_show_cursor(window);
    }
    window->disable_autohide = TRUE;

    if(window->fs_source != NULL)
        restart_autohide_timeout = TRUE;

    g_return_if_fail (window->file_list != NULL);

    file_path = VNR_FILE(window->file_list->data)->path;

    if(window->prefs->confirm_delete)
    {
        warning = _("If you delete an item, it will be permanently lost.");

        /* I18N: The '%s' is replaced with the name of the file to be deleted. */
        prompt = g_strdup_printf (_("Are you sure you want to\n"
                                    "permanently delete \"%s\"?"),
                                  VNR_FILE(window->file_list->data)->display_name);
        markup = g_strdup_printf ("<span weight=\"bold\" size=\"larger\">%s</span>\n\n%s",
                                  prompt, warning);


        dlg = gtk_message_dialog_new(GTK_WINDOW(window),
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
    }

    if(!window->prefs->confirm_delete || gtk_dialog_run(GTK_DIALOG(dlg)) == GTK_RESPONSE_YES )
    {
        if( g_unlink(file_path) != 0 )
        {
            /* I18N: The '%s' is replaced with error message. */
            vnr_message_area_show(VNR_MESSAGE_AREA (window->msg_area), TRUE,
                                  g_strdup_printf (_("Error deleting image: %s"),
                                   g_strerror(errno)), FALSE);
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
                vnr_message_area_show(VNR_MESSAGE_AREA (window->msg_area), TRUE,
                                      _("The given locations contain no images."),
                                      TRUE);
                restart_slideshow = FALSE;


                if(GTK_WIDGET_VISIBLE(window->props_dlg))
                    vnr_properties_dialog_clear(VNR_PROPERTIES_DIALOG(window->props_dlg));
            }
            else
            {
                vnr_window_set_list(window, next, FALSE);
                if(window->prefs->confirm_delete && !window->cursor_is_hidden)
                    gdk_window_set_cursor(GTK_WIDGET(dlg)->window,
                                          gdk_cursor_new(GDK_WATCH));

                gdk_flush();

                vnr_window_close(window);
                vnr_window_open(window, FALSE);
                if(window->prefs->confirm_delete && !window->cursor_is_hidden)
                    gdk_window_set_cursor(GTK_WIDGET(dlg)->window,
                                          gdk_cursor_new(GDK_LEFT_PTR));
            }
        }
    }

    window->disable_autohide = FALSE;

    if(restart_slideshow)
       start_slideshow(window);
    if(cursor_was_hidden)
        vnr_window_hide_cursor(window);
    if(restart_autohide_timeout)
        fullscreen_set_timeout(window);

    if(window->prefs->confirm_delete)
    {
        g_free(prompt);
        g_free(markup);
        gtk_widget_destroy( dlg );
    }
}

static const GtkActionEntry action_entries_window[] = {
    { "File",  NULL, N_("_File") },
    { "Edit",  NULL, N_("_Edit") },
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
      G_CALLBACK (vnr_window_cmd_about) },
    { "EditPreferences", GTK_STOCK_PREFERENCES, N_("_Preferences..."), NULL,
      N_("User preferences for Viewnior"),
      G_CALLBACK (vnr_window_cmd_preferences) }
};

static const GtkActionEntry action_entry_save[] = {
    { "FileSave", GTK_STOCK_SAVE, N_("_Save"), "<control>S",
      N_("Save changes"),
      G_CALLBACK (save_image_cb) },
};

static const GtkActionEntry action_entries_image[] = {
    { "FileDelete", GTK_STOCK_DELETE, N_("_Delete"), NULL,
      N_("Delete the current file"),
      G_CALLBACK (vnr_window_cmd_delete) },
    { "FileProperties", GTK_STOCK_PROPERTIES, N_("_Properties..."), "<Alt>Return",
      N_("Show information about the current file"),
      G_CALLBACK (vnr_window_cmd_properties) },
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

static const GtkToggleActionEntry toggle_entries_collection[] = {
    { "ViewSlideshow", GTK_STOCK_NETWORK, N_("Sli_deshow"), "F5",
      N_("Show in slideshow mode"),
      G_CALLBACK (vnr_window_cmd_slideshow) },
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
/* Modified version of eog's eog_window_key_press */
static gint
vnr_window_key_press (GtkWidget *widget, GdkEventKey *event)
{
    gint result = FALSE;
    VnrWindow *window = VNR_WINDOW(widget);

    switch(event->keyval){
        case GDK_Left:
            if (event->state & GDK_MOD1_MASK)
            {
                vnr_window_cmd_prev (NULL, window);
                result = TRUE;
                break;
            } /* else fall-trough is intended */
        case GDK_Up:
            if (scrollbars_visible (window))
            {
                /* break to let scrollview handle the key */
                break;
            }
            if (GTK_CONTAINER(window->toolbar)->focus_child != NULL ||
                GTK_CONTAINER(window->msg_area)->focus_child != NULL)
                break;

            vnr_window_cmd_prev (NULL, window);
            result = TRUE;
            break;
        case GDK_Right:
            if (event->state & GDK_MOD1_MASK)
            {
                vnr_window_cmd_next (NULL, window);
                result = TRUE;
                break;
            } /* else fall-trough is intended */
        case GDK_Down:
            if (scrollbars_visible (window))
            {
                /* break to let scrollview handle the key */
                break;
            }
            if (GTK_CONTAINER(window->toolbar)->focus_child != NULL ||
                GTK_CONTAINER(window->msg_area)->focus_child != NULL)
                break;

            vnr_window_cmd_next (NULL, window);
            result = TRUE;
            break;
        case GDK_Escape:
            if(window->mode != VNR_WINDOW_MODE_NORMAL)
                vnr_window_unfullscreen(window);
            break;
        case GDK_space:
            if (GTK_CONTAINER(window->toolbar)->focus_child != NULL ||
                GTK_CONTAINER(window->msg_area)->focus_child != NULL)
                break;
            vnr_window_next(window, TRUE);
            result = TRUE;
            break;
        case GDK_BackSpace:
            vnr_window_prev(window);
            result = TRUE;
            break;
    }

    if (result == FALSE && GTK_WIDGET_CLASS (vnr_window_parent_class)->key_press_event)
        result = (* GTK_WIDGET_CLASS (vnr_window_parent_class)->key_press_event) (widget, event);

    return result;
}

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

        if(uri_list == NULL)
        {
            vnr_window_close(VNR_WINDOW (widget));
            gtk_action_group_set_sensitive(VNR_WINDOW (widget)->actions_collection, FALSE);
            deny_slideshow(VNR_WINDOW (widget));
            vnr_message_area_show(VNR_MESSAGE_AREA (VNR_WINDOW (widget)->msg_area), TRUE,
                                  _("The given locations contain no images."),
                                  TRUE);
            return;
        }

        vnr_window_open_from_list(VNR_WINDOW (widget), uri_list);
    }
}

static void
vnr_window_class_init (VnrWindowClass * klass)
{
    GtkWidgetClass *widget_class = (GtkWidgetClass *) klass;
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

    window->writable_format_name = NULL;
    window->file_list = NULL;
    window->fs_controls = NULL;
    window->fs_source = NULL;
    window->ss_timeout = 5;
    window->slideshow = TRUE;
    window->cursor_is_hidden = FALSE;
    window->disable_autohide = FALSE;

    window->prefs = (VnrPrefs*)vnr_prefs_new (GTK_WIDGET(window));

    window->mode = VNR_WINDOW_MODE_NORMAL;

    gtk_window_set_title ((GtkWindow *) window, "Viewnior");
    gtk_window_set_default_icon_name ("viewnior");

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

    window->action_save = gtk_action_group_new("MenuActionSave");


    gtk_action_group_set_translation_domain (window->action_save,
                                             GETTEXT_PACKAGE);

    gtk_action_group_add_actions (window->action_save,
                                  action_entry_save,
                                  G_N_ELEMENTS (action_entry_save),
                                  window);

    gtk_ui_manager_insert_action_group (window->ui_mngr,
                                        window->action_save, 0);

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
    gtk_action_group_add_toggle_actions (window->actions_collection,
                                         toggle_entries_collection,
                                         G_N_ELEMENTS (toggle_entries_collection),
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
    gtk_action_group_set_sensitive(window->action_save, FALSE);

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

    window->popup_menu = gtk_ui_manager_get_widget (window->ui_mngr, "/PopupMenu");
    g_assert(GTK_IS_WIDGET(window->popup_menu));

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

    /* Care for Properties dialog */
    window->props_dlg = vnr_properties_dialog_new(window,
                             gtk_action_group_get_action (window->actions_collection,
                                                          "GoNext"),
                             gtk_action_group_get_action (window->actions_collection,
                                                          "GoPrevious"));

    vnr_window_apply_preferences(window);

    vnr_window_set_drag(window);

    g_signal_connect (G_OBJECT (window), "destroy",
                      G_CALLBACK (gtk_main_quit), NULL);

    g_signal_connect (G_OBJECT (window), "realize",
                      G_CALLBACK (window_realize_cb), NULL);

    g_signal_connect (G_OBJECT (window->view), "zoom_changed",
                      G_CALLBACK (zoom_changed_cb), window);

    gtk_window_add_accel_group (GTK_WINDOW (window),
                gtk_ui_manager_get_accel_group (window->ui_mngr));
}

/*************************************************************/
/***** Actions ***********************************************/
/*************************************************************/
gboolean
vnr_window_open (VnrWindow * window, gboolean fit_to_screen)
{
    VnrFile *file;
    GdkPixbufAnimation *pixbuf;
    GdkPixbufFormat *format;
    GError *error = NULL;

    if(window->file_list == NULL)
        return FALSE;

    file = VNR_FILE(window->file_list->data);

    update_fs_filename_label(window);

    pixbuf = gdk_pixbuf_animation_new_from_file (file->path, &error);

    if (error != NULL)
    {
        vnr_message_area_show(VNR_MESSAGE_AREA (window->msg_area),
                              TRUE, error->message, TRUE);

        if(GTK_WIDGET_VISIBLE(window->props_dlg))
            vnr_properties_dialog_clear(VNR_PROPERTIES_DIALOG(window->props_dlg));
        return FALSE;
    }

    if(vnr_message_area_is_visible(VNR_MESSAGE_AREA(window->msg_area)))
    {
        vnr_message_area_hide(VNR_MESSAGE_AREA(window->msg_area));
    }

    gtk_action_group_set_sensitive(window->actions_image, TRUE);

    format = gdk_pixbuf_get_file_info (file->path, NULL, NULL);

    g_free(window->writable_format_name);
    if(gdk_pixbuf_format_is_writable (format))
        window->writable_format_name = gdk_pixbuf_format_get_name (format);
    else
        window->writable_format_name = NULL;

    window->current_image_width = gdk_pixbuf_animation_get_width (pixbuf);
    window->current_image_height = gdk_pixbuf_animation_get_height (pixbuf);
    window->modifications = 0;

    if(fit_to_screen)
    {
        gint img_h, img_w;          /* Width and Height of the pixbuf */

        img_w = window->current_image_width;
        img_h = window->current_image_height;

        vnr_tools_fit_to_size (&img_w, &img_h, window->max_width, window->max_height);

        gtk_window_resize (GTK_WINDOW (window), img_w, img_h + window->menus->allocation.height);
    }

    /* Return TRUE if the image is static */
    if ( uni_anim_view_set_anim (UNI_ANIM_VIEW (window->view), pixbuf) )
        gtk_action_group_set_sensitive(window->actions_static_image, TRUE);
    else
        gtk_action_group_set_sensitive(window->actions_static_image, FALSE);

    if(window->mode != VNR_WINDOW_MODE_NORMAL && window->prefs->fit_on_fullscreen)
        uni_image_view_set_zoom_mode (UNI_IMAGE_VIEW(window->view),
                                      VNR_PREFS_ZOOM_FIT);
    else
        uni_image_view_set_zoom_mode (UNI_IMAGE_VIEW(window->view),
                                      window->prefs->zoom);

    if(GTK_WIDGET_VISIBLE(window->props_dlg))
        vnr_properties_dialog_update(VNR_PROPERTIES_DIALOG(window->props_dlg));

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
        vnr_file_load_single_uri (uri_list->data, &file_list, window->prefs->show_hidden, &error);
    }
    else
    {
        vnr_file_load_uri_list (uri_list, &file_list, window->prefs->show_hidden, &error);
    }

    if(error != NULL && file_list != NULL)
    {
        vnr_window_close(window);
        gtk_action_group_set_sensitive(window->actions_collection, FALSE);
        deny_slideshow(window);
        vnr_message_area_show(VNR_MESSAGE_AREA (window->msg_area),
                              TRUE, error->message, TRUE);

        vnr_window_set_list(window, file_list, TRUE);
    }
    else if(error != NULL)
    {
        vnr_window_close(window);
        deny_slideshow(window);
        vnr_message_area_show(VNR_MESSAGE_AREA (window->msg_area),
                              TRUE, error->message, TRUE);
    }
    else if(file_list == NULL)
    {
        vnr_window_close(window);
        gtk_action_group_set_sensitive(window->actions_collection, FALSE);
        deny_slideshow(window);
        vnr_message_area_show(VNR_MESSAGE_AREA (window->msg_area), TRUE,
                              _("The given locations contain no images."),
                              TRUE);
    }
    else
    {
        vnr_window_set_list(window, file_list, TRUE);
        if(!window->cursor_is_hidden)
            gdk_window_set_cursor(GTK_WIDGET(window)->window,
                                  gdk_cursor_new(GDK_WATCH));
        /* This makes the cursor show NOW */
        gdk_flush();

        vnr_window_close(window);
        vnr_window_open(window, FALSE);
        if(!window->cursor_is_hidden)
            gdk_window_set_cursor(GTK_WIDGET(window)->window,
                                  gdk_cursor_new(GDK_LEFT_PTR));
    }
}

void
vnr_window_close(VnrWindow *window)
{
    gtk_window_set_title (GTK_WINDOW (window), "Viewnior");
    uni_anim_view_set_anim (UNI_ANIM_VIEW (window->view), NULL);
    gtk_action_group_set_sensitive(window->actions_image, FALSE);
    gtk_action_group_set_sensitive(window->actions_static_image, FALSE);
}

void
vnr_window_set_list (VnrWindow *window, GList *list, gboolean free_current)
{
    if (free_current == TRUE && window->file_list != NULL)
        g_list_free (window->file_list);
    if (g_list_length(g_list_first(list)) != 1)
    {
        gtk_action_group_set_sensitive(window->actions_collection, TRUE);
        allow_slideshow(window);
    }
    else
    {
        gtk_action_group_set_sensitive(window->actions_collection, FALSE);
        deny_slideshow(window);
    }
    g_assert(list != NULL);
    window->file_list = list;
}

gboolean
vnr_window_next (VnrWindow *window, gboolean rem_timeout){
    GList *next;

    /* Don't reload current image
     * if the list contains only one (or no) image */
    if (g_list_length(g_list_first(window->file_list)) <2)
        return FALSE;

    if(window->mode == VNR_WINDOW_MODE_SLIDESHOW && rem_timeout)
        g_source_remove (window->ss_source_tag);

    next = g_list_next(window->file_list);
    if(next == NULL)
    {
        next = g_list_first(window->file_list);
    }

    window->file_list = next;

    if(!window->cursor_is_hidden)
        gdk_window_set_cursor(GTK_WIDGET(window)->window,
                              gdk_cursor_new(GDK_WATCH));
    /* This makes the cursor show NOW */
    gdk_flush();

    vnr_window_open(window, FALSE);
    if(!window->cursor_is_hidden)
        gdk_window_set_cursor(GTK_WIDGET(window)->window,
                              gdk_cursor_new(GDK_LEFT_PTR));

    if(window->mode == VNR_WINDOW_MODE_SLIDESHOW && rem_timeout)
        window->ss_source_tag = g_timeout_add_seconds (window->ss_timeout,
                                                       (GSourceFunc)next_image_src,
                                                       window);

    return TRUE;
}

gboolean
vnr_window_prev (VnrWindow *window){
    GList *prev;

    /* Don't reload current image
     * if the list contains only one (or no) image */
    if (g_list_length(g_list_first(window->file_list)) <2)
        return FALSE;

    if(window->mode == VNR_WINDOW_MODE_SLIDESHOW)
        g_source_remove (window->ss_source_tag);

    prev = g_list_previous(window->file_list);
    if(prev == NULL)
    {
        prev = g_list_last(window->file_list);
    }

    window->file_list = prev;

    if(!window->cursor_is_hidden)
        gdk_window_set_cursor(GTK_WIDGET(window)->window,
                              gdk_cursor_new(GDK_WATCH));
    /* This makes the cursor show NOW */
    gdk_flush();

    vnr_window_open(window, FALSE);
    if(!window->cursor_is_hidden)
        gdk_window_set_cursor(GTK_WIDGET(window)->window,
                              gdk_cursor_new(GDK_LEFT_PTR));

    if(window->mode == VNR_WINDOW_MODE_SLIDESHOW)
        window->ss_source_tag = g_timeout_add_seconds (window->ss_timeout,
                                                       (GSourceFunc)next_image_src,
                                                       window);

    return TRUE;
}

gboolean
vnr_window_first (VnrWindow *window){
    GList *prev;

    prev = g_list_first(window->file_list);

    if(vnr_message_area_is_critical(VNR_MESSAGE_AREA(window->msg_area)))
    {
        vnr_message_area_hide(VNR_MESSAGE_AREA(window->msg_area));
    }

    window->file_list = prev;

    if(!window->cursor_is_hidden)
        gdk_window_set_cursor(GTK_WIDGET(window)->window,
                              gdk_cursor_new(GDK_WATCH));
    /* This makes the cursor show NOW */
    gdk_flush();

    vnr_window_open(window, FALSE);
    if(!window->cursor_is_hidden)
        gdk_window_set_cursor(GTK_WIDGET(window)->window,
                              gdk_cursor_new(GDK_LEFT_PTR));
    return TRUE;
}

gboolean
vnr_window_last (VnrWindow *window){
    GList *prev;

    prev = g_list_last(window->file_list);

    if(vnr_message_area_is_critical(VNR_MESSAGE_AREA(window->msg_area)))
    {
        vnr_message_area_hide(VNR_MESSAGE_AREA(window->msg_area));
    }

    window->file_list = prev;

    if(!window->cursor_is_hidden)
        gdk_window_set_cursor(GTK_WIDGET(window)->window,
                              gdk_cursor_new(GDK_WATCH));
    /* This makes the cursor show NOW */
    gdk_flush();

    vnr_window_open(window, FALSE);
    if(!window->cursor_is_hidden)
        gdk_window_set_cursor(GTK_WIDGET(window)->window,
                              gdk_cursor_new(GDK_LEFT_PTR));
    return TRUE;
}

void
vnr_window_apply_preferences (VnrWindow *window)
{
    if(window->prefs->smooth_images && UNI_IMAGE_VIEW(window->view)->interp != GDK_INTERP_BILINEAR)
    {
        UNI_IMAGE_VIEW(window->view)->interp = GDK_INTERP_BILINEAR;
        gtk_widget_queue_draw(window->view);
    }
    else if(!window->prefs->smooth_images && UNI_IMAGE_VIEW(window->view)->interp != GDK_INTERP_NEAREST)
    {
        UNI_IMAGE_VIEW(window->view)->interp = GDK_INTERP_NEAREST;
        gtk_widget_queue_draw(window->view);
    }


    if(gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON(window->ss_timeout_widget)) != window->prefs->slideshow_timeout)
    {
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(window->ss_timeout_widget), (gdouble) window->prefs->slideshow_timeout);
    }
}

void
vnr_window_toggle_fullscreen (VnrWindow *window)
{
    gboolean fullscreen;

    fullscreen = (window->mode == VNR_WINDOW_MODE_NORMAL)?TRUE:FALSE;

    if (fullscreen)
        vnr_window_fullscreen (window);
    else
        vnr_window_unfullscreen (window);
}
