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
GdkPixbufAnimation* uni_anim_view_get_anim  (UniAnimView * aview);
void        uni_anim_view_set_anim          (UniAnimView * aview,
                                             GdkPixbufAnimation * anim);

void        uni_anim_view_set_is_playing    (UniAnimView * aview,
                                             gboolean playing);

gboolean    uni_anim_view_get_is_playing    (UniAnimView * aview);

/* Actions */
void        uni_anim_view_step      (UniAnimView * aview);

G_END_DECLS
#endif /* __UNI_ANIM_VIEW_H__ */
