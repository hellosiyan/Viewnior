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

typedef void (*UniReadExiv2Callback)(const char *label, const char *value, void *user_data);

void    uni_read_exiv2_map          (const char *uri, 
                                     UniReadExiv2Callback callback,
                                     void *user_data);

int     uni_read_exiv2_to_cache     (const char *uri);
int     uni_write_exiv2_from_cache  (const char *uri);

#endif /* __UNI_EXIV2__H_ */
