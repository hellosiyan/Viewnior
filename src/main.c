#include <gtk/gtk.h>
#include "vnr-window.h"
#include "uni-scroll-win.h"
#include "uni-image-view.h"
#include "uni-anim-view.h"

static char **files = NULL;     //array of files specified to be opened

/* List of option entries
 * The only option is for specifying file to be opened. */
static GOptionEntry opt_entries[] = {
    {G_OPTION_REMAINING, 0, 0, G_OPTION_ARG_FILENAME_ARRAY, &files, NULL,
     ("[FILE]")},
    {NULL}
};

int
main (int argc, char *argv[])
{
    GError *error = NULL;
    GOptionContext *opt_context;
    VnrWindow *win;

    /*GtkWidget * view;
       GtkWidget * scroll; */

    opt_context = g_option_context_new ("- Elegant Image Viewer");
    g_option_context_add_main_entries (opt_context, opt_entries, NULL);
    g_option_context_add_group (opt_context, gtk_get_option_group (TRUE));
    g_option_context_parse (opt_context, &argc, &argv, &error);

    if (error != NULL)
    {
        printf
            ("%s\nRun 'viewnior --help' to see a full list of available command line options.\n",
             error->message);
        return 1;
    }

    win = (VnrWindow *) vnr_window_new ();
    gtk_window_set_default_size (GTK_WINDOW (win), 800, 600);
    gtk_window_set_position (GTK_WINDOW (win), GTK_WIN_POS_CENTER);


    win->view = uni_anim_view_new ();
    win->scroll_view = uni_scroll_win_new (UNI_IMAGE_VIEW (win->view));

    /* Where "box" is a GtkBox already part of your layout. */
    gtk_container_add (GTK_CONTAINER (win), win->scroll_view);

    //printf("FILE: %s\n", *files);

    if (G_LIKELY (files))
    {
        vnr_window_open (win, *files);
    }


    gtk_widget_show_all (GTK_WIDGET (win));

    gtk_main ();

    return 0;
}
