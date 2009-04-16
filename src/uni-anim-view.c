/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; coding: utf-8 -*- 
 *
 * Copyright © 2007-2009 Björn Lindqvist <bjourne@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

/**
 * SECTION:gtkanimview
 * @see_also: #UniImageView, The
 * <filename>./tests/ex-anim.c</filename> program for an example of
 * how this widget is used
 * @short_description: Subclass of #UniImageView capable of playing
 * GIF animations.
 *
 * <para>
 *   UniAnimView subclasses UniImageView. It has the same look and
 *   feel as its parent but is also capable of displaying GIF
 *   animations.
 * </para>
 * <refsect2>
 *   <title>Keybindings</title>
 *   <para>
 *     UniAnimView uses a few more keybindings in addition to those used
 *     by UniImageView:
 *     <table width = "100%" border = "0">
 *       <thead>
 *         <tr>
 *           <th>Keys</th>
 *           <th>Corresponding function</th>
 *           <th>Description</th>
 *         </tr>
 *       </thead>
 *       <tbody>
 *         <tr>
 *           <td>%GDK_p</td>
 *           <td>uni_anim_view_set_is_playing()</td>
 *           <td>Stops or resumes the running animation.</td>
 *         </tr>
 *         <tr>
 *           <td>%GDK_j</td>
 *           <td>uni_anim_view_step()</td>
 *           <td>Steps the animation one frame forward.</td>
 *         </tr>
 *       </tbody>  
 *     </table>  
 *   </para>  
 * </refsect2>
 **/
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


/* static gboolean
uni_anim_view_on_query_tooltip_cb	(GtkWidget  *widget,
									 gint        x,
									 gint        y,
									 gboolean    keyboard_mode,
									 GtkTooltip *tooltip,
									 gpointer    user_data)
{
	gchar * tooltip_string;
	GdkPixbufAnimation *anim = UNI_ANIM_VIEW(widget)->anim;
	
	tooltip_string = g_markup_printf_escaped("<b>Information</b>\n"
											 "%ix%i pixels\n"
											 "%.2f%% zoom",
											 gdk_pixbuf_animation_get_width(anim),
											 gdk_pixbuf_animation_get_height(anim),
											 UNI_IMAGE_VIEW(widget)->zoom*100);
	
	gtk_tooltip_set_markup(tooltip, tooltip_string);
	
	return TRUE;
} */


/*************************************************************/
/***** Stuff that deals with the type ************************/
/*************************************************************/
static void
uni_anim_view_init (UniAnimView * aview)
{
    aview->anim = NULL;
    aview->iter = NULL;
    aview->timer_id = 0;

    /* g_object_set (aview, "has-tooltip", TRUE, NULL);

       g_signal_connect (aview,
       "query-tooltip",
       G_CALLBACK (uni_anim_view_on_query_tooltip_cb),
       NULL); */
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
     * %GDK_p is pressed on the widget and should not be used by
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
     * emitted when %GDK_j is pressed on the widget and should not be
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
    gtk_binding_entry_add_signal (binding_set, GDK_p, 0, "toggle_running", 0);

    /* Step */
    gtk_binding_entry_add_signal (binding_set, GDK_j, 0, "step", 0);
}

/**
 * uni_anim_view_new:
 * @returns: A new #UniAnimView.
 *
 * Creates a new #UniAnimView with default values. The default values
 * are:
 *
 * <itemizedlist>
 *   <listitem>anim : %NULL</listitem>
 *   <listitem>is_playing : %FALSE</listitem>
 * </itemizedlist>
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
 * uni_anim_view_get_anim:
 * @aview: a #UniAnimView.
 * @returns: the current animation 
 *
 * Returns the current animation of the view.
 **/
GdkPixbufAnimation *
uni_anim_view_get_anim (UniAnimView * aview)
{
    return aview->anim;
}

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
void
uni_anim_view_set_anim (UniAnimView * aview, GdkPixbufAnimation * anim)
{
    if (aview->anim)
        g_object_unref (aview->anim);
    aview->anim = anim;
    if (!aview->anim)
    {
        uni_image_view_set_pixbuf (UNI_IMAGE_VIEW (aview), NULL, TRUE);
        return;
    }
    g_object_ref (aview->anim);
    if (aview->iter)
        g_object_unref (aview->iter);

    g_get_current_time (&aview->time);
    aview->iter = gdk_pixbuf_animation_get_iter (aview->anim, &aview->time);

    GdkPixbuf *pixbuf;

    if (gdk_pixbuf_animation_is_static_image (anim))
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
    aview->timer_id = g_timeout_add (aview->delay,
                                     uni_anim_view_updator, aview);
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

/**
 * uni_anim_view_get_is_playing:
 * @aview: A #UniImageView.
 * @returns: %TRUE if an animation is playing, %FALSE otherwise.
 *
 * Returns whether the animation is playing or not. If there is no
 * current animation, this method will always returns %FALSE.
 **/
gboolean
uni_anim_view_get_is_playing (UniAnimView * aview)
{
    return aview->timer_id && aview->anim;
}

/*************************************************************/
/***** Actions ***********************************************/
/*************************************************************/
/**
 * uni_anim_view_step:
 * @aview: A #UniImageView.
 *
 * Steps the animation one frame forward. If the animation is playing
 * it will be stopped. Will it wrap around if the animation is at its
 * last frame?
 **/
void
uni_anim_view_step (UniAnimView * aview)
{
    if (aview->anim)
    {
        // Part of workaround for #437791. uni_anim_view_updator()
        // might not always immidiately step to the next frame, so we
        // loop until the frame is changed.
        //
        // If we are on the last frame, it will not wrap around so the
        // frame will never change. So we risk an infinite loop.
        // Unfortunately but expectedly, GdkPixbufAnimationIter
        // doesn't provide a way to check if we
        // are on the last frame because the API is totally brain
        // damaged. The work-around is to give uni_anim_view_updator
        // exactly 10 chances to advance the frame before bailing out.
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
