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


static void test_singleFile_NonImageFile() {
    before();

    char *path = append_strings(testdir_path, "/test.txt");
    GError *error = NULL;

    assert_tree_is_null("Non image ─ Include hidden files: F ─ Recursive: F", create_tree_from_single_uri(path, FALSE, FALSE, NULL, NULL, &error));
    assert_error_is_null(error);
    g_clear_error(&error);
    assert_tree_is_null("Non image ─ Include hidden files: F ─ Recursive: T", create_tree_from_single_uri(path, FALSE, TRUE,  NULL, NULL, &error));
    assert_error_is_null(error);
    g_clear_error(&error);
    assert_tree_is_null("Non image ─ Include hidden files: T ─ Recursive: F", create_tree_from_single_uri(path, TRUE,  FALSE, NULL, NULL, &error));
    assert_error_is_null(error);
    g_clear_error(&error);
    assert_tree_is_null("Non image ─ Include hidden files: T ─ Recursive: T", create_tree_from_single_uri(path, TRUE,  TRUE,  NULL, NULL, &error));
    assert_error_is_null(error);
    g_clear_error(&error);

    free(path);
    after();
}

static void test_singleFile_NonExistentFile() {
    before();

    char *path = NULL;
    GError *error = NULL;

    assert_tree_is_null("Null file ─ Include hidden files: F ─ Recursive: F", create_tree_from_single_uri(path, FALSE, FALSE, NULL, NULL, &error));
    assert_error_is_null(error);
    g_clear_error(&error);
    assert_tree_is_null("Null file ─ Include hidden files: F ─ Recursive: T", create_tree_from_single_uri(path, FALSE, TRUE,  NULL, NULL, &error));
    assert_error_is_null(error);
    g_clear_error(&error);
    assert_tree_is_null("Null file ─ Include hidden files: T ─ Recursive: F", create_tree_from_single_uri(path, TRUE,  FALSE, NULL, NULL, &error));
    assert_error_is_null(error);
    g_clear_error(&error);
    assert_tree_is_null("Null file ─ Include hidden files: T ─ Recursive: T", create_tree_from_single_uri(path, TRUE,  TRUE,  NULL, NULL, &error));
    assert_error_is_null(error);
    g_clear_error(&error);

    path = "non_existant_file.jpg";
    assert_tree_is_null("Non existant file ─ Include hidden files: F ─ Recursive: F", create_tree_from_single_uri(path, FALSE, FALSE, NULL, NULL, &error));
    assert_error_is_not_null(error);
    g_clear_error(&error);
    assert_tree_is_null("Non existant file ─ Include hidden files: F ─ Recursive: T", create_tree_from_single_uri(path, FALSE, TRUE,  NULL, NULL, &error));
    assert_error_is_not_null(error);
    g_clear_error(&error);
    assert_tree_is_null("Non existant file ─ Include hidden files: T ─ Recursive: F", create_tree_from_single_uri(path, TRUE,  FALSE, NULL, NULL, &error));
    assert_error_is_not_null(error);
    g_clear_error(&error);
    assert_tree_is_null("Non existant file ─ Include hidden files: T ─ Recursive: T", create_tree_from_single_uri(path, TRUE,  TRUE,  NULL, NULL, &error));
    assert_error_is_not_null(error);
    g_error_free(error);

    after();
}

static void test_singleFile_DontIncludeHidden_NotRecursive() {
    before();

    char* expected = KWHT TESTDIRNAME RESET " (3 children)\n\
├─ bepa.png\n\
├─ cepa.jpg\n\
└─ epa.png\n\
";

    GNode *tree = get_tree(SINGLE_FILE, FALSE, FALSE);
    assert_numbers_equals("#Leaves Single file ─ Include hidden files: F ─ Recursive: F", 3, get_total_number_of_leaves(tree));

    assert_equals("Single file ─ Include hidden files: F ─ Recursive: F", expected, print_and_free_tree(tree));

    after();
}

static void test_singleFile_DontIncludeHidden_Recursive() {
    before();

    char* expected = KWHT TESTDIRNAME RESET " (5 children)\n\
├─ bepa.png\n\
├─ cepa.jpg\n\
├─ epa.png\n\
├─┬" KWHT "dir_one" RESET " (1 children)\n\
│ └─ two.jpg\n\
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

    GNode *tree = get_tree(SINGLE_FILE, FALSE, TRUE);
    assert_numbers_equals("#Leaves Single file ─ Include hidden files: F ─ Recursive: T", 14, get_total_number_of_leaves(tree));

    assert_equals("Single file ─ Include hidden files: F ─ Recursive: T", expected, print_and_free_tree(tree));

    after();
}

static void test_singleFile_IncludeHidden_NotRecursive() {
    before();

    char* expected = KWHT TESTDIRNAME RESET " (5 children)\n\
├─ .apa.png\n\
├─ .depa.gif\n\
├─ bepa.png\n\
├─ cepa.jpg\n\
└─ epa.png\n\
";

    GNode *tree = get_tree(SINGLE_FILE, TRUE, FALSE);
    assert_numbers_equals("#Leaves Single file ─ Include hidden files: T ─ Recursive: F", 5, get_total_number_of_leaves(tree));

    assert_equals("Single file ─ Include hidden files: T ─ Recursive: F", expected, print_and_free_tree(tree));

    after();
}

static void test_singleFile_IncludeHidden_Recursive() {
    before();

    char* expected = KWHT TESTDIRNAME RESET " (7 children)\n\
├─ .apa.png\n\
├─ .depa.gif\n\
├─ bepa.png\n\
├─ cepa.jpg\n\
├─ epa.png\n\
├─┬" KWHT "dir_one" RESET " (3 children)\n\
│ ├─ .three.png\n\
│ ├─ two.jpg\n\
│ └─┬" KWHT ".secrets" RESET " (1 children)\n\
│   └─ img.jpg\n\
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

    GNode *tree = get_tree(SINGLE_FILE, TRUE, TRUE);
    assert_numbers_equals("#Leaves Single file ─ Include hidden files: T ─ Recursive: T", 18, get_total_number_of_leaves(tree));

    assert_equals("Single file ─ Include hidden files: T ─ Recursive: T", expected, print_and_free_tree(tree));

    after();
}


static void assert_node_has_path(char* path, gboolean include_hidden, gboolean recursive) {
    GNode *tree = open_single_file(path, include_hidden, recursive);
    assert_equals("Node returned is the one requested ─ Include hidden files: T ─ Recursive: T", path, ((VnrFile*) (tree->data))->path);
    free_whole_tree(tree);
    free(path);
}

static void test_singleFile_nodeHasTheRequestedPath() {
    before();
    char *path;


    path = get_absolute_path(testdir_path, "/cepa.jpg");
    assert_node_has_path(path, FALSE, FALSE);

    path = get_absolute_path(testdir_path, "/epa.png");
    assert_node_has_path(path, FALSE, FALSE);


    path = get_absolute_path(testdir_path, "/cepa.jpg");
    assert_node_has_path(path, FALSE, TRUE);

    path = get_absolute_path(testdir_path, "/epa.png");
    assert_node_has_path(path, FALSE, TRUE);


    path = get_absolute_path(testdir_path, "/.apa.png");
    assert_node_has_path(path, TRUE, FALSE);

    path = get_absolute_path(testdir_path, "/cepa.jpg");
    assert_node_has_path(path, TRUE, FALSE);

    path = get_absolute_path(testdir_path, "/epa.png");
    assert_node_has_path(path, TRUE, FALSE);


    path = get_absolute_path(testdir_path, "/.apa.png");
    assert_node_has_path(path, TRUE, TRUE);

    path = get_absolute_path(testdir_path, "/cepa.jpg");
    assert_node_has_path(path, TRUE, TRUE);

    path = get_absolute_path(testdir_path, "/epa.png");
    assert_node_has_path(path, TRUE, TRUE);


    after();
}



int main() {
    before_all();

    test_singleFile_NonImageFile();
    test_singleFile_NonExistentFile();
    test_singleFile_DontIncludeHidden_NotRecursive();
    test_singleFile_DontIncludeHidden_Recursive();
    test_singleFile_IncludeHidden_NotRecursive();
    test_singleFile_IncludeHidden_Recursive();
    test_singleFile_nodeHasTheRequestedPath();

    after_all();
    return errors == 0 ? 0 : -1;
}
