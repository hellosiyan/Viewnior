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

#ifndef __UNI_EXIV2__H_
#define __UNI_EXIV2__H_ 

#ifdef __cplusplus

#include <exiv2/exiv2.hpp>

extern "C" {
#include <glib/gi18n.h>
    
    typedef Exiv2::ExifData::const_iterator (*ExifDataFinder)(const Exiv2::ExifData& ed);
    typedef struct _ExifDataDictionary ExifDataDictionary;
    typedef struct _IptcDataDictionary IptcDataDictionary;

    struct _ExifDataDictionary {
        const char *key;
        const char *label;
        ExifDataFinder finder;
    };

    struct _IptcDataDictionary {
        const char *key;
        const char *label;
    };

    ExifDataDictionary exifDataDictionary[] = {
        { "Exif.Image.UniqueCameraModel", _("Camera make"), Exiv2::make },
        { "Exif.Image.LocalizedCameraModel", _("Camera model"), Exiv2::model },
        { "Exif.Photo.DateTimeOriginal", _("Date Taken"), NULL },
        { "Exif.Photo.ExposureTime", _("Exposure time"), Exiv2::exposureTime },
        { "Exif.Photo.ExposureMode", _("Exposure mode"), Exiv2::exposureMode },
        { "Exif.Photo.ApertureValue", _("Aperture Value"), NULL },
        { "Exif.Photo.ISOSpeedRatings", _("ISO speed"), Exiv2::isoSpeed },
        { "Exif.Photo.Flash", _("Flash"), NULL },
        { "Exif.Photo.MeteringMode", _("Metering mode"), Exiv2::meteringMode },
        { "Exif.Image.FocalLength", _("Focal length"), Exiv2::focalLength },
        { "Exif.Image.Software", _("Software"), NULL },
        { "Exif.Image.ImageDescription", _("Image description"), NULL },
        { "Exif.Photo.UserComment", _("User comment"), NULL }
    };

    IptcDataDictionary iptcDataDictionary[] = {
        { "Iptc.Application2.Caption", _("Description") },
        { "Iptc.Application2.Copyright", _("Copyright") },
        { "Iptc.Application2.Byline", _("Author") }
    };


#endif /* __cplusplus */

void    uni_read_exiv2_map          (const char *uri, 
                                     void (*callback)(const char*, const char*, void*), 
                                     void *user_data);

int     uni_read_exiv2_to_cache     (const char *uri);
int     uni_write_exiv2_from_cache  (const char *uri);

#ifdef __cplusplus

} /* end extern "C" */

#endif /* __cplusplus */


#endif /* __UNI_EXIV2__H_ */
