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


static void test_filemonitor_uriList_createFileInRoot() {
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

    assert_equals("File monitor before urilist create file in root ─ Include hidden files: F ─ Recursive: F", expected, output);


    create_file(testdir_path, "/apa.png");

    char* expected_after = KWHT "<ROOT>" RESET " (3 children)\n\
├─ bepa.png\n\
├─ cepa.jpg\n\
└─┬" KWHT "dir_two" RESET " (3 children)\n\
  ├─ apa.png\n\
  ├─ bepa.png\n\
  └─ cepa.png\n\
";

    wait_until_tree_is_as_expected(monitor_test_tree, expected_after);

    assert_equals("File monitor after urilist create file in root ─ Include hidden files: F ─ Recursive: F", expected_after, output);
    assert_file_system_changes_at_least(0);

    after();
}


static void test_filemonitor_uriList_createDirInRoot() {
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

    assert_equals("File monitor before urilist create dir in root ─ Include hidden files: F ─ Recursive: F", expected, output);



    create_dir (testdir_path, "/dir_three");
    create_file(testdir_path, "/dir_three/apa.png");
    create_file(testdir_path, "/dir_three/bepa.png");

    char* expected_after = KWHT "<ROOT>" RESET " (3 children)\n\
├─ bepa.png\n\
├─ cepa.jpg\n\
└─┬" KWHT "dir_two" RESET " (3 children)\n\
  ├─ apa.png\n\
  ├─ bepa.png\n\
  └─ cepa.png\n\
";

    wait_until_tree_is_as_expected(monitor_test_tree, expected_after);

    assert_equals("File monitor after urilist create dir in root ─ Include hidden files: F ─ Recursive: F", expected_after, output);
    assert_file_system_changes_at_least(0);

    after();
}

static void test_filemonitor_uriList_createFileInWatchedDir() {
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

    assert_equals("File monitor before urilist create file in watched dir ─ Include hidden files: F ─ Recursive: F", expected, output);



    create_file(testdir_path, "/dir_two/depa.png");

    char* expected_after = KWHT "<ROOT>" RESET " (3 children)\n\
├─ bepa.png\n\
├─ cepa.jpg\n\
└─┬" KWHT "dir_two" RESET " (4 children)\n\
  ├─ apa.png\n\
  ├─ bepa.png\n\
  ├─ cepa.png\n\
  └─ depa.png\n\
";

    wait_until_tree_is_as_expected(monitor_test_tree, expected_after);

    assert_equals("File monitor after urilist create file in watched dir ─ Include hidden files: F ─ Recursive: F", expected_after, output);
    assert_file_system_changes_at_least(1);

    after();
}

static void test_filemonitor_uriList_createFileInNonwatchedDir() {
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

    assert_equals("File monitor before urilist create file in nonwatched dir ─ Include hidden files: F ─ Recursive: F", expected, output);



    create_file(testdir_path, "/dir_one/depa.png");

    char* expected_after = KWHT "<ROOT>" RESET " (3 children)\n\
├─ bepa.png\n\
├─ cepa.jpg\n\
└─┬" KWHT "dir_two" RESET " (3 children)\n\
  ├─ apa.png\n\
  ├─ bepa.png\n\
  └─ cepa.png\n\
";

    wait_until_tree_is_as_expected(monitor_test_tree, expected_after);

    assert_equals("File monitor after urilist create file in nonwatched dir ─ Include hidden files: F ─ Recursive: F", expected_after, output);
    assert_file_system_changes_at_least(0);

    after();
}

static void test_filemonitor_uriList_createFileInEmptyRoot() {
    before();

    monitor_test_tree = uri_list_with_no_entries(FALSE, FALSE);
    pretty_print_tree(monitor_test_tree, output);

    char* expected = KWHT "<ROOT>" RESET " (0 children)\n\
";

    assert_equals("File monitor before urilist create file in empty dir ─ Include hidden files: F ─ Recursive: F", expected, output);



    create_file(testdir_path, "/depa.png");

    char* expected_after = KWHT "<ROOT>" RESET " (0 children)\n\
";

    wait_until_tree_is_as_expected(monitor_test_tree, expected_after);

    assert_equals("File monitor after urilist create file in empty dir ─ Include hidden files: F ─ Recursive: F", expected_after, output);
    assert_file_system_changes_at_least(0);

    after();
}



static gboolean file_monitor_tests(gpointer data) {
    before_all();

    test_filemonitor_uriList_createFileInRoot();
    test_filemonitor_uriList_createDirInRoot();
    test_filemonitor_uriList_createFileInWatchedDir();
    test_filemonitor_uriList_createFileInNonwatchedDir();
    test_filemonitor_uriList_createFileInEmptyRoot();

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
