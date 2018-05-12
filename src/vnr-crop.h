/*
 * Copyright © 2009-2018 Siyan Panayotov <contact@siyanpanayotov.com>
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

#ifndef __VNR_CROP_H_
#define __VNR_CROP_H_

#include <gtk/gtk.h>
#include "vnr-window.h"

G_BEGIN_DECLS

typedef struct _VnrCrop VnrCrop;
typedef struct _VnrCropClass VnrCropClass;

#define VNR_TYPE_CROP             (vnr_crop_get_type ())
#define VNR_CROP(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), VNR_TYPE_CROP, VnrCrop))
#define VNR_CROP_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass),  VNR_TYPE_CROP, VnrCropClass))
#define VNR_IS_CROP(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VNR_TYPE_CROP))
#define VNR_IS_CROP_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass),  VNR_TYPE_CROP))
#define VNR_CROP_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj),  VNR_TYPE_CROP, VnrCropClass))

struct _VnrCrop {
    GObject parent;

    VnrWindow *vnr_win;

    GdkPixbuf *preview_pixbuf;

    gdouble zoom;
    gdouble width;
    gdouble height;

    GdkGC *gc;
    GtkWidget *image;
    GtkSpinButton *spin_x;
    GtkSpinButton *spin_y;
    GtkSpinButton *spin_width;
    GtkSpinButton *spin_height;

    gdouble sub_x;
    gdouble sub_y;
    gdouble sub_width;
    gdouble sub_height;

    gboolean drawing_rectangle;
    gboolean do_redraw;
    gdouble start_x;
    gdouble start_y;

    GdkRectangle area;
};

struct _VnrCropClass {
    GObjectClass parent_class;
};

GType     vnr_crop_get_type (void) G_GNUC_CONST;

GObject  *vnr_crop_new      (VnrWindow *vnr_win);
gboolean  vnr_crop_run      (VnrCrop *crop);

G_END_DECLS

#endif /* __VNR_CROP_H_ */
