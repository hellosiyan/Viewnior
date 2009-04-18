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
#include "vnr-tools.h"

void
fit_to_size (gint * w, gint * h, gint mw, gint mh)
{
    gfloat ratio, mratio;

    /* if size fits well, then exit */
    if (*w < mw && *h < mh)
        return;
    /* check if dividing by 0 */
    if (*w == 0 || mh == 0)
        return;

    ratio = 1. * (*h) / (*w);
    mratio = 1. * mh / mw;

    if (mratio > ratio)
    {
        *w = mw;
        *h = ratio * (*w);
    }
    else if (ratio > mratio)
    {
        *h = mh;
        *w = (*h) / ratio;
    }
    else
    {
        *w = mh;
        *h = mh;
    }

    return;
}

#endif /* __VNR_IMAGE_H__ */
