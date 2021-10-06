/*
 * Copyright © 2009-2018 Siyan Panayotov <contact@siyanpanayotov.com>
 *
 * Based on code by (see README for details):
 * - Björn Lindqvist <bjourne@gmail.com>
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

#include <gexiv2/gexiv2.h>
#include <glib.h>
#include <glib/gi18n.h>

#include "uni-exiv2.h"

#define ARRAY_SIZE(array) (sizeof array/sizeof(array[0]))

static GExiv2Metadata *cached_image;

static void
uni_get_exiv2_tags(GExiv2Metadata *image,
                   const gchar *const *tags,
                   size_t tags_size,
                   UniReadExiv2Callback callback,
                   void *user_data)
{
    for (uint i = 0; i < tags_size; i++) {
        if (gexiv2_metadata_has_tag(image, tags[i])) {
            const gchar *label = gexiv2_metadata_get_tag_label(tags[i]);
            gchar *value = gexiv2_metadata_get_tag_interpreted_string(image, tags[i]);

            callback(label, value, user_data);

            g_free(value);
        }
    }
}

static void
uni_clear_exiv2_cache(void)
{
    gexiv2_metadata_clear(cached_image);
    g_object_unref(cached_image);
    cached_image = NULL;
}

void
uni_read_exiv2_map(const char *uri, UniReadExiv2Callback callback, void *user_data)
{
    GExiv2Metadata *image;
    GError *err;
    gchar *comment;

    gexiv2_log_set_level(GEXIV2_LOG_LEVEL_MUTE);

    image = gexiv2_metadata_new();
    if (!gexiv2_metadata_open_path(image, uri, &err)) {
        g_error("Exiv2: '%s'", err->message);
        return;
    }

    if (gexiv2_metadata_has_exif(image)) {
        static const gchar *exifTags[] = {"Exif.Image.UniqueCameraModel",
                                          "Exif.Image.LocalizedCameraModel",
                                          "Exif.Photo.DateTimeOriginal",
                                          "Exif.Photo.ExposureTime",
                                          "Exif.Photo.ExposureMode",
                                          "Exif.Photo.ApertureValue",
                                          "Exif.Photo.ISOSpeedRatings",
                                          "Exif.Photo.Flash",
                                          "Exif.Photo.MeteringMode",
                                          "Exif.Image.FocalLength",
                                          "Exif.Image.Software",
                                          "Exif.Image.ImageDescription",
                                          "Exif.Photo.UserComment"};

        uni_get_exiv2_tags(image, exifTags, ARRAY_SIZE(exifTags), callback, user_data);
    }

    comment = gexiv2_metadata_get_comment(image);
    if (comment) {
        if (*comment) {
            callback(_("Comment"), comment, user_data);
        }
        g_free(comment);
    }

    if (gexiv2_metadata_has_iptc(image)) {
        static const gchar *iptcTags[] = {"Iptc.Application2.Caption",
                                          "Iptc.Application2.Copyright",
                                          "Iptc.Application2.Byline"};

        uni_get_exiv2_tags(image, iptcTags, ARRAY_SIZE(iptcTags), callback, user_data);
    }

    g_object_unref(image);
}

int
uni_read_exiv2_to_cache(const char *uri)
{
    GError *err;

    gexiv2_log_set_level(GEXIV2_LOG_LEVEL_MUTE);

    if (cached_image != NULL) {
        uni_clear_exiv2_cache();
    }

    cached_image = gexiv2_metadata_new();

    if (!gexiv2_metadata_open_path(cached_image, uri, &err)) {
        g_error("Exiv2: '%s'", err->message);
        return 1;
    }

    return 0;
}

int
uni_write_exiv2_from_cache(const char *uri)
{
    GError *err;

    gexiv2_log_set_level(GEXIV2_LOG_LEVEL_MUTE);

    if (cached_image == NULL) {
        return 1;
    }

    if (!gexiv2_metadata_save_file(cached_image, uri, &err)) {
        g_error("Exiv2: '%s'", err->message);
    }

    uni_clear_exiv2_cache();

    return 0;
}
