/*
 * Copyright © 2009-2018 Siyan Panayotov <siyan.panayotov@gmail.com>
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

#ifndef __CALLBACK_INTERFACE_H__
#define __CALLBACK_INTERFACE_H__

#include <glib.h>

/**
 * A callback function that will be called when a file or directory with
 * a file monitor have reported a change, i.e. a file or directory has
 * been added or removed.
 *
 * @deleted@ is TRUE if a file or directory has been deleted. If it was
 * created, it will be FALSE.
 * @path@ is the path to the file or directory that was affected. The
 * memory will be freed after the call to callback.
 * @changed_node@ is the subtree that was affected. If the file or
 * directory was created, then it has been added to @root@ already. If
 * it was deleted, @changed_node@ has been removed from @root@ and it
 * along with its subnodes have been freed. The pointer may thus point
 * to an invalid structure.
 * @root@ is the root node of the tree that @changed_node@ is/was part
 * of.
 * @data@ is user provided data that will be sent back to the callback
 * function unaltered.
 */
typedef void (*callback)(gboolean deleted,
                         char *path,
                         GNode *changed_node,
                         GNode *root,
                         gpointer data);


struct MonitoringData {
    gboolean include_hidden;
    gboolean include_dirs;
    gboolean set_file_monitor_for_file;
    GNode* tree;
    callback cb;
    gpointer cb_data;
};

#endif /* __CALLBACK_INTERFACE_H__ */
