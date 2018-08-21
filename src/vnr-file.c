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

#include "vnr-file.h"

#include <stdlib.h>

#define UNUSED(x) (void)(x)

G_DEFINE_TYPE (VnrFile, vnr_file, G_TYPE_OBJECT)


static void vnr_file_class_init (VnrFileClass *klass) {
    UNUSED(klass);
}

static void vnr_file_init(VnrFile *file) {
    file->display_name = NULL;
}

VnrFile * vnr_file_new() {
    return VNR_FILE (g_object_new (VNR_TYPE_FILE, NULL));
}

static void vnr_file_set_display_name(VnrFile *vnr_file, char *display_name) {
    vnr_file->display_name = g_strdup(display_name);
    vnr_file->display_name_collate = g_utf8_collate_key_for_filename(vnr_file->display_name, -1);
}


static void vnr_file_set_file_info(VnrFile *vnrfile,
                                   char *path,
                                   char *display_name,
                                   gboolean is_directory)
{
    vnrfile->path = g_strdup(path);
    vnrfile->is_directory = is_directory;
    vnr_file_set_display_name(vnrfile, display_name);
}

VnrFile* vnr_file_create_new(gchar *path,
                             char *display_name,
                             gboolean is_directory)
{
    VnrFile *vnrfile = vnr_file_new();
    vnr_file_set_file_info(vnrfile, path, display_name, is_directory);
    return vnrfile;
}

void vnr_file_destroy_data(VnrFile *vnrfile) {
    if(vnrfile == NULL) {
        return;
    }
    if(vnrfile->monitoring_data != NULL) {
        free(vnrfile->monitoring_data);
    }
    if(vnrfile->monitor != NULL) {
        g_file_monitor_cancel(vnrfile->monitor);
        g_object_unref(vnrfile->monitor);
    }
    g_free(vnrfile->path);
    g_free(vnrfile->display_name);
    g_free((gpointer) vnrfile->display_name_collate);
    g_object_unref(vnrfile);
}

gboolean vnr_file_is_directory(VnrFile* vnrfile) {
    return vnrfile != NULL && vnrfile->is_directory;
}

gboolean vnr_file_is_image_file(VnrFile* vnrfile) {
    return vnrfile != NULL && !vnrfile->is_directory;
}
