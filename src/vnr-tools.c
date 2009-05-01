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

#ifndef __VNR_IMAGE_H__
#define __VNR_IMAGE_H__

#include <glib.h>
#include <gio/gio.h>
#include <stdio.h>
#include "vnr-tools.h"

void
vnr_tools_fit_to_size (gint * width, gint * height, gint max_width, gint max_height)
{
    gfloat ratio, max_ratio;

    /* if size fits well, then exit */
    if (*width < max_width && *height < max_height)
        return;
    /* check if dividing by 0 */
    if (*width == 0 || max_height == 0)
        return;

    ratio = 1. * (*height) / (*width);
    max_ratio = 1. * max_height / max_width;

    if (max_ratio > ratio)
    {
        *width = max_width;
        *height = ratio * (*width);
    }
    else if (ratio > max_ratio)
    {
        *height = max_height;
        *width = (*height) / ratio;
    }
    else
    {
        *width = max_height;
        *height = max_height;
    }

    return;
}

GSList*
vnr_tools_get_list_from_array (gchar **files)
{
    GSList *uri_list = NULL;
    gint i;

    if (files == NULL) return NULL;

    for (i = 0; files[i]; i++) {
        char *uri_string;

        GFile *file;

        //printf("IN:%s\n",files[i]);
        file = g_file_new_for_commandline_arg (files[i]);

        uri_string = g_file_get_path (file);
        //printf("OUT:%s\n",uri_string);

        g_object_unref (file);

        if (uri_string) {
            uri_list = g_slist_prepend (uri_list, g_strdup (uri_string));
            g_free (uri_string);
        }
    }

    return g_slist_reverse (uri_list);
}

#endif /* __VNR_IMAGE_H__ */
