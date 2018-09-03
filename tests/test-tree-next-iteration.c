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


static void test_getNextInTree_RootIn() {
    before();

    GNode *tree = single_file(TRUE, FALSE);
    tree = get_root_node(tree);
    assert_forward_iteration(tree, ".apa.png");
    assert_equals("Get First ─ Root in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Root in",   "epa.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));
    free_whole_tree(tree);

    after();
}

static void test_getNextInTree_FolderIn() {
    before();

    GNode *tree = single_file(TRUE, TRUE);
    tree = get_root_node(tree);

    tree = assert_forward_iteration(tree, ".apa.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));
    tree = assert_forward_iteration(tree, ".depa.gif");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));
    tree = assert_forward_iteration(tree, "bepa.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));
    tree = assert_forward_iteration(tree, "cepa.jpg");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));
    tree = assert_forward_iteration(tree, "epa.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = g_node_next_sibling(tree);
    VnrFile* vnrfile = tree->data;
    assert_equals("First directory should be dir_one", "dir_one", vnrfile->display_name);

    tree = assert_forward_iteration(tree, ".three.png");
    tree = assert_forward_iteration(tree, "two.jpg");
    tree = assert_forward_iteration(tree, "img.jpg");
    tree = assert_forward_iteration(tree, "apa.png");
    tree = assert_forward_iteration(tree, "bepa.png");
    tree = assert_forward_iteration(tree, "cepa.png");

    GNode *sub_dir_four = g_node_next_sibling(tree);
    vnrfile = sub_dir_four->data;
    assert_equals("Directory should be sub_dir_four", "sub_dir_four", vnrfile->display_name);

    GNode *subsub = g_node_first_child(sub_dir_four);
    vnrfile = subsub->data;
    assert_equals("Directory should be subsub", "subsub", vnrfile->display_name);

    assert_forward_iteration(subsub, "img0.png");

    GNode *subsub2 = g_node_next_sibling(subsub);
    vnrfile = subsub2->data;
    assert_equals("Directory should be subsub2", "subsub2", vnrfile->display_name);

    assert_forward_iteration(subsub2, "img0.png");

    free_whole_tree(tree);

    after();
}


static void test_getNextInTree_Iterate() {
    before();

    GNode *tree = single_file(TRUE, TRUE);

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

    tree = get_root_node(tree);
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_forward_iteration(tree, ".apa.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_forward_iteration(tree, ".depa.gif");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_forward_iteration(tree, "bepa.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_forward_iteration(tree, "cepa.jpg");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_forward_iteration(tree, "epa.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_forward_iteration(tree, ".three.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_forward_iteration(tree, "two.jpg");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_forward_iteration(tree, "img.jpg");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_forward_iteration(tree, "apa.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_forward_iteration(tree, "bepa.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_forward_iteration(tree, "cepa.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_forward_iteration(tree, "img0.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_forward_iteration(tree, "img1.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_forward_iteration(tree, "img2.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_forward_iteration(tree, "img0.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_forward_iteration(tree, "img1.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_forward_iteration(tree, "img2.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_forward_iteration(tree, "img3.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    // Loop from the beginning.
    tree = assert_forward_iteration(tree, ".apa.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_forward_iteration(tree, ".depa.gif");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    free_whole_tree(tree);

    after();
}

static void test_getPrevInTree_Iterate() {
    before();

    GNode *tree = single_file(TRUE, TRUE);

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

    tree = get_root_node(tree);
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_backward_iteration(tree, "img3.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_backward_iteration(tree, "img2.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_backward_iteration(tree, "img1.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_backward_iteration(tree, "img0.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_backward_iteration(tree, "img2.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_backward_iteration(tree, "img1.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_backward_iteration(tree, "img0.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_backward_iteration(tree, "cepa.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_backward_iteration(tree, "bepa.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_backward_iteration(tree, "apa.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_backward_iteration(tree, "img.jpg");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_backward_iteration(tree, "two.jpg");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_backward_iteration(tree, ".three.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_backward_iteration(tree, "epa.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_backward_iteration(tree, "cepa.jpg");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_backward_iteration(tree, "bepa.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_backward_iteration(tree, ".depa.gif");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_backward_iteration(tree, ".apa.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    // Loop from the end.
    tree = assert_backward_iteration(tree, "img3.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_backward_iteration(tree, "img2.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));


    free_whole_tree(tree);

    after();
}

static void test_getNextInTree_UriList_Iterate() {
    before();

    GNode *tree = uri_list(TRUE, TRUE);

// THIS IS THE STRUCTURE:
//
// <ROOT> (5 children)
// ├─ .apa.png
// ├─ .depa.gif
// ├─ bepa.png
// ├─ cepa.jpg
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
//   └─┬sub_dir_two (4 children)
//     ├─ img0.png
//     ├─ img1.png
//     ├─ img2.png
//     └─ img3.png

    tree = get_root_node(tree);
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_forward_iteration(tree, ".apa.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_forward_iteration(tree, ".depa.gif");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_forward_iteration(tree, "bepa.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_forward_iteration(tree, "cepa.jpg");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_forward_iteration(tree, "apa.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_forward_iteration(tree, "bepa.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_forward_iteration(tree, "cepa.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_forward_iteration(tree, "img0.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_forward_iteration(tree, "img1.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_forward_iteration(tree, "img2.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_forward_iteration(tree, "img0.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_forward_iteration(tree, "img1.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_forward_iteration(tree, "img2.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_forward_iteration(tree, "img3.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    // Loop from the beginning.
    tree = assert_forward_iteration(tree, ".apa.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_forward_iteration(tree, ".depa.gif");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));


    free_whole_tree(tree);

    after();
}

static void test_getPrevInTree_UriList_Iterate() {
    before();

    GNode *tree = uri_list(TRUE, TRUE);

// THIS IS THE STRUCTURE:
//
// <ROOT> (5 children)
// ├─ .apa.png
// ├─ .depa.gif
// ├─ bepa.png
// ├─ cepa.jpg
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
//   └─┬sub_dir_two (4 children)
//     ├─ img0.png
//     ├─ img1.png
//     ├─ img2.png
//     └─ img3.png

    tree = get_root_node(tree);
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_backward_iteration(tree, "img3.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_backward_iteration(tree, "img2.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_backward_iteration(tree, "img1.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_backward_iteration(tree, "img0.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_backward_iteration(tree, "img2.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_backward_iteration(tree, "img1.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_backward_iteration(tree, "img0.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_backward_iteration(tree, "cepa.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_backward_iteration(tree, "bepa.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_backward_iteration(tree, "apa.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_backward_iteration(tree, "cepa.jpg");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_backward_iteration(tree, "bepa.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_backward_iteration(tree, ".depa.gif");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_backward_iteration(tree, ".apa.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    // Loop from the end.
    tree = assert_backward_iteration(tree, "img3.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));

    tree = assert_backward_iteration(tree, "img2.png");
    assert_equals("Get First ─ Folder in", ".apa.png", (VNR_FILE(get_first_in_tree(tree)->data)->display_name));
    assert_equals("Get Last ─ Folder in",  "img3.png", (VNR_FILE(get_last_in_tree (tree)->data)->display_name));


    free_whole_tree(tree);

    after();
}



int main() {
    before_all();

    test_getNextInTree_RootIn();
    test_getNextInTree_FolderIn();
    test_getNextInTree_Iterate();
    test_getPrevInTree_Iterate();
    test_getNextInTree_UriList_Iterate();
    test_getPrevInTree_UriList_Iterate();

    after_all();
    return errors == 0 ? 0 : -1;
}
