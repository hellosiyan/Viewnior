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


static void test_uriList_DontIncludeHidden_NotRecursive() {
    before();

    char* expected = KWHT "<ROOT>" RESET " (3 children)\n\
├─ bepa.png\n\
├─ cepa.jpg\n\
└─┬" KWHT "dir_two" RESET " (3 children)\n\
  ├─ apa.png\n\
  ├─ bepa.png\n\
  └─ cepa.png\n\
";
    GNode *tree = get_tree(VALID_LIST, FALSE, FALSE);
    assert_numbers_equals("#Leaves Uri List ─ Include hidden files: F ─ Recursive: F", 5, get_total_number_of_leaves(tree));

    assert_equals("Uri List ─ Include hidden files: F ─ Recursive: F", expected, print_and_free_tree(tree));
    reset_output();


    expected = KWHT "<ROOT>" RESET " (4 children)\n\
├─ bepa.png\n\
├─ cepa.jpg\n\
├─ img.jpg\n\
└─┬" KWHT "dir_two" RESET " (3 children)\n\
  ├─ apa.png\n\
  ├─ bepa.png\n\
  └─ cepa.png\n\
";
    tree = get_tree(SEMI_INVALID_LIST, FALSE, FALSE);
    assert_numbers_equals("#Leaves Uri List ─ Include hidden files: F ─ Recursive: F", 6, get_total_number_of_leaves(tree));

    assert_equals("Uri List ─ Some invalid files. Include hidden files: F ─ Recursive: F", expected, print_and_free_tree(tree));
    reset_output();


    tree = get_tree(COMPLETELY_INVALID_LIST, FALSE, FALSE);
    assert_numbers_equals("#Leaves Uri List ─ Include hidden files: F ─ Recursive: F", 0, get_total_number_of_leaves(tree));

    assert_equals("Uri List ─ Only invalid files. Include hidden files: F ─ Recursive: F", KWHT "<ROOT>" RESET " (0 children)\n", print_and_free_tree(tree));

    after();
}

static void test_uriList_DontIncludeHidden_Recursive() {
    before();

    char* expected = KWHT "<ROOT>" RESET " (3 children)\n\
├─ bepa.png\n\
├─ cepa.jpg\n\
└─┬" KWHT "dir_two" RESET " (7 children)\n\
  ├─ apa.png\n\
  ├─ bepa.png\n\
  ├─ cepa.png\n\
  ├─┬" KWHT "sub_dir_four" RESET " (2 children)\n\
  │ ├──" KWHT "subsub" RESET " (0 children)\n\
  │ └──" KWHT "subsub2" RESET " (0 children)\n\
  ├─┬" KWHT "sub_dir_one" RESET " (3 children)\n\
  │ ├─ img0.png\n\
  │ ├─ img1.png\n\
  │ └─ img2.png\n\
  ├──" KWHT "sub_dir_three" RESET " (0 children)\n\
  └─┬" KWHT "sub_dir_two" RESET " (4 children)\n\
    ├─ img0.png\n\
    ├─ img1.png\n\
    ├─ img2.png\n\
    └─ img3.png\n\
";
    GNode *tree = get_tree(VALID_LIST, FALSE, TRUE);
    assert_numbers_equals("#Leaves Uri List ─ Include hidden files: F ─ Recursive: T", 12, get_total_number_of_leaves(tree));

    assert_equals("Uri List ─ Include hidden files: F ─ Recursive: T", expected, print_and_free_tree(tree));
    reset_output();


    expected = KWHT "<ROOT>" RESET " (4 children)\n\
├─ bepa.png\n\
├─ cepa.jpg\n\
├─ img.jpg\n\
└─┬" KWHT "dir_two" RESET " (7 children)\n\
  ├─ apa.png\n\
  ├─ bepa.png\n\
  ├─ cepa.png\n\
  ├─┬" KWHT "sub_dir_four" RESET " (2 children)\n\
  │ ├──" KWHT "subsub" RESET " (0 children)\n\
  │ └──" KWHT "subsub2" RESET " (0 children)\n\
  ├─┬" KWHT "sub_dir_one" RESET " (3 children)\n\
  │ ├─ img0.png\n\
  │ ├─ img1.png\n\
  │ └─ img2.png\n\
  ├──" KWHT "sub_dir_three" RESET " (0 children)\n\
  └─┬" KWHT "sub_dir_two" RESET " (4 children)\n\
    ├─ img0.png\n\
    ├─ img1.png\n\
    ├─ img2.png\n\
    └─ img3.png\n\
";
    tree = get_tree(SEMI_INVALID_LIST, FALSE, TRUE);
    assert_numbers_equals("#Leaves Uri List ─ Include hidden files: F ─ Recursive: T", 13, get_total_number_of_leaves(tree));

    assert_equals("Uri List ─ Some invalid files. Include hidden files: F ─ Recursive: T", expected, print_and_free_tree(tree));
    reset_output();


    tree = get_tree(COMPLETELY_INVALID_LIST, FALSE, TRUE);
    assert_numbers_equals("#Leaves Uri List ─ Include hidden files: F ─ Recursive: T", 0, get_total_number_of_leaves(tree));

    assert_equals("Uri List ─ Only invalid files. Include hidden files: F ─ Recursive: T", KWHT "<ROOT>" RESET " (0 children)\n", print_and_free_tree(tree));

    after();
}

static void test_uriList_IncludeHidden_NotRecursive() {
    before();

    char* expected = KWHT "<ROOT>" RESET " (5 children)\n\
├─ .apa.png\n\
├─ .depa.gif\n\
├─ bepa.png\n\
├─ cepa.jpg\n\
└─┬" KWHT "dir_two" RESET " (3 children)\n\
  ├─ apa.png\n\
  ├─ bepa.png\n\
  └─ cepa.png\n\
";
    GNode *tree = get_tree(VALID_LIST, TRUE, FALSE);
    assert_numbers_equals("#Leaves Uri List ─ Include hidden files: T ─ Recursive: F", 7, get_total_number_of_leaves(tree));

    assert_equals("Uri List ─ Include hidden files: T ─ Recursive: F", expected, print_and_free_tree(tree));
    reset_output();


    expected = KWHT "<ROOT>" RESET " (6 children)\n\
├─ .apa.png\n\
├─ .depa.gif\n\
├─ bepa.png\n\
├─ cepa.jpg\n\
├─ img.jpg\n\
└─┬" KWHT "dir_two" RESET " (3 children)\n\
  ├─ apa.png\n\
  ├─ bepa.png\n\
  └─ cepa.png\n\
";
    tree = get_tree(SEMI_INVALID_LIST, TRUE, FALSE);
    assert_numbers_equals("#Leaves Uri List ─ Include hidden files: T ─ Recursive: F", 8, get_total_number_of_leaves(tree));

    assert_equals("Uri List ─ Some invalid files. Include hidden files: T ─ Recursive: F", expected, print_and_free_tree(tree));
    reset_output();


    tree = get_tree(COMPLETELY_INVALID_LIST, TRUE, FALSE);
    assert_numbers_equals("#Leaves Uri List ─ Include hidden files: T ─ Recursive: F", 0, get_total_number_of_leaves(tree));

    assert_equals("Uri List ─ Only invalid files. Include hidden files: T ─ Recursive: F", KWHT "<ROOT>" RESET " (0 children)\n", print_and_free_tree(tree));

    after();
}

static void test_uriList_IncludeHidden_Recursive() {
    before();

    char* expected = KWHT "<ROOT>" RESET " (5 children)\n\
├─ .apa.png\n\
├─ .depa.gif\n\
├─ bepa.png\n\
├─ cepa.jpg\n\
└─┬" KWHT "dir_two" RESET " (7 children)\n\
  ├─ apa.png\n\
  ├─ bepa.png\n\
  ├─ cepa.png\n\
  ├─┬" KWHT "sub_dir_four" RESET " (2 children)\n\
  │ ├──" KWHT "subsub" RESET " (0 children)\n\
  │ └──" KWHT "subsub2" RESET " (0 children)\n\
  ├─┬" KWHT "sub_dir_one" RESET " (3 children)\n\
  │ ├─ img0.png\n\
  │ ├─ img1.png\n\
  │ └─ img2.png\n\
  ├──" KWHT "sub_dir_three" RESET " (0 children)\n\
  └─┬" KWHT "sub_dir_two" RESET " (4 children)\n\
    ├─ img0.png\n\
    ├─ img1.png\n\
    ├─ img2.png\n\
    └─ img3.png\n\
";
    GNode *tree = get_tree(VALID_LIST, TRUE, TRUE);
    assert_numbers_equals("#Leaves Uri List ─ Include hidden files: T ─ Recursive: T", 14, get_total_number_of_leaves(tree));

    assert_equals("Uri List ─ Include hidden files: T ─ Recursive: T", expected, print_and_free_tree(tree));
    reset_output();



    expected = KWHT "<ROOT>" RESET " (6 children)\n\
├─ .apa.png\n\
├─ .depa.gif\n\
├─ bepa.png\n\
├─ cepa.jpg\n\
├─ img.jpg\n\
└─┬" KWHT "dir_two" RESET " (7 children)\n\
  ├─ apa.png\n\
  ├─ bepa.png\n\
  ├─ cepa.png\n\
  ├─┬" KWHT "sub_dir_four" RESET " (2 children)\n\
  │ ├──" KWHT "subsub" RESET " (0 children)\n\
  │ └──" KWHT "subsub2" RESET " (0 children)\n\
  ├─┬" KWHT "sub_dir_one" RESET " (3 children)\n\
  │ ├─ img0.png\n\
  │ ├─ img1.png\n\
  │ └─ img2.png\n\
  ├──" KWHT "sub_dir_three" RESET " (0 children)\n\
  └─┬" KWHT "sub_dir_two" RESET " (4 children)\n\
    ├─ img0.png\n\
    ├─ img1.png\n\
    ├─ img2.png\n\
    └─ img3.png\n\
";
    tree = get_tree(SEMI_INVALID_LIST, TRUE, TRUE);
    assert_numbers_equals("#Leaves Uri List ─ Include hidden files: T ─ Recursive: F", 15, get_total_number_of_leaves(tree));

    assert_equals("Uri List ─ Some invalid files. Include hidden files: T ─ Recursive: T", expected, print_and_free_tree(tree));
    reset_output();

    tree = get_tree(COMPLETELY_INVALID_LIST, TRUE, TRUE);
    assert_numbers_equals("#Leaves Uri List ─ Include hidden files: T ─ Recursive: F", 0, get_total_number_of_leaves(tree));

    assert_equals("Uri List ─ Only invalid files. Include hidden files: T ─ Recursive: T", KWHT "<ROOT>" RESET " (0 children)\n", print_and_free_tree(tree));

    after();
}


static void assert_node_has_path(char* path, gboolean include_hidden, gboolean recursive) {
    GNode *tree = get_tree(SEMI_INVALID_LIST, include_hidden, recursive);
    assert_equals("Node returned is the first in folder ─ Include hidden files: T ─ Recursive: T", path, ((VnrFile*) (tree->data))->path);
    free_whole_tree(tree);
}

static void test_uriList_nodeHasTheRequestedPath() {
    before();
    char *path_normal = get_absolute_path(testdir_path, "/bepa.png");
    char *path_hidden = get_absolute_path(testdir_path, "/.apa.png");

    assert_node_has_path(path_normal, FALSE, FALSE);
    assert_node_has_path(path_normal, FALSE, TRUE);
    assert_node_has_path(path_hidden, TRUE, FALSE);
    assert_node_has_path(path_hidden, TRUE, TRUE);

    free(path_normal);
    free(path_hidden);
    after();
}



int main() {
    before_all();

    test_uriList_DontIncludeHidden_NotRecursive();
    test_uriList_DontIncludeHidden_Recursive();
    test_uriList_IncludeHidden_NotRecursive();
    test_uriList_IncludeHidden_Recursive();
    test_uriList_nodeHasTheRequestedPath();

    after_all();
    return errors == 0 ? 0 : -1;
}
