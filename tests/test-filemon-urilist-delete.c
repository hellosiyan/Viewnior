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


static void test_filemonitor_uriList_deleteFileUnderRoot() {
    before();

    monitor_test_tree = uri_list(FALSE, FALSE);
    pretty_print_tree(monitor_test_tree, output);

    char* expected = KWHT "<ROOT>" RESET " (3 children)\n\
├─ bepa.png\n\
├─ cepa.jpg\n\
└─┬" KWHT "dir_two" RESET " (3 children)\n\
  ├─ apa.png\n\
  ├─ bepa.png\n\
  └─ cepa.png\n\
";

    assert_equals("File monitor before urilist delete file in root ─ Include hidden files: F ─ Recursive: F", expected, output);



    remove_file(testdir_path, "/bepa.png");

    char* expected_after = KWHT "<ROOT>" RESET " (2 children)\n\
├─ cepa.jpg\n\
└─┬" KWHT "dir_two" RESET " (3 children)\n\
  ├─ apa.png\n\
  ├─ bepa.png\n\
  └─ cepa.png\n\
";

    wait_until_file_system_changes_is_as_expected(1);
    wait_until_tree_is_as_expected(monitor_test_tree, expected_after);

    assert_equals("File monitor after urilist delete file in root ─ Include hidden files: F ─ Recursive: F", expected_after, output);
    assert_file_system_changes_at_least(1);

    after();
}

static void test_filemonitor_uriList_deleteNonWatchedFileUnderRoot() {
    before();

    monitor_test_tree = uri_list(FALSE, FALSE);
    pretty_print_tree(monitor_test_tree, output);

    char* expected = KWHT "<ROOT>" RESET " (3 children)\n\
├─ bepa.png\n\
├─ cepa.jpg\n\
└─┬" KWHT "dir_two" RESET " (3 children)\n\
  ├─ apa.png\n\
  ├─ bepa.png\n\
  └─ cepa.png\n\
";

    assert_equals("File monitor before urilist delete nonwatched file in root ─ Include hidden files: F ─ Recursive: F", expected, output);



    remove_file(testdir_path, "/epa.png");

    char* expected_after = KWHT "<ROOT>" RESET " (3 children)\n\
├─ bepa.png\n\
├─ cepa.jpg\n\
└─┬" KWHT "dir_two" RESET " (3 children)\n\
  ├─ apa.png\n\
  ├─ bepa.png\n\
  └─ cepa.png\n\
";

    wait_until_tree_is_as_expected(monitor_test_tree, expected_after);

    assert_equals("File monitor after urilist delete nonwatched file in root ─ Include hidden files: F ─ Recursive: F", expected_after, output);
    assert_file_system_changes_at_least(0);

    after();
}

static void test_filemonitor_uriList_deleteNonWatchedFileUnderEmptyRoot() {
    before();

    monitor_test_tree = uri_list_with_no_entries(FALSE, FALSE);
    pretty_print_tree(monitor_test_tree, output);

    char* expected = KWHT "<ROOT>" RESET " (0 children)\n\
";

    assert_equals("File monitor before urilist delete file in empty root ─ Include hidden files: F ─ Recursive: F", expected, output);



    remove_file(testdir_path, "/epa.png");

    char* expected_after = KWHT "<ROOT>" RESET " (0 children)\n\
";

    wait_until_tree_is_as_expected(monitor_test_tree, expected_after);

    assert_equals("File monitor after urilist delete file in empty root ─ Include hidden files: F ─ Recursive: F", expected_after, output);
    assert_file_system_changes_at_least(0);

    after();
}

static void test_filemonitor_uriList_deleteFileUnderWatchedDir() {
    before();

    monitor_test_tree = uri_list(FALSE, FALSE);
    pretty_print_tree(monitor_test_tree, output);

    char* expected = KWHT "<ROOT>" RESET " (3 children)\n\
├─ bepa.png\n\
├─ cepa.jpg\n\
└─┬" KWHT "dir_two" RESET " (3 children)\n\
  ├─ apa.png\n\
  ├─ bepa.png\n\
  └─ cepa.png\n\
";

    assert_equals("File monitor before urilist delete file in watched dir ─ Include hidden files: F ─ Recursive: F", expected, output);



    remove_file(testdir_path, "/dir_two/bepa.png");

    char* expected_after = KWHT "<ROOT>" RESET " (3 children)\n\
├─ bepa.png\n\
├─ cepa.jpg\n\
└─┬" KWHT "dir_two" RESET " (2 children)\n\
  ├─ apa.png\n\
  └─ cepa.png\n\
";

    wait_until_tree_is_as_expected(monitor_test_tree, expected_after);

    assert_equals("File monitor after urilist delete file in watched dir ─ Include hidden files: F ─ Recursive: F", expected_after, output);
    assert_file_system_changes_at_least(1);

    after();
}

static void test_filemonitor_uriList_deleteFileUnderNonWatchedDir() {
    before();

    monitor_test_tree = uri_list(FALSE, FALSE);
    pretty_print_tree(monitor_test_tree, output);

    char* expected = KWHT "<ROOT>" RESET " (3 children)\n\
├─ bepa.png\n\
├─ cepa.jpg\n\
└─┬" KWHT "dir_two" RESET " (3 children)\n\
  ├─ apa.png\n\
  ├─ bepa.png\n\
  └─ cepa.png\n\
";

    assert_equals("File monitor before urilist delete file in nonwatched dir ─ Include hidden files: F ─ Recursive: F", expected, output);



    remove_file(testdir_path, "/dir_one/two.jpg");

    char* expected_after = KWHT "<ROOT>" RESET " (3 children)\n\
├─ bepa.png\n\
├─ cepa.jpg\n\
└─┬" KWHT "dir_two" RESET " (3 children)\n\
  ├─ apa.png\n\
  ├─ bepa.png\n\
  └─ cepa.png\n\
";

    wait_until_tree_is_as_expected(monitor_test_tree, expected_after);

    assert_equals("File monitor after urilist delete file in nonwatched dir ─ Include hidden files: F ─ Recursive: F", expected_after, output);
    assert_file_system_changes_at_least(0);

    after();
}



static gboolean file_monitor_tests(gpointer data) {
    before_all();

    test_filemonitor_uriList_deleteFileUnderRoot();
    test_filemonitor_uriList_deleteNonWatchedFileUnderRoot();
    test_filemonitor_uriList_deleteNonWatchedFileUnderEmptyRoot();
    test_filemonitor_uriList_deleteFileUnderWatchedDir();
    test_filemonitor_uriList_deleteFileUnderNonWatchedDir();

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
