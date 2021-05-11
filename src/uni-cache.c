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

#include "uni-cache.h"
#include "uni-utils.h"
#include <string.h>

static gboolean
uni_rectangle_contains_rect (GdkRectangle r1, GdkRectangle r2)
{
    return
        r1.x <= r2.x &&
        r1.y <= r2.y &&
        (r2.x + r2.width) <= (r1.x + r1.width) &&
        (r2.y + r2.height) <= (r1.y + r1.height);
}

static void
uni_pixbuf_copy_area_intact (GdkPixbuf * src,
                             int src_x,
                             int src_y,
                             int width,
                             int height,
                             GdkPixbuf * dst, int dst_x, int dst_y)
{
    int y;
    if (src_x == dst_x && src_y == dst_y && src == dst)
        return;

    int src_stride = gdk_pixbuf_get_rowstride (src);
    int dst_stride = gdk_pixbuf_get_rowstride (dst);
    int chans = gdk_pixbuf_get_n_channels (src);

    int linelen = width * chans;

    guchar *src_base = gdk_pixbuf_get_pixels (src);
    guchar *dst_base = gdk_pixbuf_get_pixels (dst);

    int src_y_ofs = src_y * src_stride;
    int dst_y_ofs = dst_y * dst_stride;
    if (dst_y > src_y)
    {
        src_y_ofs = (src_y + height - 1) * src_stride;
        dst_y_ofs = (dst_y + height - 1) * dst_stride;
        src_stride = -src_stride;
        dst_stride = -dst_stride;
    }
    guchar *src_ofs = src_base + src_y_ofs + src_x * chans;
    guchar *dst_ofs = dst_base + dst_y_ofs + dst_x * chans;

    void (*copy_func) (void *, void *, size_t) = (void *) memcpy;
    if (dst_x > src_x)
        copy_func = (void *) memmove;

    for (y = 0; y < height; y++)
    {
        copy_func (dst_ofs, src_ofs, linelen);
        src_ofs += src_stride;
        dst_ofs += dst_stride;
    }
}

/**
 * uni_pixbuf_draw_cache_get_method:
 * @old: the last draw options used
 * @new_: the current draw options
 * @returns: the best draw method to use to draw
 *
 * Gets the fastest method to draw the specified draw options.
 * @last_opts is assumed to be the last #PixbufDrawOpts used and
 * @new_opts is the one to use this time.
 **/
UniPixbufDrawMethod
uni_pixbuf_draw_cache_get_method (UniPixbufDrawOpts * old,
                                  UniPixbufDrawOpts * new_)
{
    if (new_->zoom != old->zoom ||
        new_->interp != old->interp || new_->pixbuf != old->pixbuf)
    {
        return UNI_PIXBUF_DRAW_METHOD_SCALE;
    }
    else if (uni_rectangle_contains_rect (old->zoom_rect, new_->zoom_rect))
    {
        return UNI_PIXBUF_DRAW_METHOD_CONTAINS;
    }
    else
    {
        return UNI_PIXBUF_DRAW_METHOD_SCROLL;
    }
}

/**
 * uni_pixbuf_draw_cache_new:
 * @returns: a new #UniPixbufDrawCache
 *
 * Creates a new pixbuf draw cache.
 **/
UniPixbufDrawCache *
uni_pixbuf_draw_cache_new ()
{
    UniPixbufDrawCache *cache = g_new0 (UniPixbufDrawCache, 1);
    cache->last_pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, 1, 1);
    cache->check_size = 16;
    cache->old = (UniPixbufDrawOpts)
    {
        0,
        {
        0, 0, 0, 0}
    , 0, 0, GDK_INTERP_NEAREST, cache->last_pixbuf};
    return cache;
}

/**
 * uni_pixbuf_draw_cache_free:
 * @cache: a #UniPixbufDrawCache
 *
 * Deallocates a pixbuf draw cache and all its data.
 **/
void
uni_pixbuf_draw_cache_free (UniPixbufDrawCache * cache)
{
    g_object_unref (cache->last_pixbuf);
    g_free (cache);
}

/**
 * uni_pixbuf_draw_cache_invalidate:
 * @cache: a #UniPixbufDrawCache
 *
 * Force the pixbuf draw cache to scale the pixbuf at the next draw.
 *
 * UniPixbufDrawCache tries to minimize the number of scale operations
 * needed by caching the last drawn pixbuf. It would be inefficient to
 * check the individual pixels inside the pixbuf so it assumes that if
 * the memory address of the pixbuf has not changed, then the cache is
 * good to use.
 *
 * However, when the image data is modified, this assumtion breaks,
 * which is why this method must be used to tell draw cache about it.
 **/
void
uni_pixbuf_draw_cache_invalidate (UniPixbufDrawCache * cache)
{
    /* Set the cached zoom to a bogus value, to force a
       DRAW_FLAGS_SCALE. */
    cache->old.zoom = -1234.0;
}

static GdkPixbuf *
uni_pixbuf_draw_cache_scroll_intersection (GdkPixbuf * pixbuf,
                                           int new_width,
                                           int new_height,
                                           int src_x,
                                           int src_y,
                                           int inter_width,
                                           int inter_height,
                                           int dst_x, int dst_y)
{
    int last_width = gdk_pixbuf_get_width (pixbuf);
    int last_height = gdk_pixbuf_get_height (pixbuf);

    int width = MAX (last_width, new_width);
    int height = MAX (last_height, new_height);
    if (width > last_width || height > last_height)
    {
        GdkColorspace cs = gdk_pixbuf_get_colorspace (pixbuf);
        int bps = gdk_pixbuf_get_bits_per_sample (pixbuf);
        gboolean alpha = gdk_pixbuf_get_has_alpha (pixbuf);
        GdkPixbuf *tmp = gdk_pixbuf_new (cs, alpha, bps, width, height);

        uni_pixbuf_copy_area_intact (pixbuf,
                                     src_x, src_y,
                                     inter_width, inter_height,
                                     tmp, dst_x, dst_y);
        g_object_unref (pixbuf);
        return tmp;
    }
    uni_pixbuf_copy_area_intact (pixbuf,
                                 src_x, src_y,
                                 inter_width, inter_height,
                                 pixbuf, dst_x, dst_y);
    return pixbuf;
}

/**
 * uni_pixbuf_draw_cache_intersect_draw:
 *
 * Updates the cache by first scrolling the still valid area in the
 * cache. Then the newly exposed areas in the cache is sampled from
 * the pixbuf.
 **/
static void
uni_pixbuf_draw_cache_intersect_draw (UniPixbufDrawCache * cache,
                                      UniPixbufDrawOpts * opts)
{
    GdkRectangle this = opts->zoom_rect;
    GdkRectangle old_rect = cache->old.zoom_rect;
    int n;

    /* If there is no intersection, we have to scale the whole area
       from the source pixbuf. */
    GdkRectangle inter = {0, 0, 0, 0};
    GdkRectangle around[4] = {
        this,
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    };
    if (gdk_rectangle_intersect (&old_rect, &this, &inter))
        uni_rectangle_get_rects_around (&this, &inter, around);

    cache->last_pixbuf =
        uni_pixbuf_draw_cache_scroll_intersection (cache->last_pixbuf,
                                                   this.width,
                                                   this.height,
                                                   inter.x - old_rect.x,
                                                   inter.y - old_rect.y,
                                                   inter.width,
                                                   inter.height,
                                                   around[1].width,
                                                   around[0].height);

    for (n = 0; n < 4; n++)
    {
        if (!around[n].width || !around[n].height)
            continue;
        uni_pixbuf_scale_blend (opts->pixbuf,
                                cache->last_pixbuf,
                                around[n].x - this.x,
                                around[n].y - this.y,
                                around[n].width, around[n].height,
                                -this.x, -this.y,
                                opts->zoom,
                                opts->interp, around[n].x, around[n].y);
    }
}

/**
 * uni_pixbuf_draw_cache_draw:
 * @cache: a #UniPixbufDrawCache
 * @opts: the #UniPixbufDrawOpts to use in this draw
 * @window: a #GdkWindow to draw on
 *
 * Redraws the area specified in the pixbuf draw options in an
 * efficient way by using caching.
 **/
void
uni_pixbuf_draw_cache_draw (UniPixbufDrawCache * cache,
                            UniPixbufDrawOpts * opts, GdkWindow * window)
{
    GdkRectangle this = opts->zoom_rect;
    UniPixbufDrawMethod method =
        uni_pixbuf_draw_cache_get_method (&cache->old, opts);
    int deltax = 0;
    int deltay = 0;
    if (method == UNI_PIXBUF_DRAW_METHOD_CONTAINS)
    {
        deltax = this.x - cache->old.zoom_rect.x;
        deltay = this.y - cache->old.zoom_rect.y;
    }
    else if (method == UNI_PIXBUF_DRAW_METHOD_SCROLL)
    {
        uni_pixbuf_draw_cache_intersect_draw (cache, opts);
    }
    else if (method == UNI_PIXBUF_DRAW_METHOD_SCALE)
    {
        int last_width = gdk_pixbuf_get_width (cache->last_pixbuf);
        int last_height = gdk_pixbuf_get_height (cache->last_pixbuf);
        GdkColorspace new_cs = gdk_pixbuf_get_colorspace (opts->pixbuf);
        GdkColorspace last_cs =
            gdk_pixbuf_get_colorspace (cache->last_pixbuf);
        int new_bps = gdk_pixbuf_get_bits_per_sample (opts->pixbuf);
        int last_bps = gdk_pixbuf_get_bits_per_sample (cache->last_pixbuf);

        if (this.width > last_width || this.height > last_height ||
            new_cs != last_cs || new_bps != last_bps)
        {
            g_object_unref (cache->last_pixbuf);
            cache->last_pixbuf = gdk_pixbuf_new (new_cs, FALSE, new_bps,
                                                 this.width, this.height);
        }

        uni_pixbuf_scale_blend (opts->pixbuf,
                                cache->last_pixbuf,
                                0, 0,
                                this.width, this.height,
                                (double) -this.x, (double) -this.y,
                                opts->zoom, opts->interp, this.x, this.y);
    }
    gdk_draw_pixbuf (window,
                     NULL,
                     cache->last_pixbuf,
                     deltax, deltay,
                     opts->widget_x, opts->widget_y,
                     this.width, this.height,
                     GDK_RGB_DITHER_MAX, opts->widget_x, opts->widget_y);
    if (method != UNI_PIXBUF_DRAW_METHOD_CONTAINS)
        cache->old = *opts;
}
