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


static void test_filemonitor_deleteFileInFolderRoot_noCallbackFunction() {
    before();

    GError *error = NULL;
    monitor_test_tree = create_tree_from_single_uri(testdir_path, TRUE, FALSE, NULL, NULL, &error);
    assert_error_is_null(error);
    free(error);

    pretty_print_tree(monitor_test_tree, output);

    char* expected = KWHT TESTDIRNAME RESET " (5 children)\n\
├─ .apa.png\n\
├─ .depa.gif\n\
├─ bepa.png\n\
├─ cepa.jpg\n\
└─ epa.png\n\
";

    assert_equals("File monitor before delete in dirroot with no callback ─ Include hidden files: T ─ Recursive: F", expected, output);


    remove_file(testdir_path, "/bepa.png");

    char* expected_after = KWHT TESTDIRNAME RESET " (4 children)\n\
├─ .apa.png\n\
├─ .depa.gif\n\
├─ cepa.jpg\n\
└─ epa.png\n\
";

    wait_until_tree_is_as_expected(monitor_test_tree, expected_after);

    assert_equals("File monitor after delete in dirroot with no callback ─ Include hidden files: T ─ Recursive: F", expected_after, output);
    assert_file_system_changes_at_least(0);

    after();
}

static void test_filemonitor_deleteFileInFolderRoot() {
    before();

    monitor_test_tree = single_folder(TRUE, FALSE);
    pretty_print_tree(monitor_test_tree, output);

    char* expected = KWHT TESTDIRNAME RESET " (5 children)\n\
├─ .apa.png\n\
├─ .depa.gif\n\
├─ bepa.png\n\
├─ cepa.jpg\n\
└─ epa.png\n\
";

    assert_equals("File monitor before delete in dirroot ─ Include hidden files: T ─ Recursive: F", expected, output);


    remove_file(testdir_path, "/bepa.png");

    char* expected_after = KWHT TESTDIRNAME RESET " (4 children)\n\
├─ .apa.png\n\
├─ .depa.gif\n\
├─ cepa.jpg\n\
└─ epa.png\n\
";

    wait_until_tree_is_as_expected(monitor_test_tree, expected_after);

    assert_equals("File monitor after delete in dirroot ─ Include hidden files: T ─ Recursive: F", expected_after, output);
    assert_file_system_changes_at_least(1);

    after();
}

static void test_filemonitor_deleteHiddenFileInFolderRoot() {
    before();

    monitor_test_tree = single_folder(TRUE, FALSE);
    pretty_print_tree(monitor_test_tree, output);

    char* expected = KWHT TESTDIRNAME RESET " (5 children)\n\
├─ .apa.png\n\
├─ .depa.gif\n\
├─ bepa.png\n\
├─ cepa.jpg\n\
└─ epa.png\n\
";

    assert_equals("File monitor before delete hidden ─ Include hidden files: T ─ Recursive: F", expected, output);


    remove_file(testdir_path, "/.apa.png");

    char* expected_after = KWHT TESTDIRNAME RESET " (4 children)\n\
├─ .depa.gif\n\
├─ bepa.png\n\
├─ cepa.jpg\n\
└─ epa.png\n\
";

    wait_until_file_system_changes_is_as_expected(1);
    wait_until_tree_is_as_expected(monitor_test_tree, expected_after);

    assert_equals("File monitor after delete hidden ─ Include hidden files: T ─ Recursive: F", expected_after, output);
    assert_file_system_changes_at_least(1);

    after();
}

static void test_filemonitor_deleteFileInSubFolder() {
    before();

    monitor_test_tree = single_folder(FALSE, TRUE);
    pretty_print_tree(monitor_test_tree, output);

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

    assert_equals("File monitor before delete in subdir ─ Include hidden files: F ─ Recursive: T", expected, output);


    remove_file(testdir_path, "/dir_two/sub_dir_one/img1.png");

    char* expected_after = KWHT TESTDIRNAME RESET " (5 children)\n\
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
  ├─┬" KWHT "sub_dir_one" RESET " (2 children)\n\
  │ ├─ img0.png\n\
  │ └─ img2.png\n\
  ├──" KWHT "sub_dir_three" RESET " (0 children)\n\
  └─┬" KWHT "sub_dir_two" RESET " (4 children)\n\
    ├─ img0.png\n\
    ├─ img1.png\n\
    ├─ img2.png\n\
    └─ img3.png\n\
";

    wait_until_tree_is_as_expected(monitor_test_tree, expected_after);

    assert_equals("File monitor after delete in subdir ─ Include hidden files: F ─ Recursive: T", expected_after, output);
    assert_file_system_changes_at_least(1);

    after();
}

static void test_filemonitor_deleteMultipleFiles() {
    before();

    monitor_test_tree = single_folder(FALSE, TRUE);
    pretty_print_tree(monitor_test_tree, output);

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

    assert_equals("File monitor before delete in subdir ─ Include hidden files: F ─ Recursive: T", expected, output);


    remove_file(testdir_path, "/dir_two/sub_dir_one/img1.png");
    remove_file(testdir_path, "/dir_two/bepa.png");
    remove_file(testdir_path, "/dir_two/sub_dir_two/img2.png");

    char* expected_after = KWHT TESTDIRNAME RESET " (5 children)\n\
├─ bepa.png\n\
├─ cepa.jpg\n\
├─ epa.png\n\
├─┬" KWHT "dir_one" RESET " (1 children)\n\
│ └─ two.jpg\n\
└─┬" KWHT "dir_two" RESET " (6 children)\n\
  ├─ apa.png\n\
  ├─ cepa.png\n\
  ├─┬" KWHT "sub_dir_four" RESET " (2 children)\n\
  │ ├──" KWHT "subsub" RESET " (0 children)\n\
  │ └──" KWHT "subsub2" RESET " (0 children)\n\
  ├─┬" KWHT "sub_dir_one" RESET " (2 children)\n\
  │ ├─ img0.png\n\
  │ └─ img2.png\n\
  ├──" KWHT "sub_dir_three" RESET " (0 children)\n\
  └─┬" KWHT "sub_dir_two" RESET " (3 children)\n\
    ├─ img0.png\n\
    ├─ img1.png\n\
    └─ img3.png\n\
";

    wait_until_tree_is_as_expected(monitor_test_tree, expected_after);

    assert_equals("File monitor after delete in subdir ─ Include hidden files: F ─ Recursive: T", expected_after, output);
    assert_file_system_changes_at_least(3);

    after();
}

static void test_filemonitor_deleteSubsubFolder() {
    before();

    monitor_test_tree = single_folder(FALSE, TRUE);
    pretty_print_tree(monitor_test_tree, output);

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

    assert_equals("File monitor before delete subsubdir ─ Include hidden files: F ─ Recursive: T", expected, output);


    remove_directory(testdir_path, "/dir_two/sub_dir_one/");

    char* expected_after = KWHT TESTDIRNAME RESET " (5 children)\n\
├─ bepa.png\n\
├─ cepa.jpg\n\
├─ epa.png\n\
├─┬" KWHT "dir_one" RESET " (1 children)\n\
│ └─ two.jpg\n\
└─┬" KWHT "dir_two" RESET " (6 children)\n\
  ├─ apa.png\n\
  ├─ bepa.png\n\
  ├─ cepa.png\n\
  ├─┬" KWHT "sub_dir_four" RESET " (2 children)\n\
  │ ├──" KWHT "subsub" RESET " (0 children)\n\
  │ └──" KWHT "subsub2" RESET " (0 children)\n\
  ├──" KWHT "sub_dir_three" RESET " (0 children)\n\
  └─┬" KWHT "sub_dir_two" RESET " (4 children)\n\
    ├─ img0.png\n\
    ├─ img1.png\n\
    ├─ img2.png\n\
    └─ img3.png\n\
";

    wait_until_file_system_changes_is_as_expected(5);
    wait_until_tree_is_as_expected(monitor_test_tree, expected_after);


    assert_equals("File monitor after delete subsubdir ─ Include hidden files: F ─ Recursive: T", expected_after, output);
    assert_file_system_changes_at_least(4);

    after();
}

static void test_filemonitor_deleteSubFolder() {
    before();

    monitor_test_tree = single_folder(FALSE, TRUE);
    pretty_print_tree(monitor_test_tree, output);

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

    assert_equals("File monitor before delete subdir ─ Include hidden files: F ─ Recursive: T", expected, output);


    remove_directory(testdir_path, "/dir_two/");

    char* expected_after = KWHT TESTDIRNAME RESET " (4 children)\n\
├─ bepa.png\n\
├─ cepa.jpg\n\
├─ epa.png\n\
└─┬" KWHT "dir_one" RESET " (1 children)\n\
  └─ two.jpg\n\
";

    wait_until_tree_is_as_expected(monitor_test_tree, expected_after);


    assert_equals("File monitor after delete subdir ─ Include hidden files: F ─ Recursive: T", expected_after, output);
    assert_file_system_changes_at_least(16);

    after();
}

static void test_filemonitor_deleteRootFolder() {
    before();

    monitor_test_tree = single_folder(FALSE, TRUE);
    pretty_print_tree(monitor_test_tree, output);

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

    assert_equals("File monitor before delete root ─ Include hidden files: F ─ Recursive: T", expected, output);


    remove_directory(testdir_path, "");


    wait_until_tree_is_null();

    assert_tree_is_null("File monitor after delete root ─ Include hidden files: F ─ Recursive: T", monitor_test_tree);
    assert_file_system_changes_at_least(22);

    after();
}



static gboolean file_monitor_tests(gpointer data) {
    before_all();

    test_filemonitor_deleteFileInFolderRoot_noCallbackFunction();
    test_filemonitor_deleteFileInFolderRoot();
    test_filemonitor_deleteHiddenFileInFolderRoot();
    test_filemonitor_deleteFileInSubFolder();
    test_filemonitor_deleteMultipleFiles();
    test_filemonitor_deleteSubsubFolder();
    test_filemonitor_deleteSubFolder();
    test_filemonitor_deleteRootFolder();

    after_all();
    g_main_loop_quit((GMainLoop*)data);
    return FALSE;
}

int main() {
    GMainLoop* loop = g_main_loop_new(NULL, FALSE);
    g_timeout_add(1000, file_monitor_tests, loop);
    g_main_loop_run(loop);
    g_main_loop_unref(loop);

    return errors == 0 ? 0 : -1;
}
