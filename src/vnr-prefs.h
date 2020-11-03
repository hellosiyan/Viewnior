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

#ifndef __VNR_PREFS_H__
#define __VNR_PREFS_H__

#include "config.h"

#include <glib.h>
#include <gtk/gtk.h>
#include <gio/gio.h>
#include <gdk/gdkkeysyms.h>

G_BEGIN_DECLS

typedef struct _VnrPrefs VnrPrefs;
typedef struct _VnrPrefsClass VnrPrefsClass;

#define VNR_TYPE_PREFS             (vnr_prefs_get_type ())
#define VNR_PREFS(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), VNR_TYPE_PREFS, VnrPrefs))
#define VNR_PREFS_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass),  VNR_TYPE_PREFS, VnrPrefsClass))
#define VNR_IS_PREFS(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VNR_TYPE_PREFS))
#define VNR_IS_PREFS_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass),  VNR_TYPE_PREFS))
#define VNR_PREFS_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj),  VNR_TYPE_PREFS, VnrPrefsClass))

typedef enum{
    VNR_PREFS_ZOOM_SMART,
    VNR_PREFS_ZOOM_NORMAL,
    VNR_PREFS_ZOOM_FIT,
    VNR_PREFS_ZOOM_LAST_USED,
} VnrPrefsZoom;


typedef enum{
    VNR_PREFS_DESKTOP_GNOME2,
    VNR_PREFS_DESKTOP_GNOME3,
    VNR_PREFS_DESKTOP_XFCE,
    VNR_PREFS_DESKTOP_LXDE,
    VNR_PREFS_DESKTOP_PUPPY,
    VNR_PREFS_DESKTOP_FLUXBOX,
    VNR_PREFS_DESKTOP_NITROGEN,
    VNR_PREFS_DESKTOP_MATE,
    VNR_PREFS_DESKTOP_CINNAMON,
    VNR_PREFS_DESKTOP_AUTO,
} VnrPrefsDesktop;

typedef enum{
    VNR_PREFS_WHEEL_NAVIGATE ,
    VNR_PREFS_WHEEL_ZOOM ,
    VNR_PREFS_WHEEL_SCROLL,
} VnrPrefsWheel ;

typedef enum{
    VNR_PREFS_CLICK_ZOOM ,
    VNR_PREFS_CLICK_FULLSCREEN ,
    VNR_PREFS_CLICK_NEXT ,
} VnrPrefsClick ;

typedef enum{
    VNR_PREFS_MODIFY_ASK ,
    VNR_PREFS_MODIFY_SAVE ,
    VNR_PREFS_MODIFY_IGNORE ,
} VnrPrefsModify ;

struct _VnrPrefs {
    GObject parent;

    VnrPrefsZoom zoom;

    VnrPrefsDesktop desktop;

    VnrPrefsWheel behavior_wheel;
    VnrPrefsClick behavior_click;
    VnrPrefsModify behavior_modify;
    gboolean fit_on_fullscreen;
    gboolean show_hidden;
    gboolean smooth_images;
    gboolean confirm_delete;
    gboolean reload_on_save;
    gboolean show_menu_bar;
    gboolean show_toolbar;
    gboolean show_scrollbar;
    gboolean show_statusbar;
    gboolean start_maximized;
    gboolean start_slideshow;
    gboolean start_fullscreen;
    gboolean auto_resize;
    int slideshow_timeout;
    int jpeg_quality;
    int png_compression;

    GtkWidget *dialog;
    GtkWidget *vnr_win;

    GtkSpinButton *slideshow_timeout_widget;
};

struct _VnrPrefsClass {
    GObjectClass parent_class;
};

GType     vnr_prefs_get_type (void) G_GNUC_CONST;

GObject*  vnr_prefs_new (GtkWidget *window);
void      vnr_prefs_show_dialog (VnrPrefs *prefs);
void      vnr_prefs_set_slideshow_timeout (VnrPrefs *prefs, int value);
void      vnr_prefs_set_show_menu_bar     (VnrPrefs *prefs, gboolean show_menu_bar);
void      vnr_prefs_set_show_toolbar      (VnrPrefs *prefs, gboolean show_toolbar);
void      vnr_prefs_set_show_scrollbar    (VnrPrefs *prefs, gboolean show_scollbar);
void      vnr_prefs_set_show_statusbar    (VnrPrefs *prefs, gboolean show_statusbar);
gboolean  vnr_prefs_save (VnrPrefs *prefs);

G_END_DECLS
#endif /* __VNR_PREFS_H__ */
