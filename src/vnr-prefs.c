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

#include <libintl.h>
#include <glib/gi18n.h>
#define _(String) gettext (String)

#include "config.h"
#include "vnr-prefs.h"
#include "vnr-window.h"

#define UI_PATH PACKAGE_DATA_DIR"/viewnior/vnr-preferences-dialog.ui"

G_DEFINE_TYPE (VnrPrefs, vnr_prefs, G_TYPE_OBJECT);

/*************************************************************/
/***** Private signal handlers *******************************/
/*************************************************************/

static void
toggle_show_hidden_cb (GtkToggleButton *togglebutton, gpointer user_data)
{
    VNR_PREFS(user_data)->show_hidden = gtk_toggle_button_get_active(togglebutton);
    vnr_prefs_save(VNR_PREFS(user_data));
}

static void
toggle_fit_on_fullscreen_cb (GtkToggleButton *togglebutton, gpointer user_data)
{
    VNR_PREFS(user_data)->fit_on_fullscreen = gtk_toggle_button_get_active(togglebutton);
    vnr_prefs_save(VNR_PREFS(user_data));
}

static void
toggle_smooth_images_cb (GtkToggleButton *togglebutton, gpointer user_data)
{
    VNR_PREFS(user_data)->smooth_images = gtk_toggle_button_get_active(togglebutton);
    vnr_prefs_save(VNR_PREFS(user_data));
    vnr_window_apply_preferences(VNR_WINDOW(VNR_PREFS(user_data)->vnr_win));
}

static void
toggle_confirm_delete_cb (GtkToggleButton *togglebutton, gpointer user_data)
{
    VNR_PREFS(user_data)->confirm_delete = gtk_toggle_button_get_active(togglebutton);
    vnr_prefs_save(VNR_PREFS(user_data));
}

static void
toggle_reload_on_save_cb (GtkToggleButton *togglebutton, gpointer user_data)
{
    VNR_PREFS(user_data)->reload_on_save = gtk_toggle_button_get_active(togglebutton);
    vnr_prefs_save(VNR_PREFS(user_data));
}

static void
change_zoom_mode_cb (GtkComboBox *widget, gpointer user_data)
{
    VNR_PREFS(user_data)->zoom = gtk_combo_box_get_active(widget);
    vnr_prefs_save(VNR_PREFS(user_data));
}

static void
change_desktop_env_cb (GtkComboBox *widget, gpointer user_data)
{
    VNR_PREFS(user_data)->desktop = gtk_combo_box_get_active(widget);
    vnr_prefs_save(VNR_PREFS(user_data));
}

static void
change_jpeg_quality_cb (GtkRange *range, gpointer user_data)
{
    VNR_PREFS(user_data)->jpeg_quality = (int) gtk_range_get_value(range);
    vnr_prefs_save(VNR_PREFS(user_data));
}

static void
change_png_compression_cb (GtkRange *range, gpointer user_data)
{
    VNR_PREFS(user_data)->png_compression = (int) gtk_range_get_value(range);
    vnr_prefs_save(VNR_PREFS(user_data));
}

static void
change_action_wheel_cb (GtkComboBox *widget, gpointer user_data)
{
    VNR_PREFS(user_data)->behavior_wheel = gtk_combo_box_get_active(widget);
    vnr_prefs_save(VNR_PREFS(user_data));
}

static void
change_action_click_cb (GtkComboBox *widget, gpointer user_data)
{
    VNR_PREFS(user_data)->behavior_click = gtk_combo_box_get_active(widget);
    vnr_prefs_save(VNR_PREFS(user_data));
}

static void
change_action_modify_cb (GtkComboBox *widget, gpointer user_data)
{
    VNR_PREFS(user_data)->behavior_modify = gtk_combo_box_get_active(widget);
    vnr_prefs_save(VNR_PREFS(user_data));
}

static void
change_spin_value_cb (GtkSpinButton *spinbutton, gpointer user_data)
{
    int new_value;

    new_value = gtk_spin_button_get_value_as_int (spinbutton);

    VNR_PREFS(user_data)->slideshow_timeout = new_value;
    vnr_prefs_save(VNR_PREFS(user_data));
    vnr_window_apply_preferences(VNR_WINDOW(VNR_PREFS(user_data)->vnr_win));
}

static gboolean
key_press_cb (GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
    if(event->keyval == GDK_KEY_Escape)
    {
        gtk_widget_hide(widget);
        return TRUE;
    }
    else
        return FALSE;
}

/*************************************************************/
/***** Private actions ***************************************/
/*************************************************************/

static void
vnr_prefs_set_default(VnrPrefs *prefs)
{
    prefs->zoom = VNR_PREFS_ZOOM_SMART;
    prefs->show_hidden = FALSE;
    prefs->fit_on_fullscreen = TRUE;
    prefs->smooth_images = TRUE;
    prefs->confirm_delete = TRUE;
    prefs->slideshow_timeout = 5;
    prefs->behavior_wheel = VNR_PREFS_WHEEL_ZOOM;
    prefs->behavior_click = VNR_PREFS_CLICK_ZOOM;
    prefs->behavior_modify = VNR_PREFS_MODIFY_ASK;
    prefs->jpeg_quality = 90;
    prefs->png_compression = 9;
    prefs->reload_on_save = FALSE;
    prefs->show_menu_bar = FALSE;
    prefs->show_toolbar = TRUE;
    prefs->show_scrollbar = TRUE;
    prefs->show_statusbar = FALSE;
    prefs->start_maximized = FALSE;
    prefs->start_slideshow = FALSE;
    prefs->start_fullscreen = FALSE;
    prefs->auto_resize = FALSE;
    prefs->desktop = VNR_PREFS_DESKTOP_AUTO;
}

static GtkWidget *
build_dialog (VnrPrefs *prefs)
{
    GtkBuilder *builder;
    GtkWidget *window;
    GError *error = NULL;

    GObject *close_button;
    GtkToggleButton *show_hidden;
    GtkToggleButton *fit_on_fullscreen;
    GtkBox *zoom_mode_box;
    GtkComboBox *zoom_mode;
    GtkToggleButton *smooth_images;
    GtkToggleButton *confirm_delete;
    GtkToggleButton *reload_on_save;
    GtkSpinButton *slideshow_timeout;
    GtkTable *behavior_table;
    GtkComboBox *action_wheel;
    GtkComboBox *action_click;
    GtkComboBox *action_modify;
    GtkRange *jpeg_scale;
    GtkRange *png_scale;

    GtkBox *desktop_box;
    GtkComboBox *desktop_env;

    builder = gtk_builder_new ();
    gtk_builder_add_from_file (builder, UI_PATH, &error);

    if (error != NULL)
    {
        g_warning ("%s\n", error->message);
        g_object_unref(builder);
        return NULL;
    }

    window = GTK_WIDGET (gtk_builder_get_object (builder, "window"));

    /* Close button */
    close_button = gtk_builder_get_object (builder, "close_button");
    g_signal_connect_swapped(close_button, "clicked",
                             G_CALLBACK(gtk_widget_hide_on_delete), window);

    /* Show hidden files checkbox */
    show_hidden = GTK_TOGGLE_BUTTON (gtk_builder_get_object (builder, "show_hidden"));
    gtk_toggle_button_set_active( show_hidden, prefs->show_hidden );
    g_signal_connect(G_OBJECT(show_hidden), "toggled", G_CALLBACK(toggle_show_hidden_cb), prefs);

    /* Fit on fullscreen checkbox */
    fit_on_fullscreen = GTK_TOGGLE_BUTTON (gtk_builder_get_object (builder, "fit_on_fullscreen"));
    gtk_toggle_button_set_active( fit_on_fullscreen, prefs->fit_on_fullscreen );
    g_signal_connect(G_OBJECT(fit_on_fullscreen), "toggled", G_CALLBACK(toggle_fit_on_fullscreen_cb), prefs);

    /* Smooth images checkbox */
    smooth_images = GTK_TOGGLE_BUTTON (gtk_builder_get_object (builder, "smooth_images"));
    gtk_toggle_button_set_active( smooth_images, prefs->smooth_images );
    g_signal_connect(G_OBJECT(smooth_images), "toggled", G_CALLBACK(toggle_smooth_images_cb), prefs);

    /* Confirm delete checkbox */
    confirm_delete = GTK_TOGGLE_BUTTON (gtk_builder_get_object (builder, "confirm_delete"));
    gtk_toggle_button_set_active( confirm_delete, prefs->confirm_delete );
    g_signal_connect(G_OBJECT(confirm_delete), "toggled", G_CALLBACK(toggle_confirm_delete_cb), prefs);

    /* Reload image after save checkbox */
    reload_on_save = GTK_TOGGLE_BUTTON (gtk_builder_get_object (builder, "reload"));
    gtk_toggle_button_set_active( reload_on_save, prefs->reload_on_save );
    g_signal_connect(G_OBJECT(reload_on_save), "toggled", G_CALLBACK(toggle_reload_on_save_cb), prefs);

    /* Slideshow timeout spin button */
    slideshow_timeout = GTK_SPIN_BUTTON (gtk_builder_get_object (builder, "slideshow_timeout"));
    gtk_spin_button_set_value( slideshow_timeout, (gdouble)prefs->slideshow_timeout);
    prefs->slideshow_timeout_widget = slideshow_timeout;
    g_signal_connect(G_OBJECT(slideshow_timeout), "value-changed", G_CALLBACK(change_spin_value_cb), prefs);

    /* JPEG quality scale */
    jpeg_scale = GTK_RANGE (gtk_builder_get_object (builder, "jpeg_scale"));
    gtk_range_set_value(jpeg_scale, (gdouble)prefs->jpeg_quality);
    g_signal_connect(G_OBJECT(jpeg_scale), "value-changed", G_CALLBACK(change_jpeg_quality_cb), prefs);

    /* PNG compression scale */
    png_scale = GTK_RANGE (gtk_builder_get_object (builder, "png_scale"));
    gtk_range_set_value(png_scale, (gdouble)prefs->png_compression);
    g_signal_connect(G_OBJECT(png_scale), "value-changed", G_CALLBACK(change_png_compression_cb), prefs);

    /* Zoom mode combo box */
    zoom_mode_box = GTK_BOX (gtk_builder_get_object (builder, "zoom_mode_box"));

    zoom_mode = (GtkComboBox*) gtk_combo_box_new_text();
    gtk_combo_box_append_text(zoom_mode, _("Smart Mode"));
    gtk_combo_box_append_text(zoom_mode, _("1:1 Mode"));
    gtk_combo_box_append_text(zoom_mode, _("Fit To Window Mode"));
    gtk_combo_box_append_text(zoom_mode, _("Last Used Mode"));
    gtk_combo_box_set_active(zoom_mode, prefs->zoom);

    gtk_box_pack_end (zoom_mode_box, GTK_WIDGET(zoom_mode), FALSE, FALSE, 0);
    gtk_widget_show(GTK_WIDGET(zoom_mode));

    g_signal_connect(G_OBJECT(zoom_mode), "changed", G_CALLBACK(change_zoom_mode_cb), prefs);

    /* Desktop combo box */
    desktop_box = GTK_BOX (gtk_builder_get_object (builder, "desktop_box"));

    desktop_env = (GtkComboBox*) gtk_combo_box_new_text();
    gtk_combo_box_append_text(desktop_env, "GNOME 2");
    gtk_combo_box_append_text(desktop_env, "GNOME 3");
    gtk_combo_box_append_text(desktop_env, "XFCE");
    gtk_combo_box_append_text(desktop_env, "LXDE");
    gtk_combo_box_append_text(desktop_env, "PUPPY");
    gtk_combo_box_append_text(desktop_env, "FluxBox");
    gtk_combo_box_append_text(desktop_env, "Nitrogen");
    gtk_combo_box_append_text(desktop_env, "MATE");
    gtk_combo_box_append_text(desktop_env, "Cinnamon");
    gtk_combo_box_append_text(desktop_env, _("Autodetect"));
    gtk_combo_box_set_active(desktop_env, prefs->desktop);

    gtk_box_pack_end (desktop_box, GTK_WIDGET(desktop_env), FALSE, FALSE, 0);
    gtk_widget_show(GTK_WIDGET(desktop_env));

    g_signal_connect(G_OBJECT(desktop_env), "changed", G_CALLBACK(change_desktop_env_cb), prefs);

    /* Behavior combo boxes */
    behavior_table = GTK_TABLE (gtk_builder_get_object (builder, "behavior_table"));

    action_wheel = (GtkComboBox*) gtk_combo_box_new_text();
    gtk_combo_box_append_text(action_wheel, _("Navigate images"));
    gtk_combo_box_append_text(action_wheel, _("Zoom image"));
    gtk_combo_box_append_text(action_wheel, _("Scroll image up/down"));
    gtk_combo_box_set_active(action_wheel, prefs->behavior_wheel);

    gtk_table_attach (behavior_table, GTK_WIDGET(action_wheel), 1,2,0,1, GTK_FILL,0, 0,0);
    gtk_widget_show(GTK_WIDGET(action_wheel));
    g_signal_connect(G_OBJECT(action_wheel), "changed", G_CALLBACK(change_action_wheel_cb), prefs);

    action_click = (GtkComboBox*) gtk_combo_box_new_text();
    gtk_combo_box_append_text(action_click, _("Switch zoom modes"));
    gtk_combo_box_append_text(action_click, _("Enter fullscreen mode"));
    gtk_combo_box_append_text(action_click, _("Navigate images"));
    gtk_combo_box_set_active(action_click, prefs->behavior_click);

    gtk_table_attach (behavior_table, GTK_WIDGET(action_click), 1,2,1,2, GTK_FILL,0, 0,0);
    gtk_widget_show(GTK_WIDGET(action_click));
    g_signal_connect(G_OBJECT(action_click), "changed", G_CALLBACK(change_action_click_cb), prefs);

    action_modify = (GtkComboBox*) gtk_combo_box_new_text();
    gtk_combo_box_append_text(action_modify, _("Ask every time"));
    gtk_combo_box_append_text(action_modify, _("Autosave"));
    gtk_combo_box_append_text(action_modify, _("Ignore changes"));
    gtk_combo_box_set_active(action_modify, prefs->behavior_modify);

    gtk_table_attach (behavior_table, GTK_WIDGET(action_modify), 1,2,2,3, GTK_FILL,0, 0,0);
    gtk_widget_show(GTK_WIDGET(action_modify));
    g_signal_connect(G_OBJECT(action_modify), "changed", G_CALLBACK(change_action_modify_cb), prefs);

    /* Window signals */
    g_signal_connect(G_OBJECT(window), "delete-event",
                     G_CALLBACK(gtk_widget_hide_on_delete), NULL);
    g_signal_connect(G_OBJECT(window), "key-press-event", G_CALLBACK(key_press_cb), NULL);


    g_object_unref (G_OBJECT (builder));

    return window;
}

static gboolean
vnr_prefs_load (VnrPrefs *prefs)
{
    GKeyFile *conf;
    GError *error = NULL;
    const gchar *path;

    path = g_build_filename (g_get_user_config_dir(), PACKAGE, "viewnior.conf", NULL);

    conf = g_key_file_new();
    g_key_file_load_from_file (conf, path, G_KEY_FILE_NONE, &error);

    g_free((char*)path);

    if(error != NULL)
    {
        g_warning("Loading config file: %s. All preferences are set to their default values. Saving ...", error->message);
        g_key_file_free (conf);
        return FALSE;
    }

    prefs->zoom = g_key_file_get_integer (conf, "prefs", "zoom-mode", &error);
    prefs->fit_on_fullscreen = g_key_file_get_boolean (conf, "prefs", "fit-on-fullscreen", &error);
    prefs->show_hidden = g_key_file_get_boolean (conf, "prefs", "show-hidden", &error);
    prefs->smooth_images = g_key_file_get_boolean (conf, "prefs", "smooth-images", &error);
    prefs->confirm_delete = g_key_file_get_boolean (conf, "prefs", "confirm-delete", &error);
    prefs->reload_on_save = g_key_file_get_boolean (conf, "prefs", "reload-on-save", &error);
    prefs->show_menu_bar = g_key_file_get_boolean (conf, "prefs", "show-menu-bar", &error);
    prefs->show_toolbar = g_key_file_get_boolean (conf, "prefs", "show-toolbar", &error);
    prefs->show_scrollbar = g_key_file_get_boolean (conf, "prefs", "show-scrollbar", &error);
    prefs->show_statusbar = g_key_file_get_boolean (conf, "prefs", "show-statusbar", &error);
    prefs->start_maximized = g_key_file_get_boolean (conf, "prefs", "start-maximized", &error);
    prefs->slideshow_timeout = g_key_file_get_integer (conf, "prefs", "slideshow-timeout", &error);
    prefs->auto_resize = g_key_file_get_boolean (conf, "prefs", "auto-resize", &error);
    prefs->behavior_wheel = g_key_file_get_integer (conf, "prefs", "behavior-wheel", &error);
    prefs->behavior_click = g_key_file_get_integer (conf, "prefs", "behavior-click", &error);
    prefs->behavior_modify = g_key_file_get_integer (conf, "prefs", "behavior-modify", &error);
    prefs->jpeg_quality = g_key_file_get_integer (conf, "prefs", "jpeg-quality", &error);
    prefs->png_compression = g_key_file_get_integer (conf, "prefs", "png-compression", &error);
    prefs->desktop = g_key_file_get_integer (conf, "prefs", "desktop", &error);

    if(error != NULL)
    {
        g_warning("Parsing config file: %s. All preferences are set to their default values.", error->message);
        g_key_file_free (conf);
        return FALSE;
    }

    g_key_file_free (conf);

    return TRUE;
}

/*************************************************************/
/***** Stuff that deals with the type ************************/
/*************************************************************/

static void
vnr_prefs_class_init (VnrPrefsClass * klass) {}

GObject *
vnr_prefs_new (GtkWidget *vnr_win)
{
    VnrPrefs *prefs;

    prefs = g_object_new (VNR_TYPE_PREFS, NULL);

    prefs->vnr_win = vnr_win;

    return (GObject *) prefs;
}

static void
vnr_prefs_init (VnrPrefs * prefs)
{
    if ( !vnr_prefs_load (prefs) )
    {
        vnr_prefs_set_default(prefs);
        vnr_prefs_save (prefs);
    }

    prefs->dialog = NULL;
}

/*************************************************************/
/***** Actions ***********************************************/
/*************************************************************/

void
vnr_prefs_show_dialog(VnrPrefs *prefs)
{
    if (prefs->dialog == NULL)
    {
        prefs->dialog = build_dialog (prefs);
        if (prefs->dialog == NULL)
            return;
    }
    gtk_window_present(GTK_WINDOW(prefs->dialog));
}

gboolean
vnr_prefs_save (VnrPrefs *prefs)
{
    GKeyFile *conf;
    FILE *rcfile;
    const gchar *dir;
    const gchar *path;

    dir = g_build_filename (g_get_user_config_dir(), PACKAGE, NULL);
    path = g_build_filename (dir, "viewnior.conf", NULL);

    conf = g_key_file_new();
    g_key_file_set_integer (conf, "prefs", "zoom-mode", prefs->zoom);
    g_key_file_set_boolean (conf, "prefs", "fit-on-fullscreen", prefs->fit_on_fullscreen);
    g_key_file_set_boolean (conf, "prefs", "show-hidden", prefs->show_hidden);
    g_key_file_set_boolean (conf, "prefs", "smooth-images", prefs->smooth_images);
    g_key_file_set_boolean (conf, "prefs", "confirm-delete", prefs->confirm_delete);
    g_key_file_set_boolean (conf, "prefs", "reload-on-save", prefs->reload_on_save);
    g_key_file_set_boolean (conf, "prefs", "show-menu-bar", prefs->show_menu_bar);
    g_key_file_set_boolean (conf, "prefs", "show-toolbar", prefs->show_toolbar);
    g_key_file_set_boolean (conf, "prefs", "show-scrollbar", prefs->show_scrollbar);
    g_key_file_set_boolean (conf, "prefs", "show-statusbar", prefs->show_statusbar);
    g_key_file_set_boolean (conf, "prefs", "start-maximized", prefs->start_maximized);
    g_key_file_set_integer (conf, "prefs", "slideshow-timeout", prefs->slideshow_timeout);
    g_key_file_set_boolean (conf, "prefs", "auto-resize", prefs->auto_resize);
    g_key_file_set_integer (conf, "prefs", "behavior-wheel", prefs->behavior_wheel);
    g_key_file_set_integer (conf, "prefs", "behavior-click", prefs->behavior_click);
    g_key_file_set_integer (conf, "prefs", "behavior-modify", prefs->behavior_modify);
    g_key_file_set_integer (conf, "prefs", "jpeg-quality", prefs->jpeg_quality);
    g_key_file_set_integer (conf, "prefs", "png-compression", prefs->png_compression);
    g_key_file_set_integer (conf, "prefs", "desktop", prefs->desktop);

    if(g_mkdir_with_parents (dir, 0700) != 0)
        g_warning("Error creating config file's parent directory (%s)\n", dir);

    rcfile = fopen(path , "w");

    if(rcfile != NULL)
    {
        gchar *data = g_key_file_to_data (conf, NULL, NULL);
        fputs(data, rcfile);
        fclose(rcfile);
        g_free(data);
    }
    else
        g_warning("Saving config file: Unable to open the configuration file for writing!\n");

    g_key_file_free (conf);
    g_free((char*)dir);
    g_free((char*)path);

    return TRUE;
}

void
vnr_prefs_set_slideshow_timeout (VnrPrefs *prefs, int value)
{
    if (prefs->dialog != NULL)
        gtk_spin_button_set_value(prefs->slideshow_timeout_widget, (gdouble)value);
}

void
vnr_prefs_set_show_toolbar (VnrPrefs *prefs, gboolean show_toolbar)
{
    if(prefs->show_toolbar != show_toolbar)
    {
        prefs->show_toolbar = show_toolbar;
        vnr_prefs_save(prefs);
    }
}

void
vnr_prefs_set_show_scrollbar (VnrPrefs *prefs, gboolean show_scrollbar)
{
    if(prefs->show_scrollbar != show_scrollbar)
    {
        prefs->show_scrollbar = show_scrollbar;
        vnr_prefs_save(prefs);
    }
}

void
vnr_prefs_set_show_statusbar (VnrPrefs *prefs, gboolean show_statusbar)
{
    if(prefs->show_statusbar != show_statusbar)
    {
        prefs->show_statusbar = show_statusbar;
        vnr_prefs_save(prefs);
    }
}

void
vnr_prefs_set_show_menu_bar (VnrPrefs *prefs, gboolean show_menu_bar)
{
    if(prefs->show_menu_bar != show_menu_bar)
    {
        prefs->show_menu_bar = show_menu_bar;
        vnr_prefs_save(prefs);
    }
}
