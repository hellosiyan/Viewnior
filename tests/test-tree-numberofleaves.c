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


static void test_getNumberOfLeaves_NullIn() {
    before();

    gint position, total;
    get_leaf_position(NULL, &position, &total);

    assert_numbers_equals("#Leaves ─ Null input", -1, position);
    assert_numbers_equals("#Leaves ─ Null input", 0, total);
    assert_numbers_equals("#Leaves ─ Null input", 0, get_total_number_of_leaves(NULL));

    after();
}

static void test_getNumberOfLeaves_SingleFileNonRecursive_ReturnSameNumberNoMatterWhereInTheTreeWeAre() {
    before();

    int position, total;
    GNode *tree = single_file(TRUE, FALSE);
    tree = get_root_node(tree);
    GNode *node = tree;

// THIS IS THE STRUCTURE:
//
// test-dir (5 children)
// ├─ .apa.png
// ├─ .depa.gif
// ├─ bepa.png
// ├─ cepa.jpg
// └─ epa.png

    get_leaf_position(node, &position, &total);
    assert_numbers_equals("#Leaves ─ Tree root as input ─ position", 0, position);
    assert_numbers_equals("#Leaves ─ Tree root as input ─ total", 5, total);


    node = tree;
    assert_numbers_equals("#Leaves ─ Single file, non-recursive ─ Different places in tree", 5, get_total_number_of_leaves(node));
    node = assert_forward_iteration(node, ".apa.png");
    assert_numbers_equals("#Leaves ─ Single file, non-recursive ─ Different places in tree", 5, get_total_number_of_leaves(node));
    node = assert_forward_iteration(node, ".depa.gif");
    assert_numbers_equals("#Leaves ─ Single file, non-recursive ─ Different places in tree", 5, get_total_number_of_leaves(node));

    free_whole_tree(tree);
    after();
}

static void test_getNumberOfLeaves_UriListRecursive_ReturnSameNumberNoMatterWhereInTheTreeWeAre() {
    before();

    int position, total;
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
    get_leaf_position(tree, &position, &total);
    assert_numbers_equals("#Leaves ─ Node iteration ─ position", 0, position);
    assert_numbers_equals("#Leaves ─ Node iteration ─ total", 14, total);
    assert_numbers_equals("#Leaves ─ UriList, recursive ─ Different places in tree", 14, get_total_number_of_leaves(tree));

    tree = assert_forward_iteration(tree, ".apa.png");
    get_leaf_position(tree, &position, &total);
    assert_numbers_equals("#Leaves ─ Node iteration ─ position", 1, position);
    assert_numbers_equals("#Leaves ─ Node iteration ─ total", 14, total);
    assert_numbers_equals("#Leaves ─ UriList, recursive ─ Different places in tree", 14, get_total_number_of_leaves(tree));

    tree = assert_forward_iteration(tree, ".depa.gif");
    get_leaf_position(tree, &position, &total);
    assert_numbers_equals("#Leaves ─ Node iteration ─ position", 2, position);
    assert_numbers_equals("#Leaves ─ Node iteration ─ total", 14, total);
    assert_numbers_equals("#Leaves ─ UriList, recursive ─ Different places in tree", 14, get_total_number_of_leaves(tree));

    tree = assert_forward_iteration(tree, "bepa.png");
    get_leaf_position(tree, &position, &total);
    assert_numbers_equals("#Leaves ─ Node iteration ─ position", 3, position);
    assert_numbers_equals("#Leaves ─ Node iteration ─ total", 14, total);
    assert_numbers_equals("#Leaves ─ UriList, recursive ─ Different places in tree", 14, get_total_number_of_leaves(tree));

    tree = assert_forward_iteration(tree, "cepa.jpg");
    get_leaf_position(tree, &position, &total);
    assert_numbers_equals("#Leaves ─ Node iteration ─ position", 4, position);
    assert_numbers_equals("#Leaves ─ Node iteration ─ total", 14, total);
    assert_numbers_equals("#Leaves ─ UriList, recursive ─ Different places in tree", 14, get_total_number_of_leaves(tree));

    tree = assert_forward_iteration(tree, "apa.png");
    get_leaf_position(tree, &position, &total);
    assert_numbers_equals("#Leaves ─ Node iteration ─ position", 5, position);
    assert_numbers_equals("#Leaves ─ Node iteration ─ total", 14, total);
    assert_numbers_equals("#Leaves ─ UriList, recursive ─ Different places in tree", 14, get_total_number_of_leaves(tree));

    tree = assert_forward_iteration(tree, "bepa.png");
    get_leaf_position(tree, &position, &total);
    assert_numbers_equals("#Leaves ─ Node iteration ─ position", 6, position);
    assert_numbers_equals("#Leaves ─ Node iteration ─ total", 14, total);
    assert_numbers_equals("#Leaves ─ UriList, recursive ─ Different places in tree", 14, get_total_number_of_leaves(tree));

    tree = assert_forward_iteration(tree, "cepa.png");
    get_leaf_position(tree, &position, &total);
    assert_numbers_equals("#Leaves ─ Node iteration ─ position", 7, position);
    assert_numbers_equals("#Leaves ─ Node iteration ─ total", 14, total);
    assert_numbers_equals("#Leaves ─ UriList, recursive ─ Different places in tree", 14, get_total_number_of_leaves(tree));

    tree = assert_forward_iteration(tree, "img0.png");
    get_leaf_position(tree, &position, &total);
    assert_numbers_equals("#Leaves ─ Node iteration ─ position", 8, position);
    assert_numbers_equals("#Leaves ─ Node iteration ─ total", 14, total);
    assert_numbers_equals("#Leaves ─ UriList, recursive ─ Different places in tree", 14, get_total_number_of_leaves(tree));

    tree = assert_forward_iteration(tree, "img1.png");
    get_leaf_position(tree, &position, &total);
    assert_numbers_equals("#Leaves ─ Node iteration ─ position", 9, position);
    assert_numbers_equals("#Leaves ─ Node iteration ─ total", 14, total);
    assert_numbers_equals("#Leaves ─ UriList, recursive ─ Different places in tree", 14, get_total_number_of_leaves(tree));

    tree = assert_forward_iteration(tree, "img2.png");
    get_leaf_position(tree, &position, &total);
    assert_numbers_equals("#Leaves ─ Node iteration ─ position", 10, position);
    assert_numbers_equals("#Leaves ─ Node iteration ─ total", 14, total);
    assert_numbers_equals("#Leaves ─ UriList, recursive ─ Different places in tree", 14, get_total_number_of_leaves(tree));

    tree = assert_forward_iteration(tree, "img0.png");
    get_leaf_position(tree, &position, &total);
    assert_numbers_equals("#Leaves ─ Node iteration ─ position", 11, position);
    assert_numbers_equals("#Leaves ─ Node iteration ─ total", 14, total);
    assert_numbers_equals("#Leaves ─ UriList, recursive ─ Different places in tree", 14, get_total_number_of_leaves(tree));

    tree = assert_forward_iteration(tree, "img1.png");
    get_leaf_position(tree, &position, &total);
    assert_numbers_equals("#Leaves ─ Node iteration ─ position", 12, position);
    assert_numbers_equals("#Leaves ─ Node iteration ─ total", 14, total);
    assert_numbers_equals("#Leaves ─ UriList, recursive ─ Different places in tree", 14, get_total_number_of_leaves(tree));

    tree = assert_forward_iteration(tree, "img2.png");
    get_leaf_position(tree, &position, &total);
    assert_numbers_equals("#Leaves ─ Node iteration ─ position", 13, position);
    assert_numbers_equals("#Leaves ─ Node iteration ─ total", 14, total);
    assert_numbers_equals("#Leaves ─ UriList, recursive ─ Different places in tree", 14, get_total_number_of_leaves(tree));

    tree = assert_forward_iteration(tree, "img3.png");
    get_leaf_position(tree, &position, &total);
    assert_numbers_equals("#Leaves ─ Node iteration ─ position", 14, position);
    assert_numbers_equals("#Leaves ─ Node iteration ─ total", 14, total);
    assert_numbers_equals("#Leaves ─ UriList, recursive ─ Different places in tree", 14, get_total_number_of_leaves(tree));

    // Loop from the beginning.
    tree = assert_forward_iteration(tree, ".apa.png");
    get_leaf_position(tree, &position, &total);
    assert_numbers_equals("#Leaves ─ Node iteration ─ position", 1, position);
    assert_numbers_equals("#Leaves ─ Node iteration ─ total", 14, total);
    assert_numbers_equals("#Leaves ─ UriList, recursive ─ Different places in tree", 14, get_total_number_of_leaves(tree));

    tree = assert_forward_iteration(tree, ".depa.gif");
    get_leaf_position(tree, &position, &total);
    assert_numbers_equals("#Leaves ─ Node iteration ─ position", 2, position);
    assert_numbers_equals("#Leaves ─ Node iteration ─ total", 14, total);
    assert_numbers_equals("#Leaves ─ UriList, recursive ─ Different places in tree", 14, get_total_number_of_leaves(tree));

    free_whole_tree(tree);

    after();
}



int main() {
    before_all();

    test_getNumberOfLeaves_NullIn();
    test_getNumberOfLeaves_SingleFileNonRecursive_ReturnSameNumberNoMatterWhereInTheTreeWeAre();
    test_getNumberOfLeaves_UriListRecursive_ReturnSameNumberNoMatterWhereInTheTreeWeAre();

    after_all();
    return errors == 0 ? 0 : -1;
}
