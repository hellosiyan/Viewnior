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

#include <glib/gi18n.h>
#include <stdlib.h>

#include <gtk/gtk.h>

#include "vnr-tree.h"

#define UNUSED(x) (void)(x)

typedef enum {RIGHT, LEFT} Direction;
typedef enum {CONTINUE, RETREAT} Course;


struct Preference_Settings {
    gboolean include_hidden;
    gboolean include_dirs;
    gboolean set_file_monitor_for_file;
    callback cb;
    gpointer cb_data;
};



static GNode*
vnr_file_dir_content_to_list(VnrFile  *vnrfile,
                             struct Preference_Settings* preference_settings,
                             GError   **error);

static gboolean
vnr_file_get_file_info(char *filepath,
                       VnrFile **vnrfile,
                       gboolean include_hidden,
                       GError **error);

static void
vnr_file_set_file_monitor(GNode* tree, struct Preference_Settings* preference_settings);


static void
add_file_list_to_tree(GNode **tree, GList **file_list, struct Preference_Settings *preference_settings);

static void
add_directory_list_to_tree(GNode **tree, GList **dir_list, struct Preference_Settings *preference_settings, GError **error);

static gboolean
tree_contains_path(GNode *tree, char *path);

GList * supported_mime_types;



gint compare_quarks (gconstpointer a, gconstpointer b) {
    GQuark quark = g_quark_from_string ((const gchar *) a);
    return quark - GPOINTER_TO_INT (b);
}


static gint vnr_file_list_compare(gconstpointer a, gconstpointer b) {
    return g_strcmp0(VNR_FILE(a)->display_name_collate,
                     VNR_FILE(b)->display_name_collate);
}



/* Modified version of eog's eog_image_get_supported_mime_types */
static GList * vnr_file_get_supported_mime_types(void) {
    GSList *format_list, *it;
    gchar **mime_types;
    int i;

    if(!supported_mime_types) {
        format_list = gdk_pixbuf_get_formats();

        for(it = format_list; it != NULL; it = it->next) {
            mime_types = gdk_pixbuf_format_get_mime_types((GdkPixbufFormat *) it->data);

            for(i = 0; mime_types[i] != NULL; i++) {
                supported_mime_types = g_list_prepend(supported_mime_types, g_strdup(mime_types[i]));
            }

            g_strfreev(mime_types);
        }

        supported_mime_types = g_list_prepend(supported_mime_types, "image/vnd.microsoft.icon");
        supported_mime_types = g_list_sort(supported_mime_types, (GCompareFunc) compare_quarks);

        g_slist_free(format_list);
    }

    return supported_mime_types;
}

static gboolean vnr_file_is_supported_mime_type(const char *mime_type) {
    GList *result;

    GQuark quark = g_quark_from_string(mime_type);
    supported_mime_types = vnr_file_get_supported_mime_types();

    result = g_list_find_custom(supported_mime_types,
                                GINT_TO_POINTER (quark),
                                (GCompareFunc) compare_quarks);

    return result != NULL;
}


static struct Preference_Settings* create_preference_settings(gboolean include_hidden,
                                                              gboolean include_dirs,
                                                              gboolean set_file_monitor_for_file,
                                                              callback cb,
                                                              gpointer cb_data) {

    struct Preference_Settings* preference_settings = malloc(sizeof(*preference_settings));
    preference_settings->include_hidden = include_hidden;
    preference_settings->include_dirs = include_dirs;
    preference_settings->set_file_monitor_for_file = set_file_monitor_for_file;
    preference_settings->cb = cb;
    preference_settings->cb_data = cb_data;
    return preference_settings;
}


static void remove_file_from_tree(struct MonitoringData *monitoring_data, GFile *file) {

    GNode* tree = monitoring_data->tree;
    callback tree_changed_callback = monitoring_data->cb;
    gpointer cb_data = monitoring_data->cb_data;

    GNode *root = get_root_node(tree);

    char *file_path = g_file_get_path(file);
    GNode* child = get_child_in_directory(tree, file_path);

    if(child != NULL) {
        g_node_unlink(child);
        free_current_tree(child);
    }

    if(tree_changed_callback != NULL) {
        tree_changed_callback(TRUE, file_path, child, root, cb_data);
    }

    g_free(file_path);
}

static void add_file_to_tree(struct MonitoringData *monitoring_data, GFile *file) {

    VnrFile* vnrfile_new = NULL;

    GNode* tree = monitoring_data->tree;
    gboolean include_hidden = monitoring_data->include_hidden;
    gboolean include_dirs = monitoring_data->include_dirs;
    gboolean set_file_monitor_for_file = monitoring_data->set_file_monitor_for_file;
    callback tree_changed_callback = monitoring_data->cb;
    gpointer cb_data = monitoring_data->cb_data;

    GNode *root = get_root_node(tree);
    char *file_path = g_file_get_path(file);

    if(!tree_contains_path(tree, file_path)) {

        vnr_file_get_file_info(file_path,
                               &vnrfile_new,
                               include_hidden,
                               NULL);

        gboolean file_added_to_tree = FALSE;
        GNode *newnode = NULL;
        if(vnr_file_is_directory(vnrfile_new)) {
            if(include_dirs) {
                // Newly created directory. It might already have been populated.

                struct Preference_Settings* preference_settings = create_preference_settings(include_hidden,
                                                                                             include_dirs,
                                                                                             set_file_monitor_for_file,
                                                                                             tree_changed_callback,
                                                                                             cb_data);

                newnode = vnr_file_dir_content_to_list(vnrfile_new, preference_settings, NULL);
                add_node_in_tree(tree, newnode);
                vnr_file_set_file_monitor(newnode, preference_settings);

                file_added_to_tree = TRUE;
                free(preference_settings);
            }

        } else if(vnr_file_is_image_file(vnrfile_new)) {
            newnode = g_node_new(vnrfile_new);
            add_node_in_tree(tree, newnode);
            file_added_to_tree = TRUE;
        }

        if(file_added_to_tree && tree_changed_callback != NULL) {
            tree_changed_callback(FALSE, file_path, newnode, root, cb_data);
        }

        if(!file_added_to_tree) {
            vnr_file_destroy_data(vnrfile_new);
        }
    }
    g_free(file_path);
}


static void
vnr_file_directory_updated(GFileMonitor       *monitor,
                           GFile              *file,
                           GFile              *other_file,
                           GFileMonitorEvent   type,
                           gpointer            data)
{
    UNUSED(monitor);
    UNUSED(other_file);

    VnrFile* vnrfile = data;

    switch (type) {
        case G_FILE_MONITOR_EVENT_DELETED:

            remove_file_from_tree(vnrfile->monitoring_data, file);
            break;

        case G_FILE_MONITOR_EVENT_CHANGED: // Fall-through
        case G_FILE_MONITOR_EVENT_CREATED:

            add_file_to_tree(vnrfile->monitoring_data, file);
            break;

        default:
            break;
    }
}



static void
vnr_file_set_file_monitor(GNode* tree, struct Preference_Settings* preference_settings)
{
    VnrFile* vnrfile = tree->data;
    GFile *file = g_file_new_for_path(vnrfile->path);
    // It's not fatal if directory monitoring isn't supported,
    // so set error to NULL.
    vnrfile->monitor = g_file_monitor(file,
                                      G_FILE_MONITOR_NONE,
                                      NULL,
                                      NULL);
    g_object_unref(file);

    if(vnrfile->monitor) {

        // This will be freed when the VnrFile is destroyed.
        struct MonitoringData* monitoring_data = malloc(sizeof(*monitoring_data));

        monitoring_data->tree = tree;
        monitoring_data->include_hidden = preference_settings->include_hidden;
        monitoring_data->include_dirs = preference_settings->include_dirs;
        monitoring_data->set_file_monitor_for_file = preference_settings->set_file_monitor_for_file;
        monitoring_data->cb = preference_settings->cb;
        monitoring_data->cb_data = preference_settings->cb_data;

        vnrfile->monitoring_data = monitoring_data;

        g_signal_connect(vnrfile->monitor,
                         "changed",
                         G_CALLBACK(vnr_file_directory_updated),
                         vnrfile);
    }
}



static gboolean
vnr_file_get_file_info(char *filepath,
                       VnrFile **vnrfile,
                       gboolean include_hidden,
                       GError **error)
{
    if(filepath == NULL) {
        return FALSE;
    }
    GFile *file;
    GFileInfo *fileinfo;
    const char *mimetype;
    char *display_name;
    char *full_filepath;
    gboolean file_info_success;
    gboolean is_directory;
    gboolean supported_mime_type = FALSE;

    *vnrfile = NULL;
    file = g_file_new_for_path(filepath);
    fileinfo = g_file_query_info(file,
                                 G_FILE_ATTRIBUTE_STANDARD_TYPE","
                                 G_FILE_ATTRIBUTE_STANDARD_DISPLAY_NAME","
                                 G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE","
                                 G_FILE_ATTRIBUTE_STANDARD_IS_HIDDEN,
                                 (GFileQueryInfoFlags) 0, NULL, error);
    file_info_success = fileinfo != NULL;

    if(file_info_success && (include_hidden || !g_file_info_get_is_hidden(fileinfo))) {
        is_directory = g_file_info_get_file_type(fileinfo) == G_FILE_TYPE_DIRECTORY;
        display_name = g_strdup(g_file_info_get_display_name(fileinfo));

        if(!is_directory) {
            mimetype = g_file_info_get_content_type(fileinfo);
            supported_mime_type = vnr_file_is_supported_mime_type(mimetype);
        }

        if(is_directory || supported_mime_type) {
            full_filepath = g_file_get_path(file);
            *vnrfile = vnr_file_create_new(full_filepath, display_name, is_directory);
            free(full_filepath);
        }
        free(display_name);
    }
    if(file_info_success) {
        g_object_unref(fileinfo);
    }
    g_object_unref(file);
    return file_info_success;
}


static void
vnr_file_add_file_to_lists_if_possible(gchar   *filepath,
                                       GList  **dir_list,
                                       GList  **file_list,
                                       struct Preference_Settings* preference_settings,
                                       GError **error)
{
    VnrFile *vnrfile;
    gboolean file_info_ok = vnr_file_get_file_info(filepath,
                                                   &vnrfile,
                                                   preference_settings->include_hidden,
                                                   error);

    if(file_info_ok && vnr_file_is_directory(vnrfile) && preference_settings->include_dirs) {
        *dir_list  = g_list_prepend( *dir_list, vnrfile);
    } else if(file_info_ok && vnr_file_is_image_file(vnrfile)) {
        *file_list = g_list_prepend(*file_list, vnrfile);
    } else if(vnrfile != NULL) {
        vnr_file_destroy_data(vnrfile);
    }
}

static void
vnr_append_file_and_dir_lists_to_tree(GNode  **tree,
                                      GList  **dir_list,
                                      GList  **file_list,
                                      struct Preference_Settings* preference_settings,
                                      GError **error)
{
    add_file_list_to_tree(tree, file_list, preference_settings);
    add_directory_list_to_tree(tree, dir_list, preference_settings, error);
}

static void
add_file_list_to_tree(GNode **tree,
                      GList **file_list,
                      struct Preference_Settings *preference_settings) {

    *file_list = g_list_sort(*file_list, vnr_file_list_compare);

    while(*file_list != NULL) {
        GNode *node = g_node_new((*file_list)->data);
        add_node_in_tree(*tree, node);

        if(preference_settings->set_file_monitor_for_file) {
            vnr_file_set_file_monitor(node, preference_settings);
        }

        *file_list = g_list_next(*file_list);
    }
}

static void
add_directory_list_to_tree(GNode  **tree,
                           GList  **dir_list,
                           struct Preference_Settings *preference_settings,
                           GError **error) {

    *dir_list  = g_list_sort(*dir_list, vnr_file_list_compare);

    struct Preference_Settings* dir_preference_settings = create_preference_settings(preference_settings->include_hidden,
                                                                                     preference_settings->include_dirs,
                                                                                     FALSE,
                                                                                     preference_settings->cb,
                                                                                     preference_settings->cb_data);
    while(*dir_list != NULL) {

        GNode *node = vnr_file_dir_content_to_list((*dir_list)->data,
                                                   dir_preference_settings,
                                                   error);
        vnr_file_set_file_monitor(node, preference_settings);

        add_node_in_tree(*tree, node);
        *dir_list = g_list_next(*dir_list);
    }

    free(dir_preference_settings);
}


static GNode*
vnr_file_dir_content_to_list(VnrFile  *vnrfile,
                             struct Preference_Settings* preference_settings,
                             GError   **error)
{
    GNode *tree       = g_node_new(vnrfile);
    GList *dir_list   = NULL;
    GList *file_list  = NULL;

    GFile *file;
    GFileEnumerator *f_enum;
    GFileInfo *file_info;

    char* folder_path = vnrfile->path;

    file   = g_file_new_for_path(folder_path);
    f_enum = g_file_enumerate_children(file,
                                       G_FILE_ATTRIBUTE_STANDARD_NAME","
                                       G_FILE_ATTRIBUTE_STANDARD_DISPLAY_NAME","
                                       G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE","
                                       G_FILE_ATTRIBUTE_STANDARD_IS_HIDDEN,
                                       G_FILE_QUERY_INFO_NONE,
                                       NULL, NULL);
    file_info = g_file_enumerator_next_file(f_enum, NULL, NULL);


    while(file_info != NULL) {
        char* child_path = g_strjoin(G_DIR_SEPARATOR_S, folder_path,
                                     (char*)g_file_info_get_name (file_info), NULL);

        vnr_file_add_file_to_lists_if_possible(child_path,
                                               &dir_list,
                                               &file_list,
                                               preference_settings,
                                               error);

        free(child_path);
        g_object_unref(file_info);
        file_info = g_file_enumerator_next_file(f_enum, NULL, NULL);
    }

    g_object_unref(file);
    g_file_enumerator_close(f_enum, NULL, NULL);
    g_object_unref(f_enum);

    vnr_append_file_and_dir_lists_to_tree(&tree,
                                          &dir_list,
                                          &file_list,
                                          preference_settings,
                                          error);
    g_list_free(dir_list);
    g_list_free(file_list);
    return tree;
}

static char*
vnr_get_parent_file_path(char *path)
{
    char* parent_path = NULL;

    GFile *file = g_file_new_for_path(path);
    GFile *parent = g_file_get_parent(file);

    parent_path = g_file_get_path(parent);

    g_object_unref(parent);
    g_object_unref(file);

    return parent_path;
}


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
                                   gboolean include_dirs,
                                   callback cb,
                                   gpointer cb_data,
                                   GError **error)
{
    GNode *tree = NULL;
    VnrFile* vnrfile;
    gboolean file_info_ok;

    struct Preference_Settings* preference_settings = create_preference_settings(include_hidden,
                                                                                 include_dirs,
                                                                                 FALSE,
                                                                                 cb,
                                                                                 cb_data);

    file_info_ok = vnr_file_get_file_info(uri,
                                          &vnrfile,
                                          include_hidden,
                                          error);

    if(file_info_ok && vnrfile != NULL && vnrfile->is_directory) {
        tree = vnr_file_dir_content_to_list(vnrfile,
                                            preference_settings,
                                            error);
        vnr_file_set_file_monitor(tree, preference_settings);

        tree = get_next_in_tree(tree);

    } else if(file_info_ok && vnrfile != NULL) {
        vnr_file_destroy_data(vnrfile);
        char* parent_path = vnr_get_parent_file_path(uri);

        file_info_ok = vnr_file_get_file_info(parent_path,
                                              &vnrfile,
                                              include_hidden,
                                              error);

        if(file_info_ok && vnrfile != NULL) {
            tree = vnr_file_dir_content_to_list(vnrfile,
                                                preference_settings,
                                                error);
            vnr_file_set_file_monitor(tree, preference_settings);
        }

        GNode *node = get_child_in_directory(tree, uri);
        if(node == NULL) {
            tree = get_next_in_tree(tree);
        } else {
            tree = node;
        }
        free(parent_path);
    }

    free(preference_settings);
    return tree;
}

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
                                 gboolean include_dirs,
                                 callback cb,
                                 gpointer cb_data,
                                 GError **error)
{
    GNode *tree      = g_node_new(NULL);
    GList *dir_list  = NULL;
    GList *file_list = NULL;


    struct Preference_Settings* dir_preference_settings = create_preference_settings(include_hidden,
                                                                                     TRUE,
                                                                                     TRUE,
                                                                                     cb,
                                                                                     cb_data);

    while(uri_list != NULL) {

        vnr_file_add_file_to_lists_if_possible(uri_list->data,
                                               &dir_list,
                                               &file_list,
                                               dir_preference_settings,
                                               error);
        g_clear_error(error);
        uri_list = g_slist_next(uri_list);
    }

    struct Preference_Settings* preference_settings = create_preference_settings(include_hidden,
                                                                                 include_dirs,
                                                                                 TRUE,
                                                                                 cb,
                                                                                 cb_data);
    vnr_append_file_and_dir_lists_to_tree(&tree,
                                          &dir_list,
                                          &file_list,
                                          preference_settings,
                                          error);

    tree = get_next_in_tree(tree);

    g_list_free(dir_list);
    g_list_free(file_list);
    free(dir_preference_settings);
    free(preference_settings);
    return tree;
}






static gboolean is_leaf(GNode *node) {
    VnrFile* vnrfile = node->data;
    return vnrfile != NULL && !vnrfile->is_directory; // A leaf in the tree
        // can represent an empty directory. Otherwise we could do G_NODE_IS_LEAF(node)
}

static gboolean has_more_siblings_in_direction(GNode *tree, Direction direction) {
    return tree != (direction == RIGHT ? g_node_last_sibling(tree) : g_node_first_sibling(tree));
}

static GNode* get_prev_or_next(GNode* tree, Direction direction) {
    return direction == RIGHT ? g_node_next_sibling(tree) : g_node_prev_sibling(tree);
}
static GNode* get_first_or_last(GNode* tree, Direction direction) {
    return direction == RIGHT ? g_node_first_child(tree) : g_node_last_child(tree);
}

static GNode* recursively_find_prev_or_next(GNode *tree, GNode *original_node, Direction direction, Course course) {

    if(tree == NULL || tree == original_node) {
        return original_node;
    }
    if(is_leaf(tree)) {
        return tree;
    }

    // It is a directory.

    GNode *node;
    Course new_course = CONTINUE;
    if(G_NODE_IS_ROOT(tree)) {
        node = get_first_or_last(tree, direction);

    } else if(has_children(tree) && course != RETREAT) {
        node = get_first_or_last(tree, direction);

    } else if(has_more_siblings_in_direction(tree, direction)) {
        node = get_prev_or_next(tree, direction);

    } else {
        node = tree->parent;
        new_course = RETREAT;
    }
    return recursively_find_prev_or_next(node, original_node, direction, new_course);
}

static GNode* get_prev_or_next_in_tree(GNode *tree, Direction direction) {

    if(tree == NULL) {
        return NULL;
    }
    GNode *next = tree;
    Course course = CONTINUE;

    if(G_NODE_IS_ROOT(tree)) {
        // Is root
        next = get_first_or_last(tree, direction);

    } else if(is_leaf(tree) && has_more_siblings_in_direction(tree, direction)) {
        // Is leaf with more siblings
        next = get_prev_or_next(tree, direction);

    } else if(is_leaf(tree) && !has_more_siblings_in_direction(tree, direction)) {
        // Is leaf with no more siblings
        next = tree->parent;
        course = RETREAT;

    } else if(!is_leaf(tree) && has_children(tree)) {
        // Is directory with children
        next = get_first_or_last(tree, direction);

    } else if(!is_leaf(tree) && has_more_siblings_in_direction(tree, direction)) {
        // Is directory without children but with more siblings
        next = get_prev_or_next(tree, direction);

    } else if(!is_leaf(tree) && !has_children(tree) && !has_more_siblings_in_direction(tree, direction)) {
        // Is directory without children and with no more siblings
        next = tree->parent;
        course = RETREAT;
    }

    return recursively_find_prev_or_next(next, tree, direction, course);
}

/**
 * Returns the first file (i.e. not directory) in the given @tree@.
 * Will climb up the structure relative to @tree@ if needed.
 * If there is no such file, @tree@ will be returned.
 */
GNode* get_first_in_tree(GNode* tree) {
    GNode *node = get_root_node(tree);
    return get_prev_or_next_in_tree(node, RIGHT);
}

/**
 * Returns the last file (i.e. not directory) in the given @tree@.
 * Will climb up the structure relative to @tree@ if needed.
 * If there is no such file, @tree@ will be returned.
 */
GNode* get_last_in_tree(GNode* tree) {
    GNode *node = get_first_in_tree(tree);
    if(node == NULL || node->data == NULL) {
        return tree;
    } else {
        return get_prev_or_next_in_tree(node, LEFT);
    }
}




static gboolean found_position_where_node_should_be_inserted(GNode *currnode, GNode *newnode) {
    if(currnode == NULL) {
        return TRUE;
    }

    gboolean currnode_is_dir = !is_leaf(currnode);
    gboolean newnode_is_file = is_leaf(newnode);
    gboolean both_nodes_are_of_same_type = is_leaf(currnode) == is_leaf(newnode);
    return (currnode_is_dir && newnode_is_file) ||
           (both_nodes_are_of_same_type && g_strcmp0(((VnrFile*) currnode->data)->display_name_collate, ((VnrFile*) newnode->data)->display_name_collate) > 0);
}

/**
 * Adds @node@ as a child of @tree@, sorted by @display_name_collate@.
 * @tree@ must be a directory, not a file; @node@ may be a file or a
 * directory. When sorting, files will be inserted before directories.
 * If @node@ is already present, @tree@ will remain unchanged.
 */
void add_node_in_tree(GNode *tree, GNode *node) {
    if(node == NULL || node->data == NULL || tree == NULL || is_leaf(tree)) {
        return;
    }
    GNode *child = get_first_or_last(tree, RIGHT);

    gboolean already_present = FALSE;
    int i = 0;
    while(!found_position_where_node_should_be_inserted(child, node)) {
        i++;
        if(g_strcmp0(((VnrFile*) child->data)->path, ((VnrFile*) node->data)->path) == 0) {
            already_present = TRUE;
            break;
        }
        if(!has_more_siblings(child)) {
            break;
        }
        child = g_node_next_sibling(child);
    }
    if(!already_present) {
        g_node_insert(tree, i, node);
    }
}


static gboolean node_has_path(GNode *node, char *path) {
    if(node == NULL || node->data == NULL) {
        return FALSE;
    }
    VnrFile *vnrfile = node->data;
    return g_strcmp0(vnrfile->path, path) == 0;
}


static GNode* recursively_get_child_in_directory(GNode *tree, char* path) {
    if(node_has_path(tree, path)) {
        return tree;
    }
    GNode *child, *dirchild;

    if(has_children(tree)) {
        child = g_node_first_child(tree);
        dirchild = recursively_get_child_in_directory(child, path);

        while(dirchild == NULL && has_more_siblings(child)) {
            child = g_node_next_sibling(child);
            dirchild = recursively_get_child_in_directory(child, path);
        }
        return dirchild;
    }
    return NULL;
}


/**
 * Will move to the topmost root of @tree@, traverse the whole structure
 * and return the node (file or directory) whose path is equal to
 * @path@. If no such node exists in the structure, NULL is returned.
 */
GNode* get_child_in_directory(GNode *tree, char* path) {
    return recursively_get_child_in_directory(get_root_node(tree), path);
}

static gboolean tree_contains_path(GNode *tree, char *path) {
    return get_child_in_directory(tree, path) != NULL;
}

/**
 * Returns whether or not the given @tree@ has children
 * (files or directories).
 */
gboolean has_children(GNode *tree) {
    return tree != NULL && g_node_n_children(tree) > 0;
}

/**
 * Returns whether or not the given @tree@ has any more siblings (files
 * or directories). If a @tree@ has n children, then giving any of the
 * first n-1 children of @tree@ as input to this function would return
 * TRUE. For child n, this function would return FALSE as there are no
 * more children after it.
 */
gboolean has_more_siblings(GNode *tree) {
    return has_more_siblings_in_direction(tree, RIGHT);
}



/**
 * Returns the next file (i.e. not directory) in the given @tree@.
 * Will climb up the structure relative to @tree@ or "wrap around" if
 * needed. If there is no such file, @tree@ will be returned.
 */
GNode* get_next_in_tree(GNode *tree) {
    return get_prev_or_next_in_tree(tree, RIGHT);
}

/**
 * Returns the previous file (i.e. not directory) in the given @tree@.
 * Will climb up the structure relative to @tree@ or "wrap around" if
 * needed. If there is no such file, @tree@ will be returned.
 */
GNode* get_prev_in_tree(GNode *tree) {
    return get_prev_or_next_in_tree(tree, LEFT);
}

static void get_number_of_leaves(GNode *tree, GNode *node_to_look_for, int *current, int *total) {
    if(tree == NULL) {
        return;
    }

    if(is_leaf(tree)) {
        *total = *total + 1;
    }
    if(tree == node_to_look_for) {
        *current = *total;
    }

    GNode *child = get_first_or_last(tree, RIGHT);
    while(child != NULL) {

        get_number_of_leaves(child, node_to_look_for, current, total);
        if(!has_more_siblings(child)) {
            break;
        }
        child = get_prev_or_next(child, RIGHT);
    }
}

/**
 * This function will move to the root from @tree@, and go through the
 * whole structure. All files (i.e. not directories) will be counted,
 * and that number will be placed in @total@. The position of @tree@ in
 * the structure will be placed in @tree_position@.
 */
void get_leaf_position(GNode *tree, int *tree_position, int *total) {

    *tree_position = -1;
    *total = 0;

    get_number_of_leaves(get_root_node(tree), tree, tree_position, total);
}

/**
 * This function will move to the root from @tree@, and go through the
 * whole structure. All files (i.e. not directories) will be counted,
 * and that number will be returned.
 */
int get_total_number_of_leaves(GNode *tree) {

    int tree_position = -1;
    int total = 0;

    get_number_of_leaves(get_root_node(tree), NULL, &tree_position, &total);
    return total;
}

/**
 * Returns the topmost root of @tree@.
 */
GNode* get_root_node(GNode *tree) {
    GNode *node = tree;
    while (node != NULL && node->parent != NULL) {
        node = node->parent;
    }
    return node;
}


static gboolean destroy_node(GNode *node, gpointer data) {
    UNUSED(data);
    vnr_file_destroy_data(node->data);
    return FALSE;
}

/**
 * Frees @tree@. If it is a sub-tree, the rest of the tree will be left
 * alone. Traverses the whole of @tree@ and destroys the nodes as well.
 */
void free_current_tree(GNode *tree) {
    g_node_traverse(tree, G_POST_ORDER, G_TRAVERSE_ALL, -1, destroy_node, NULL);
    g_node_destroy(tree);
}

/**
 * Moves to the topmost root of @tree@ and frees the whole structure.
 * Traverses the whole tree and destroys the nodes as well.
 */
void free_whole_tree(GNode *tree) {
    GNode *node = get_root_node(tree);
    free_current_tree(node);
}
