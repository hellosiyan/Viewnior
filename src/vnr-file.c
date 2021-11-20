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

#include <gtk/gtk.h>
#include <gio/gio.h>
#include <gdk/gdkpixbuf.h>
#include "vnr-file.h"
#include "vnr-tools.h"

G_DEFINE_TYPE (VnrFile, vnr_file, G_TYPE_OBJECT);

GList * supported_mime_types;

static gint
compare_files(VnrFile *file, char *uri)
{
    if(g_strcmp0(uri, file->path) == 0)
        return 0;
    else
        return 1;
}

/* Modified version of eog's eog_image_get_supported_mime_types */
static GList *
vnr_file_get_supported_mime_types (void)
{
    GSList *format_list, *it;

    if (!supported_mime_types) {
        format_list = gdk_pixbuf_get_formats ();

        for (it = format_list; it != NULL; it = it->next) {
            gchar **mime_types = gdk_pixbuf_format_get_mime_types((GdkPixbufFormat *) it->data);

            int i;
            for (i = 0; mime_types[i] != NULL; i++) {
                supported_mime_types =
                    g_list_prepend (supported_mime_types,
                            g_strdup (mime_types[i]));
            }

            g_strfreev (mime_types);
        }

        supported_mime_types = g_list_prepend(supported_mime_types,
                                              "image/vnd.microsoft.icon");

        supported_mime_types = g_list_sort (supported_mime_types,
                            (GCompareFunc) compare_quarks);

        g_slist_free (format_list);
    }

    return supported_mime_types;
}

static gboolean
vnr_file_is_supported_mime_type (const char *mime_type)
{
    GList *result;
    GQuark quark;

    if (mime_type == NULL) {
        return FALSE;
    }

    supported_mime_types = vnr_file_get_supported_mime_types ();

    quark = g_quark_from_string (mime_type);

    result = g_list_find_custom (supported_mime_types,
                     GINT_TO_POINTER (quark),
                     (GCompareFunc) compare_quarks);

    return (result != NULL);
}

static void
vnr_file_class_init (VnrFileClass * klass)
{
}

static void
vnr_file_init (VnrFile * file)
{
    file->display_name = NULL;
}

VnrFile *
vnr_file_new ()
{
    return VNR_FILE (g_object_new (VNR_TYPE_FILE, NULL));
}

static void
vnr_file_set_display_name(VnrFile *vnr_file, char *display_name)
{
    vnr_file->display_name = g_strdup(display_name);
    vnr_file->display_name_collate = g_utf8_collate_key_for_filename(display_name, -1);
}


static gint
vnr_file_list_compare(gconstpointer a, gconstpointer b, gpointer user_data){
    return g_strcmp0(VNR_FILE(a)->display_name_collate,
                     VNR_FILE(b)->display_name_collate);
}


static GList *
vnr_file_dir_content_to_list(gchar *path, gboolean sort, gboolean include_hidden)
{
    GList *file_list = NULL;
    GFile *file;
    GFileEnumerator *f_enum ;
    GFileInfo *file_info;

    file = g_file_new_for_path(path);
    f_enum = g_file_enumerate_children(file, G_FILE_ATTRIBUTE_STANDARD_NAME","
                                       G_FILE_ATTRIBUTE_STANDARD_DISPLAY_NAME","
                                       G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE","
                                       G_FILE_ATTRIBUTE_STANDARD_IS_HIDDEN","
                                       G_FILE_ATTRIBUTE_TIME_MODIFIED,
                                       G_FILE_QUERY_INFO_NONE,
                                       NULL, NULL);
    file_info = g_file_enumerator_next_file(f_enum,NULL,NULL);


    while(file_info != NULL){
        VnrFile *vnr_file = vnr_file_new();

        const char *mimetype =g_file_info_get_content_type(file_info);

        if(vnr_file_is_supported_mime_type(mimetype) && (include_hidden || !g_file_info_get_is_hidden (file_info)) ){
            vnr_file_set_display_name(vnr_file, (char*)g_file_info_get_display_name (file_info));

            vnr_file->mtime = g_file_info_get_attribute_uint64 (file_info, G_FILE_ATTRIBUTE_TIME_MODIFIED);

            vnr_file->path =g_strjoin(G_DIR_SEPARATOR_S, path,
                                      vnr_file->display_name, NULL);

            file_list = g_list_prepend(file_list, vnr_file);
        }

        g_object_unref(file_info);
        file_info = g_file_enumerator_next_file(f_enum,NULL,NULL);
    }

    g_object_unref (file);
    g_file_enumerator_close (f_enum, NULL, NULL);
    g_object_unref (f_enum);

    if(sort)
        file_list = g_list_sort_with_data(file_list,
                                          vnr_file_list_compare, NULL);

    return file_list;
}


void
vnr_file_load_single_uri(char *p_path, GList **file_list, gboolean include_hidden, GError **error)
{
    GFile *file;
    GFileInfo *fileinfo;
    GFileType filetype;

    file = g_file_new_for_path(p_path);
    fileinfo = g_file_query_info (file, G_FILE_ATTRIBUTE_STANDARD_TYPE","
                                  G_FILE_ATTRIBUTE_STANDARD_DISPLAY_NAME,
                                  0, NULL, error);

    if (fileinfo == NULL)
        return;

    filetype = g_file_info_get_file_type(fileinfo);

    if (filetype == G_FILE_TYPE_DIRECTORY)
    {
        *file_list = vnr_file_dir_content_to_list(p_path, TRUE, include_hidden);
    }
    else
    {
        GFile *parent;
        GList *current_position;

        parent = g_file_get_parent(file);
        *file_list = vnr_file_dir_content_to_list(g_file_get_path(parent), TRUE, include_hidden);

        g_object_unref(parent);

        current_position = g_list_find_custom(*file_list, p_path,
                                  (GCompareFunc)compare_files);

        if(current_position != NULL)
            *file_list = current_position;
        else if(*file_list == NULL)
            return;
        else
        {
            *error = g_error_new(1, 0,
                                 _("Couldn't recognise the image file\n"
                                 "format for file '%s'"),
                                 g_file_info_get_display_name (fileinfo));
        }
    }
    g_object_unref (file);
    g_object_unref(fileinfo);
}

void
vnr_file_load_uri_list (GSList *uri_list, GList **file_list, gboolean include_hidden, GError **error)
{
    GFileType filetype;
    gchar *p_path;

    while(uri_list != NULL)
    {
        p_path = uri_list->data;
        GFile *file = g_file_new_for_path(p_path);
        GFileInfo *fileinfo = g_file_query_info (file, G_FILE_ATTRIBUTE_STANDARD_TYPE","
                                      G_FILE_ATTRIBUTE_STANDARD_DISPLAY_NAME","
                                      G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE","
                                      G_FILE_ATTRIBUTE_STANDARD_IS_HIDDEN","
                                       G_FILE_ATTRIBUTE_TIME_MODIFIED,
                                      0, NULL, error);

        if (fileinfo == NULL)
        {
            g_clear_error (error);
            g_object_unref (file);

            uri_list = g_slist_next(uri_list);
            continue;
        }

        filetype = g_file_info_get_file_type(fileinfo);

        if (filetype == G_FILE_TYPE_DIRECTORY)
        {
            *file_list = g_list_concat (*file_list, vnr_file_dir_content_to_list(p_path, FALSE, include_hidden));
        }
        else
        {
            VnrFile *new_vnrfile;
            const char *mimetype;

            new_vnrfile = vnr_file_new();

            mimetype = g_file_info_get_content_type(fileinfo);

            if(vnr_file_is_supported_mime_type(mimetype) && (include_hidden || !g_file_info_get_is_hidden (fileinfo)) )
            {
                vnr_file_set_display_name(new_vnrfile, (char*)g_file_info_get_display_name (fileinfo));

                new_vnrfile->mtime = g_file_info_get_attribute_uint64 (fileinfo, G_FILE_ATTRIBUTE_TIME_MODIFIED);

                new_vnrfile->path = g_strdup(p_path);

                *file_list = g_list_prepend(*file_list, new_vnrfile);
            }
        }
        g_object_unref (file);
        g_object_unref (fileinfo);

        uri_list = g_slist_next(uri_list);
    }

    *file_list = g_list_sort_with_data(*file_list, vnr_file_list_compare, NULL);
}
