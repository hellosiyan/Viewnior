#include <gtk/gtk.h>
#include "vnr-window.h"
#include "uni-anim-view.h"
#include "vnr-tools.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

G_DEFINE_TYPE (VnrWindow, vnr_window, GTK_TYPE_WINDOW);

/* static void		vnr_window_finalize		(GObject *object); */
static gint vnr_window_delete (GtkWidget * widget, GdkEventAny * event);
/*static gint		vnr_window_key_press	(GtkWidget *widget, GdkEventKey *event);*/
/*static gboolean	vnr_window_state_event	( GtkWidget* widget, GdkEventWindowState* state );*/


static void
vnr_window_class_init (VnrWindowClass * klass)
{
    /* GObjectClass *g_object_class = (GObjectClass *) klass; */
    GtkWidgetClass *widget_class = (GtkWidgetClass *) klass;

    /*      g_object_class->constructor = eog_window_constructor;
     *      g_object_class->dispose = eog_window_dispose;
     *g_object_class->finalize = vnr_window_finalize;
     *      g_object_class->set_property = eog_window_set_property;
     *      g_object_class->get_property = eog_window_get_property; */

    widget_class->delete_event = vnr_window_delete;
    /*widget_class->key_press_event = vnr_window_key_press;
     *      widget_class->button_press_event = eog_window_button_press;
     *      widget_class->drag_data_received = eog_window_drag_data_received;
     *      widget_class->configure_event = eog_window_configure_event;
     *widget_class->window_state_event = vnr_window_state_event;
     *      widget_class->unrealize = eog_window_unrealize;
     *      widget_class->focus_in_event = eog_window_focus_in_event;
     *      widget_class->focus_out_event = eog_window_focus_out_event; */

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

    pixbuf = gdk_pixbuf_animation_new_from_file (path, &error);

    if (error != NULL)
    {
        /* Warn about the error! */
        printf ("ERROR: %s\n", error->message);
        return FALSE;
    }

    img_w = gdk_pixbuf_animation_get_width (pixbuf);
    img_h = gdk_pixbuf_animation_get_height (pixbuf);

    fit_to_size (&img_w, &img_h, win->max_width, win->max_height);

    gtk_window_resize (GTK_WINDOW (win), (img_w < 200) ? 200 : img_w,
                       (img_h < 200) ? 200 : img_h);

    uni_anim_view_set_anim (UNI_ANIM_VIEW (win->view), pixbuf);

    gtk_window_set_title (GTK_WINDOW (win),
                          g_strconcat (g_path_get_basename (path),
                                       " - Viewnior", NULL));

    return TRUE;
}
