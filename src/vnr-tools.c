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
