/*
 * Copyright © 2009-2018 Siyan Panayotov <contact@siyanpanayotov.com>
 *
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

#include <glib.h>
#include <gdk/gdkkeysyms.h>
#include "uni-anim-view.h"

/*************************************************************/
/***** Private data ******************************************/
/*************************************************************/
enum {
    TOGGLE_RUNNING,
    STEP,
    LAST_SIGNAL
};

static guint uni_anim_view_signals[LAST_SIGNAL] = { 0 };

G_DEFINE_TYPE (UniAnimView, uni_anim_view, UNI_TYPE_IMAGE_VIEW);

/*************************************************************/
/***** Static stuff ******************************************/
/*************************************************************/

static gboolean
uni_anim_view_updator (gpointer data)
{
    UniAnimView *aview = (UniAnimView *) data;

    // Workaround for #437791.
    glong delay_us = aview->delay * 1000;
    if (aview->delay == 20)
    {
        // If the delay time is 20 ms, the GIF is a "fast player." and
        // we increase it to a more reasonable 100 ms so that the
        // frame is only updated 1/5 of the times
        // uni_anim_view_updator() is run.
        delay_us = 200;
    }
    g_time_val_add (&aview->time, delay_us);

    gboolean next = gdk_pixbuf_animation_iter_advance (aview->iter,
                                                       &aview->time);
    uni_anim_view_set_is_playing (aview, FALSE);

    aview->delay = gdk_pixbuf_animation_iter_get_delay_time (aview->iter);
    aview->timer_id = g_timeout_add (aview->delay,
                                     uni_anim_view_updator, aview);

    if (!next)
        return FALSE;

    GdkPixbuf *pixbuf = gdk_pixbuf_animation_iter_get_pixbuf (aview->iter);
    uni_image_view_set_pixbuf (UNI_IMAGE_VIEW (aview), pixbuf, FALSE);

    return FALSE;
}

/*************************************************************/
/***** Private signal handlers *******************************/
/*************************************************************/
static void
uni_anim_view_toggle_running (UniAnimView * aview)
{
    uni_anim_view_set_is_playing (aview, !aview->timer_id);
}

/* Steps the animation one frame forward. If the animation is playing
 * it will be stopped. Will it wrap around if the animation is at its
 * last frame?
 **/
static void
uni_anim_view_step (UniAnimView * aview)
{
    if (aview->anim)
    {
        /* Part of workaround for #437791. uni_anim_view_updator()
         * might not always immidiately step to the next frame, so we
         * loop until the frame is changed.
         *
         * If we are on the last frame, it will not wrap around so the
         * frame will never change. So we risk an infinite loop.
         * Unfortunately but expectedly, GdkPixbufAnimationIter
         * doesn't provide a way to check if we
         * are on the last frame because the API is totally brain
         * damaged. The work-around is to give uni_anim_view_updator
         * exactly 10 chances to advance the frame before bailing out.
         * */
        int n = 0;
        GdkPixbuf *old = gdk_pixbuf_animation_iter_get_pixbuf (aview->iter);
        while ((gdk_pixbuf_animation_iter_get_pixbuf (aview->iter) == old)
               && (n < 10))
        {
            uni_anim_view_updator (aview);
            n++;
        }
    }
    uni_anim_view_set_is_playing (aview, FALSE);
}

/*************************************************************/
/***** Stuff that deals with the type ************************/
/*************************************************************/
static void
uni_anim_view_init (UniAnimView * aview)
{
    aview->anim = NULL;
    aview->iter = NULL;
    aview->timer_id = 0;
}

static void
uni_anim_view_finalize (GObject * object)
{
    uni_anim_view_set_is_playing (UNI_ANIM_VIEW (object), FALSE);

    /* Chain up. */
    G_OBJECT_CLASS (uni_anim_view_parent_class)->finalize (object);
}

static void
uni_anim_view_init_signals (UniAnimViewClass * klass)
{
    /**
     * UniAnimView::toggle-running:
     * @aview: a #UniAnimView
     *
     * Stops the animation if it was playing or resumes it, if it was
     * playing. ::toggle-running is a keybinding signal emitted when
     * %GDK_KEY_p is pressed on the widget and should not be used by
     * clients of this library.
     **/
    uni_anim_view_signals[TOGGLE_RUNNING] =
        g_signal_new ("toggle_running",
                      G_TYPE_FROM_CLASS (klass),
                      G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                      G_STRUCT_OFFSET (UniAnimViewClass, toggle_running),
                      NULL, NULL,
                      g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
    /**
     * UniAnimView::step:
     * @aview: a #UniAnimView
     *
     * Steps the animation one frame forward. If the animation is
     * playing it will first be stopped. ::step is a keybinding signal
     * emitted when %GDK_KEY_j is pressed on the widget and should not be
     * used by clients of this library.
     **/
    uni_anim_view_signals[STEP] =
        g_signal_new ("step",
                      G_TYPE_FROM_CLASS (klass),
                      G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                      G_STRUCT_OFFSET (UniAnimViewClass, step),
                      NULL, NULL,
                      g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
}

static void
uni_anim_view_class_init (UniAnimViewClass * klass)
{
    uni_anim_view_init_signals (klass);

    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    object_class->finalize = uni_anim_view_finalize;

    klass->toggle_running = uni_anim_view_toggle_running;
    klass->step = uni_anim_view_step;

    /* Add keybindings. */
    GtkBindingSet *binding_set = gtk_binding_set_by_class (klass);

    /* Stop */
    gtk_binding_entry_add_signal (binding_set, GDK_KEY_p, 0, "toggle_running", 0);

    /* Step */
    gtk_binding_entry_add_signal (binding_set, GDK_KEY_j, 0, "step", 0);
}

/**
 * uni_anim_view_new:
 * @returns: A new #UniAnimView.
 *
 * Creates a new #UniAnimView with default values.
 **/
GtkWidget *
uni_anim_view_new (void)
{
    GtkWidget *aview = g_object_new (UNI_TYPE_ANIM_VIEW, NULL);
    return aview;
}


/*************************************************************/
/***** Read-write properties *********************************/
/*************************************************************/
/**
 * uni_anim_view_set_anim:
 * @aview: A #UniAnimView.
 * @anim: A pixbuf animation to play.
 *
 * Sets the pixbuf animation to play, or %NULL to not play any
 * animation.
 *
 * If the animation is a static image or only has one frame, then the
 * static image will be displayed instead. If more frames are loaded
 * into the animation, then #UniAnimView will automatically animate to
 * those frames.
 *
 * The effect of this method is analoguous to
 * uni_image_view_set_pixbuf(). Fit mode is reset to
 * %GTK_FIT_SIZE_IF_LARGER so that the whole area of the animation
 * fits in the view. Three signals are emitted, first the
 * #UniImageView will emit ::zoom-changed and then ::pixbuf-changed,
 * second, #UniAnimView itself will emit ::anim-changed.
 *
 * The default pixbuf animation is %NULL.
 **/

/* Return TRUE if anim is a static image */
gboolean
uni_anim_view_set_anim (UniAnimView * aview, GdkPixbufAnimation * anim)
{
    gboolean is_static;

    if (aview->anim)
        g_object_unref (aview->anim);
    aview->anim = anim;

    if (!anim)
    {
        uni_anim_view_set_is_playing (aview, FALSE);
        uni_image_view_set_pixbuf (UNI_IMAGE_VIEW (aview), NULL, TRUE);
        return TRUE;
    }

    g_object_ref (aview->anim);
    if (aview->iter)
        g_object_unref (aview->iter);

    g_get_current_time (&aview->time);
    aview->iter = gdk_pixbuf_animation_get_iter (aview->anim, &aview->time);

    GdkPixbuf *pixbuf;

    is_static = gdk_pixbuf_animation_is_static_image (anim);

    if (is_static)
    {
        pixbuf = gdk_pixbuf_animation_get_static_image (anim);
    }
    else
    {
        pixbuf = gdk_pixbuf_animation_iter_get_pixbuf (aview->iter);
    }

    uni_image_view_set_pixbuf (UNI_IMAGE_VIEW (aview), pixbuf, TRUE);

    uni_anim_view_set_is_playing (aview, FALSE);
    aview->delay = gdk_pixbuf_animation_iter_get_delay_time (aview->iter);

    if(!is_static)
        aview->timer_id = g_timeout_add (aview->delay,
                                         uni_anim_view_updator, aview);
    return is_static;
}


/* No conversion from GdkPixbuf to GdkPixbufAnim can be made
 * directly using the current API, so this makes a static
 * GdkPixbufAnimation and updates the UniAnimView */
void
uni_anim_view_set_static (UniAnimView * aview, GdkPixbuf * pixbuf)
{
    GdkPixbufSimpleAnim *s_anim;

    s_anim = gdk_pixbuf_simple_anim_new (gdk_pixbuf_get_width(pixbuf),
                                         gdk_pixbuf_get_height(pixbuf),
                                         -1);
    gdk_pixbuf_simple_anim_add_frame(s_anim, pixbuf);

    /* Simple version of uni_anim_view_set_anim */
    if (aview->anim)
        g_object_unref (aview->anim);

    aview->anim = (GdkPixbufAnimation*)s_anim;

    g_object_ref (aview->anim);
    if (aview->iter)
        g_object_unref (aview->iter);

    uni_image_view_set_pixbuf (UNI_IMAGE_VIEW (aview), pixbuf, TRUE);
    uni_anim_view_set_is_playing (aview, FALSE);
    aview->delay = -1;
    aview->iter = NULL;

    g_object_unref(pixbuf);
}


/**
 * uni_anim_view_set_is_playing:
 * @aview: a #UniImageView
 * @playing: %TRUE to play the animation, %FALSE otherwise
 *
 * Sets whether the animation should play or not. If there is no
 * current animation this method does not have any effect.
 **/
void
uni_anim_view_set_is_playing (UniAnimView * aview, gboolean playing)
{
    if (!playing && aview->timer_id)
    {
        /* Commanded to stop AND the animation is playing. */
        g_source_remove (aview->timer_id);
        aview->timer_id = 0;
    }
    else if (playing && aview->anim)
        uni_anim_view_updator (aview);
}
