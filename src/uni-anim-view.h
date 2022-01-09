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

#ifndef __UNI_ANIM_VIEW_H__
#define __UNI_ANIM_VIEW_H__

#include "uni-image-view.h"

G_BEGIN_DECLS
#define UNI_TYPE_ANIM_VIEW              (uni_anim_view_get_type ())
#define UNI_ANIM_VIEW(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), UNI_TYPE_ANIM_VIEW, UniAnimView))
#define UNI_ANIM_VIEW_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), UNI_TYPE_ANIM_VIEW, UniAnimViewClass))
#define UNI_IS_ANIM_VIEW(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), UNI_TYPE_ANIM_VIEW))
#define UNI_IS_ANIM_VIEW_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), UNI_TYPE_ANIM_VIEW))
#define UNI_ANIM_VIEW_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), UNI_TYPE_ANIM_VIEW, UniAnimViewClass))
typedef struct _UniAnimView UniAnimView;
typedef struct _UniAnimViewClass UniAnimViewClass;

/**
 * UniAnimView:
 *
 * UniAnimView is a subclass of #UniImageView that provies facilities
 * for displaying and controlling an animation.
 **/
struct _UniAnimView {
    UniImageView parent;

    /* The current animation. */
    GdkPixbufAnimation *anim;

    /* The iterator of the current animation. */
    GdkPixbufAnimationIter *iter;

    /* ID of the currently running animation timer. */
    int timer_id;

    /* Timer used to get the right frame. */
    GTimeVal time;
    int delay;

};

struct _UniAnimViewClass {
    UniImageViewClass parent_class;

    /* Keybinding signals. */
    void (*toggle_running) (UniAnimView * aview);
    void (*step) (UniAnimView * aview);
};

GType uni_anim_view_get_type (void) G_GNUC_CONST;

/* Constructors */
GtkWidget *uni_anim_view_new (void);

/* Read-write properties */
gboolean    uni_anim_view_set_anim          (UniAnimView * aview,
                                             GdkPixbufAnimation * anim);

void        uni_anim_view_set_static        (UniAnimView * aview,
                                             GdkPixbuf *anim);

void        uni_anim_view_set_is_playing    (UniAnimView * aview,
                                             gboolean playing);

G_END_DECLS
#endif /* __UNI_ANIM_VIEW_H__ */
