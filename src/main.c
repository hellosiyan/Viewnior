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

#include <config.h>
#include <gtk/gtk.h>
#include "vnr-window.h"
#include "uni-scroll-win.h"
#include "uni-image-view.h"
#include "vnr-message-area.h"
#include "uni-anim-view.h"

static char **files = NULL;     //array of files specified to be opened
static gboolean version = FALSE;

/* List of option entries
 * The only option is for specifying file to be opened. */
static GOptionEntry opt_entries[] = {
    {G_OPTION_REMAINING, 0, 0, G_OPTION_ARG_FILENAME_ARRAY, &files, NULL, "[FILE]"},
    {"version", 0, 0, G_OPTION_ARG_NONE, &version, NULL, NULL},
    {NULL}
};

int
main (int argc, char *argv[])
{
    GError *error = NULL;
    GOptionContext *opt_context;
    VnrWindow *win;

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
    else if(version)
    {
        printf("%s\n", PACKAGE_STRING);
        return 0;
    }

    win = (VnrWindow *) vnr_window_new ();
    gtk_window_set_default_size (GTK_WINDOW (win), 480, 300);
    gtk_window_set_position (GTK_WINDOW (win), GTK_WIN_POS_CENTER);

    //printf("FILE: %s\n", *files);

    if (G_LIKELY (files))
    {
        vnr_window_open (win, *files);
    }
    else
    {
        vnr_message_area_show_warning(VNR_MESSAGE_AREA (win->msg_area),
                                      "No image specified!");
    }


    gtk_widget_show (GTK_WIDGET (win));
    gtk_main ();

    return 0;
}
