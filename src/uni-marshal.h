
#ifndef __uni_marshal_MARSHAL_H__
#define __uni_marshal_MARSHAL_H__

#include	<glib-object.h>

G_BEGIN_DECLS

/* VOID:ENUM,ENUM (uni-marshal.list:1) */
extern void uni_marshal_VOID__ENUM_ENUM (GClosure     *closure,
                                         GValue       *return_value,
                                         guint         n_param_values,
                                         const GValue *param_values,
                                         gpointer      invocation_hint,
                                         gpointer      marshal_data);

/* VOID:POINTER,POINTER (uni-marshal.list:2) */
extern void uni_marshal_VOID__POINTER_POINTER (GClosure     *closure,
                                               GValue       *return_value,
                                               guint         n_param_values,
                                               const GValue *param_values,
                                               gpointer      invocation_hint,
                                               gpointer      marshal_data);

G_END_DECLS

#endif /* __uni_marshal_MARSHAL_H__ */

