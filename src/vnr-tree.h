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

#ifndef tree_H
#define tree_H

#include <glib.h>
#include "vnr-callback-interface.h"
#include "vnr-file.h"


/**
 * Given the path @uri@, a tree will be created and returned. The path
 * @uri@ may point to a file or a directory. If it is a file, the
 * content of the whole directory that the file is located in will be
 * used to populate the tree.
 *
 * The root node of the returned tree will always be the directory that
 * @uri@ points to (if it is a file, then the directory where that file
 * is placed). If @uri@ is a directory, then the first file in that
 * directory will be returned. However, if @uri@ is a file, then the
 * corresponding node down the child branches of the root node will be
 * the node that is returned from this function. For example, if @uri@
 * is a file called "/tmp/bepa.png", and in the same directory there are
 * also "/tmp/apa.jpg" and "/tmp/cepa.gif", then the structure created
 * would be
 * tmp
 * ├─ apa.jpg
 * ├─ bepa.png
 * └─ cepa.gif
 * Thus, the root would be "/tmp", with three children. However, the
 * node corresponding to "/tmp/bepa.png" would be returned. If there are
 * no files, the root node is returned.
 *
 * File monitors will be set on the directory that @uri@ points to (if
 * it is a file, then the directory of that file will have a file
 * monitor). If a file is removed from the file system, the file monitor
 * will automatically remove the corresponding entry from the returned
 * tree structure. If files are added to the file system, the
 * corresponding entries will automatically be added to the returned
 * tree structure. If @include_dirs@ is TRUE, all nested subdirectories
 * will also have file monitors. When a file monitor is triggered (a
 * file or directory is created or deleted), a call to the callback
 * function @cb@ will be made. To it, @cb_data@ will be sent.
 * In the example above, there is a file monitor on "/tmp", so if files
 * are added to /tmp in the file system, they are also automatically
 * added to the tree structure.
 *
 * Setting @include_hidden@ to TRUE will include hidden files and
 * directories.
 * Setting @include_dirs@ to TRUE will recursively go down all
 * subdirectories, include them as well and set file monitors on them.
 * @error@ will contain any errors that occurred in the process.
 */
GNode* create_tree_from_single_uri(char *uri,
                                   gboolean include_hidden,
                                   gboolean recursive,
                                   callback cb,
                                   gpointer cb_data,
                                   GError **error);


/**
 * Given a list of paths @uri_list@, a tree will be created and
 * returned. The paths in @uri_list@ may point to files or directories.
 *
 * The root node of the returned tree will not contain a file or
 * directory itself, but instead act as a parent to the children beneath
 * it, i.e. the content of @uri_list@. The files and directories in
 * @uri_list@ do not have to be in the same directory. For example, if
 * @uri_list@ consists of "/tmp/subdir", "/tmp/bepa.png", and
 * "/tmp/somedir/apa.jpg", then the structure created will be
 * <ROOT>
 * ├─ apa.jpg
 * ├─ bepa.png
 * └─ /subdir
 * Thus, the root would contain the three children in @uri_list@, but
 * the root would not contain a file or directory itself. Despite the
 * files "apa.jpg" and "bepa.png" not being in the same directory, they
 * are placed as siblings in the tree.
 *
 * The first node containing a file in the tree structure will be
 * returned. Note that this is not neccessarily the first node in
 * @uri_list@, since the tree structure is alphabetically sorted by name
 * (and directories are placed after files). In the case above, the
 * returned node would be the one containing "apa.jpg". If there are no
 * files, then the root node is returned.
 *
 * File monitors will be set on all the files and directories in
 * @uri_list@. If a file is removed from the file system, the file
 * monitor will automatically remove the corresponding entry from the
 * returned tree structure. If files are added to the file system, the
 * corresponding entries will automatically be added to the returned
 * tree structure. If @include_dirs@ is TRUE, all nested subdirectories
 * will also have file monitors. When a file monitor is triggered (a
 * file or directory is created or deleted), a call to the callback
 * function @cb@ will be made. To it, @cb_data@ will be sent.
 * In the example above, the files "apa.jpg", "bepa.png" and "/subdir"
 * will have file monitors. Thus, removing "bepa.png" from the file
 * system will remove it from the tree structure as well. Adding a file
 * "/tmp/cepa.gif" will do nothing to the tree, since "/tmp" has no file
 * monitor. However, Adding a file "/tmp/subdir/cepa.gif" will add it to
 * the tree as well, since "/subdir" does have a file monitor.
 *
 * Setting @include_hidden@ to TRUE will include hidden files and
 * directories.
 * Setting @include_dirs@ to TRUE will recursively go down all
 * subdirectories, include them as well and set file monitors on them.
 * @error@ will contain any errors that occurred in the process.
 */
GNode* create_tree_from_uri_list(GSList *uri_list,
                                 gboolean include_hidden,
                                 gboolean recursive,
                                 callback cb,
                                 gpointer cb_data,
                                 GError **error);


/**
 * Adds @node@ as a child of @tree@, sorted by @display_name_collate@.
 * @tree@ must be a directory, not a file; @node@ may be a file or a
 * directory. When sorting, files will be inserted before directories.
 * If @node@ is already present, @tree@ will remain unchanged.
 */
void add_node_in_tree(GNode *tree, GNode *node);


/**
 * Returns the next file (i.e. not directory) in the given @tree@.
 * Will climb up the structure relative to @tree@ or "wrap around" if
 * needed. If there is no such file, @tree@ will be returned.
 */
GNode* get_next_in_tree(GNode *tree);

/**
 * Returns the previous file (i.e. not directory) in the given @tree@.
 * Will climb up the structure relative to @tree@ or "wrap around" if
 * needed. If there is no such file, @tree@ will be returned.
 */
GNode* get_prev_in_tree(GNode *tree);

/**
 * Returns the first file (i.e. not directory) in the given @tree@.
 * Will climb up the structure relative to @tree@ if needed.
 * If there is no such file, @tree@ will be returned.
 */
GNode* get_first_in_tree(GNode* tree);

/**
 * Returns the last file (i.e. not directory) in the given @tree@.
 * Will climb up the structure relative to @tree@ if needed.
 * If there is no such file, @tree@ will be returned.
 */
GNode* get_last_in_tree(GNode* tree);

/**
 * Will move to the topmost root of @tree@, traverse the whole structure
 * and return the node (file or directory) whose path is equal to
 * @path@. If no such node exists in the structure, NULL is returned.
 */
GNode* get_child_in_directory(GNode *tree, char* path);



/**
 * This function will move to the root from @tree@, and go through the
 * whole structure. All files (i.e. not directories) will be counted,
 * and that number will be placed in @total@. The position of @tree@ in
 * the structure will be placed in @tree_position@.
 */
void get_leaf_position(GNode *tree, int *tree_position, int *total);

/**
 * This function will move to the root from @tree@, and go through the
 * whole structure. All files (i.e. not directories) will be counted,
 * and that number will be returned.
 */
int get_total_number_of_leaves(GNode *tree);



/**
 * Returns whether or not the given @tree@ has children
 * (files or directories).
 */
gboolean has_children(GNode *tree);

/**
 * Returns whether or not the given @tree@ has any more siblings (files
 * or directories). If a @tree@ has n children, then giving any of the
 * first n-1 children of @tree@ as input to this function would return
 * TRUE. For child n, this function would return FALSE as there are no
 * more children after it.
 */
gboolean has_more_siblings(GNode *tree);


/**
 * Returns the topmost root of @tree@.
 */
GNode* get_root_node(GNode *tree);


/**
 * Frees @tree@. If it is a sub-tree, the rest of the tree will be left
 * alone. Traverses the whole of @tree@ and destroys the nodes as well.
 */
void free_current_tree(GNode *tree);

/**
 * Moves to the topmost root of @tree@ and frees the whole structure.
 * Traverses the whole tree and destroys the nodes as well.
 */
void free_whole_tree(GNode *tree);

#endif // tree_H
