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


static void assert_child_is_equal(char* description, GNode* tree, char* expected) {
    VnrFile *vnrfile = tree != NULL ? tree->data : NULL;
    assert_equals(description, expected, vnrfile != NULL ? vnrfile->path : "NULL");
}


static void test_getChildInDirectory_FindFromRoot_FileExists() {
    before();
    GNode *tree = single_folder(TRUE, TRUE);

// THIS IS THE STRUCTURE:
//
// test-dir (7 children)
// ├─ .apa.png
// ├─ .depa.gif
// ├─ bepa.png
// ├─ cepa.jpg
// ├─ epa.png
// ├─┬dir_one (3 children)
// │ ├─ .three.png
// │ ├─ two.jpg
// │ └─┬.secrets (1 children)
// │   └─ img.jpg
// └─┬dir_two (7 children)
//   ├─ apa.png
//   ├─ bepa.png
//   ├─ cepa.png
//   ├─┬sub_dir_four (2 children)
//   │ ├──subsub (0 children)
//   │ └──subsub2 (0 children)
//   ├─┬sub_dir_one (3 children)
//   │ ├─ img0.png
//   │ ├─ img1.png
//   │ └─ img2.png
//   ├──sub_dir_three (0 children)
//   └─┬sub_dir_two (3 children)
//     ├─ img0.png
//     ├─ img1.png
//     ├─ img2.png
//     └─ img3.png

    char *path0 = get_absolute_path(testdir_path, "/bepa.png");
    char *path1 = get_absolute_path(testdir_path, "/dir_two/bepa.png");

    GNode *child0 = get_child_in_directory(tree, path0);
    GNode *child1 = get_child_in_directory(tree, path1);

    assert_child_is_equal("Get child in directory ─ From root ─ File exists", child0, path0);
    assert_child_is_equal("Get child in directory ─ From root ─ File exists", child1, path1);

    free(path0);
    free(path1);
    free_whole_tree(tree);
    after();
}

static void test_getChildInDirectory_FindFromFileAndDir_FileExists() {
    before();
    GNode *tree = single_folder(TRUE, TRUE);

// THIS IS THE STRUCTURE:
//
// test-dir (7 children)
// ├─ .apa.png
// ├─ .depa.gif
// ├─ bepa.png
// ├─ cepa.jpg
// ├─ epa.png
// ├─┬dir_one (3 children)
// │ ├─ .three.png
// │ ├─ two.jpg
// │ └─┬.secrets (1 children)
// │   └─ img.jpg
// └─┬dir_two (7 children)
//   ├─ apa.png
//   ├─ bepa.png
//   ├─ cepa.png
//   ├─┬sub_dir_four (2 children)
//   │ ├──subsub (0 children)
//   │ └──subsub2 (0 children)
//   ├─┬sub_dir_one (3 children)
//   │ ├─ img0.png
//   │ ├─ img1.png
//   │ └─ img2.png
//   ├──sub_dir_three (0 children)
//   └─┬sub_dir_two (3 children)
//     ├─ img0.png
//     ├─ img1.png
//     ├─ img2.png
//     └─ img3.png

    char *path0 = get_absolute_path(testdir_path, "/bepa.png");
    char *path1 = get_absolute_path(testdir_path, "/dir_two");
    char *path2 = get_absolute_path(testdir_path, "/dir_two/bepa.png");

    GNode *child0 = get_child_in_directory(tree, path0);
    GNode *child1 = g_node_last_child(get_root_node(tree));
    assert_child_is_equal("Get child in directory ─ Find bepa.png", child0, path0);
    assert_child_is_equal("Get child in directory ─ Last child is dir_two", child1, path1);

    GNode *child2 = get_child_in_directory(child0, path2);
    GNode *child3 = get_child_in_directory(child1, path2);

    assert_child_is_equal("Get child in directory ─ From file ─ File exists", child2, path2);
    assert_child_is_equal("Get child in directory ─ From subdir ─ File exists", child3, path2);

    free(path0);
    free(path1);
    free(path2);
    free_whole_tree(tree);
    after();
}

static void test_getChildInDirectory_FindAbove_FileExists() {
    before();
    GNode *tree = single_folder(TRUE, TRUE);

// THIS IS THE STRUCTURE:
//
// test-dir (7 children)
// ├─ .apa.png
// ├─ .depa.gif
// ├─ bepa.png
// ├─ cepa.jpg
// ├─ epa.png
// ├─┬dir_one (3 children)
// │ ├─ .three.png
// │ ├─ two.jpg
// │ └─┬.secrets (1 children)
// │   └─ img.jpg
// └─┬dir_two (7 children)
//   ├─ apa.png
//   ├─ bepa.png
//   ├─ cepa.png
//   ├─┬sub_dir_four (2 children)
//   │ ├──subsub (0 children)
//   │ └──subsub2 (0 children)
//   ├─┬sub_dir_one (3 children)
//   │ ├─ img0.png
//   │ ├─ img1.png
//   │ └─ img2.png
//   ├──sub_dir_three (0 children)
//   └─┬sub_dir_two (3 children)
//     ├─ img0.png
//     ├─ img1.png
//     ├─ img2.png
//     └─ img3.png

    char *path0 = get_absolute_path(testdir_path, "/dir_two");
    char *path1 = get_absolute_path(testdir_path, "/epa.png");

    GNode *child0 = g_node_last_child(get_root_node(tree));
    assert_child_is_equal("Get child in directory ─ Find above ─ Last child is dir_two", child0, path0);

    GNode *child1 = get_child_in_directory(child0, path1);

    assert_child_is_equal("Get child in directory ─ Find above ─ File exists", child1, path1);

    free(path0);
    free(path1);
    free_whole_tree(tree);
    after();
}

static void test_getChildInDirectory_FileDoesNotExist() {
    before();
    GNode *tree = single_folder(TRUE, TRUE);

// THIS IS THE STRUCTURE:
//
// test-dir (7 children)
// ├─ .apa.png
// ├─ .depa.gif
// ├─ bepa.png
// ├─ cepa.jpg
// ├─ epa.png
// ├─┬dir_one (3 children)
// │ ├─ .three.png
// │ ├─ two.jpg
// │ └─┬.secrets (1 children)
// │   └─ img.jpg
// └─┬dir_two (7 children)
//   ├─ apa.png
//   ├─ bepa.png
//   ├─ cepa.png
//   ├─┬sub_dir_four (2 children)
//   │ ├──subsub (0 children)
//   │ └──subsub2 (0 children)
//   ├─┬sub_dir_one (3 children)
//   │ ├─ img0.png
//   │ ├─ img1.png
//   │ └─ img2.png
//   ├──sub_dir_three (0 children)
//   └─┬sub_dir_two (3 children)
//     ├─ img0.png
//     ├─ img1.png
//     ├─ img2.png
//     └─ img3.png

    char *path0 = get_absolute_path(testdir_path, "/dir_two");
    char *path1 = get_absolute_path(testdir_path, "/dir_two/depa.png");

    GNode *child = g_node_last_child(get_root_node(tree));
    assert_child_is_equal("Get child in directory ─ File does not exist ─ Last child is dir_two", child, path0);

    child = get_child_in_directory(child, path1);

    assert_tree_is_null("Get child in directory ─ File does not exist", child);

    free(path0);
    free(path1);
    free_whole_tree(tree);
    after();
}

static void test_getChildInDirectory_SearchFromSubDirectoryIsEmpty() {
    before();
    GNode *tree = single_folder(TRUE, TRUE);

// THIS IS THE STRUCTURE:
//
// test-dir (7 children)
// ├─ .apa.png
// ├─ .depa.gif
// ├─ bepa.png
// ├─ cepa.jpg
// ├─ epa.png
// ├─┬dir_one (3 children)
// │ ├─ .three.png
// │ ├─ two.jpg
// │ └─┬.secrets (1 children)
// │   └─ img.jpg
// └─┬dir_two (7 children)
//   ├─ apa.png
//   ├─ bepa.png
//   ├─ cepa.png
//   ├─┬sub_dir_four (2 children)
//   │ ├──subsub (0 children)
//   │ └──subsub2 (0 children)
//   ├─┬sub_dir_one (3 children)
//   │ ├─ img0.png
//   │ ├─ img1.png
//   │ └─ img2.png
//   ├──sub_dir_three (0 children)
//   └─┬sub_dir_two (3 children)
//     ├─ img0.png
//     ├─ img1.png
//     ├─ img2.png
//     └─ img3.png

    char *path0 = get_absolute_path(testdir_path, "/dir_two/sub_dir_four/subsub");
    char *path1 = get_absolute_path(testdir_path, "/.apa.png");

    GNode *child = get_root_node(tree);
    child = g_node_last_child(child);
    child = g_node_first_child(child);
    child = g_node_next_sibling(child);
    child = g_node_next_sibling(child);
    child = g_node_next_sibling(child);
    child = g_node_first_child(child);
    assert_child_is_equal("Get child in directory ─ From subdir ─ Last child is subsub", child, path0);

    child = get_child_in_directory(child, path1);

    assert_child_is_equal("Get child in directory ─ From subdir ─ File exists", child, path1);

    free(path0);
    free(path1);
    free_whole_tree(tree);
    after();
}

static void test_getChildInDirectory_DirectoryIsEmpty() {
    before();
    GNode *tree = single_folder(TRUE, TRUE);

// THIS IS THE STRUCTURE:
//
// test-dir (7 children)
// ├─ .apa.png
// ├─ .depa.gif
// ├─ bepa.png
// ├─ cepa.jpg
// ├─ epa.png
// ├─┬dir_one (3 children)
// │ ├─ .three.png
// │ ├─ two.jpg
// │ └─┬.secrets (1 children)
// │   └─ img.jpg
// └─┬dir_two (7 children)
//   ├─ apa.png
//   ├─ bepa.png
//   ├─ cepa.png
//   ├─┬sub_dir_four (2 children)
//   │ ├──subsub (0 children)
//   │ └──subsub2 (0 children)
//   ├─┬sub_dir_one (3 children)
//   │ ├─ img0.png
//   │ ├─ img1.png
//   │ └─ img2.png
//   ├──sub_dir_three (0 children)
//   └─┬sub_dir_two (3 children)
//     ├─ img0.png
//     ├─ img1.png
//     ├─ img2.png
//     └─ img3.png

    char *path0 = get_absolute_path(testdir_path, "/dir_two/sub_dir_four/subsub");
    char *path1 = get_absolute_path(testdir_path, "/dir_two/sub_dir_four/subsub/apa.jpg");

    GNode *child = get_root_node(tree);
    child = g_node_last_child(child);
    child = g_node_first_child(child);
    child = g_node_next_sibling(child);
    child = g_node_next_sibling(child);
    child = g_node_next_sibling(child);
    child = g_node_first_child(child);
    assert_child_is_equal("Get child in directory ─ Last child is subsub", child, path0);

    child = get_child_in_directory(child, path1);

    assert_tree_is_null("Get child in directory ─ Directory is empty", child);

    free(path0);
    free(path1);
    free_whole_tree(tree);
    after();
}

static void test_getChildInDirectory_FindRoot() {
    before();
    GNode *tree = single_folder(TRUE, TRUE);

// THIS IS THE STRUCTURE:
//
// test-dir (7 children)
// ├─ .apa.png
// ├─ .depa.gif
// ├─ bepa.png
// ├─ cepa.jpg
// ├─ epa.png
// ├─┬dir_one (3 children)
// │ ├─ .three.png
// │ ├─ two.jpg
// │ └─┬.secrets (1 children)
// │   └─ img.jpg
// └─┬dir_two (7 children)
//   ├─ apa.png
//   ├─ bepa.png
//   ├─ cepa.png
//   ├─┬sub_dir_four (2 children)
//   │ ├──subsub (0 children)
//   │ └──subsub2 (0 children)
//   ├─┬sub_dir_one (3 children)
//   │ ├─ img0.png
//   │ ├─ img1.png
//   │ └─ img2.png
//   ├──sub_dir_three (0 children)
//   └─┬sub_dir_two (3 children)
//     ├─ img0.png
//     ├─ img1.png
//     ├─ img2.png
//     └─ img3.png

    char *path0 = get_absolute_path(testdir_path, "");

    GNode *child = get_child_in_directory(tree, path0);

    assert_child_is_equal("Get child in directory ─ Find root", child, path0);

    free(path0);
    free_whole_tree(tree);
    after();
}

static void test_getChildInDirectory_DirectoryIsNull() {
    before();
    char *path0 = get_absolute_path(testdir_path, "/dir_two/sub_dir_four/subsub/apa.jpg");

    GNode *child = get_child_in_directory(NULL, path0);

    assert_tree_is_null("Get child in directory ─ Directory is empty", child);

    free(path0);
    after();
}



int main() {
    before_all();

    test_getChildInDirectory_FindFromRoot_FileExists();
    test_getChildInDirectory_FindFromFileAndDir_FileExists();
    test_getChildInDirectory_FindAbove_FileExists();
    test_getChildInDirectory_FileDoesNotExist();
    test_getChildInDirectory_SearchFromSubDirectoryIsEmpty();
    test_getChildInDirectory_DirectoryIsEmpty();
    test_getChildInDirectory_FindRoot();
    test_getChildInDirectory_DirectoryIsNull();

    after_all();
    return errors == 0 ? 0 : -1;
}
