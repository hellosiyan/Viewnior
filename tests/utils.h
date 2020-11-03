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

#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../src/vnr-tree.h"
#include "../src/vnr-file.h"
#include "tree-printer.h"

#define TESTDIRNAME "tests"
#define TESTDIRBASE "./"

#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KWHT  "\033[1m\033[37m"
#define RESET "\x1B[0m"
#define OUTPUTSIZE 2048

extern char* testdir_path;
extern int errors;
extern char* output;
extern GNode* monitor_test_tree;

typedef enum {SINGLE_FILE, SINGLE_FOLDER, VALID_LIST, SEMI_INVALID_LIST, COMPLETELY_INVALID_LIST} inputtype;

char* append_strings(char* base_str, char* append_str);
GSList* add_path_to_list(GSList *list, char *parent_dir, char *filename);
char* get_absolute_path(char *dir, char *filename);

GNode* open_single_file(char* path, gboolean include_hidden, gboolean recursive);
GNode* single_file(gboolean include_hidden, gboolean recursive);
GNode* single_folder(gboolean include_hidden, gboolean recursive);
GNode* uri_list(gboolean include_hidden, gboolean recursive);
GNode* uri_list_with_some_invalid_entries(gboolean include_hidden, gboolean recursive);
GNode* uri_list_with_only_invalid_entries(gboolean include_hidden, gboolean recursive);
GNode* get_tree(inputtype type, gboolean include_hidden, gboolean recursive);
char* print_and_free_tree(GNode *tree);
GNode* uri_list_with_no_entries(gboolean include_hidden, gboolean recursive);

void create_dir(char *parent_dir, char *dir_name);
void create_file(char *parent_dir, char *filename);
void create_file_structure();
void remove_file(char* dir, char *filename);
int remove_directory(char *parent_dir, char *path);
void remove_test_directory();

void before_all();
void after_all();
void before();
void after();
void reset_output();
void assert_equals(char* description, char* expected, char* actual);
void assert_numbers_equals(char* description, int expected, int actual);
void assert_trees_equal(char* description, GNode* expected, GNode* actual);
void assert_tree_is_null(char* description, GNode* tree);
void assert_error_is_null(GError* error);
void assert_error_is_not_null(GError* error);
void assert_file_system_changes_at_least(int expected);
void wait_until_tree_is_null();
void wait_until_tree_is_as_expected(GNode* tree, char* expected);
void wait_until_file_system_changes_is_as_expected(int expected);

GNode* assert_forward_iteration(GNode* node, char* expected_file_name);
GNode* assert_backward_iteration(GNode* node, char* expected_file_name);

#endif /* __UTILS_H__ */
