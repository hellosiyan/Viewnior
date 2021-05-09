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

#include "utils.h"


static void test_getNextInTree_NullIn() {
    before();
    assert_tree_is_null("Get Next ─ Input is NULL", get_next_in_tree(NULL));
    assert_tree_is_null("Get First ─ Input is NULL", get_first_in_tree(NULL));
    assert_tree_is_null("Get Last ─ Input is NULL", get_last_in_tree(NULL));
    after();
}

static void test_getNextInTree_RootWithoutElements() {
    before();

    GNode *tree = g_node_new(NULL);
    assert_trees_equal("Get Next ─ Input is Root without elements", tree, get_next_in_tree(tree));
    assert_trees_equal("Get First ─ Input is Root without elements", tree, get_first_in_tree(tree));
    assert_trees_equal("Get Last ─ Input is Root without elements", tree, get_last_in_tree(tree));
    free_whole_tree(tree);

    after();
}

static void test_getNextInTree_RootWithOnlyDir() {
    before();

    VnrFile* vnrfile = vnr_file_create_new("filepath", "display_name", TRUE);
    GNode *tree = g_node_new(vnrfile);
    assert_trees_equal("Get Next ─ Input is Root with only one dir", tree, get_next_in_tree(tree));
    assert_trees_equal("Get First ─ Input is Root with only one dir", tree, get_first_in_tree(tree));
    assert_trees_equal("Get Last ─ Input is Root with only one dir", tree, get_last_in_tree(tree));
    free_whole_tree(tree);

    after();
}

static void test_getNextInTree_SingleFolder_RootWithOnlyThreeDirs() {
    // No before!

    create_dir(testdir_path, "");
    create_dir(testdir_path, "/apa");
    create_dir(testdir_path, "/bepa");
    create_dir(testdir_path, "/cepa");

    GNode *tree = single_folder(TRUE, TRUE);

    assert_trees_equal("Get Next ─ Single folder ─ Input is Root with only three dirs", tree, get_next_in_tree(tree));
    assert_trees_equal("Get First ─ Single folder ─ Input is Root with only three dirs", tree, get_first_in_tree(tree));
    assert_trees_equal("Get Last ─ Single folder ─ Input is Root with only three dirs", tree, get_last_in_tree(tree));
    free_whole_tree(tree);

    after();
}

static void test_getNextInTree_UriList_RootWithOnlyThreeDirs() {
    // No before!

    create_dir(testdir_path, "");
    create_dir(testdir_path, "/apa");
    create_dir(testdir_path, "/bepa");
    create_dir(testdir_path, "/cepa");

    GSList *uri_list = NULL;
    uri_list = add_path_to_list(uri_list, testdir_path, "/bepa.png");
    uri_list = add_path_to_list(uri_list, testdir_path, "/cepa.jpg");

    GError *error = NULL;
    GNode *tree = create_tree_from_uri_list(uri_list, TRUE, TRUE, NULL, NULL, &error);
    assert_error_is_null(error);
    free(error);
    g_slist_free_full(uri_list, free);

    assert_trees_equal("Get Next ─ Uri List ─ Input is Root with only three dirs", tree, get_next_in_tree(tree));
    assert_trees_equal("Get First ─ Uri List ─ Input is Root with only three dirs", tree, get_first_in_tree(tree));
    assert_trees_equal("Get Last ─ Uri List ─ Input is Root with only three dirs", tree, get_last_in_tree(tree));
    free_whole_tree(tree);

    after();
}



int main() {
    before_all();

    test_getNextInTree_NullIn();
    test_getNextInTree_RootWithoutElements();
    test_getNextInTree_RootWithOnlyDir();
    test_getNextInTree_SingleFolder_RootWithOnlyThreeDirs();
    test_getNextInTree_UriList_RootWithOnlyThreeDirs();

    after_all();
    return errors == 0 ? 0 : -1;
}
