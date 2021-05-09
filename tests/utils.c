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

char* output = NULL;
GNode* monitor_test_tree = NULL;

char *testdir_path;
const int TIMEOUT = 5;
int file_system_changes = 0;
int errors = 0;
gpointer expected_callback_data = (gpointer) 1;

char* append_strings(char* base_str, char* append_str) {
    char* new_str = malloc(strlen(base_str) + strlen(append_str) + 1);
    strcpy(new_str, base_str);
    strcat(new_str, append_str);
    return new_str;
}

GSList* add_path_to_list(GSList *list, char *parent_dir, char *filename) {
    char *path = append_strings(parent_dir, filename);
    list = g_slist_prepend(list, path);
    return list;
}

char* get_absolute_path(char *dir, char *filename) {
    char *absolute_path;
    char *relative_path = append_strings(dir, filename);

    GFile *file = g_file_new_for_path(relative_path);
    absolute_path = g_file_get_path(file);

    free(relative_path);
    g_object_unref(file);
    return absolute_path;
}


void file_system_changed_callback(gboolean deleted, char *path, GNode *changed_node, GNode *root, gpointer data) {
    file_system_changes++;

    if(!deleted && changed_node != NULL) {
        assert_equals("'path' in callback should be equal to that of the changed_node", path, VNR_FILE(changed_node->data)->path);
    }
    assert_numbers_equals("Callback data should be as expected", (int) (intptr_t) expected_callback_data,
                          (int) (intptr_t) data);

    if(deleted && changed_node == root) {
        // The root was deleted.
        monitor_test_tree = NULL;
    }
    if(monitor_test_tree != NULL) {
        monitor_test_tree = get_next_in_tree(root);
    }
}



GNode* open_single_file(char* path, gboolean include_hidden, gboolean recursive) {
    GNode *tree = NULL;
    GError *error = NULL;

    tree = create_tree_from_single_uri(path, include_hidden, recursive, file_system_changed_callback, expected_callback_data, &error);

    assert_error_is_null(error);
    free(error);
    return tree;
}

GNode* single_file(gboolean include_hidden, gboolean recursive) {
    char *path = append_strings(testdir_path, "/bepa.png");
    GNode *tree = open_single_file(path, include_hidden, recursive);
    free(path);
    return tree;
}

GNode* single_folder(gboolean include_hidden, gboolean recursive) {
    GNode *tree = NULL;
    GError *error = NULL;
    char *path = testdir_path;

    tree = create_tree_from_single_uri(path, include_hidden, recursive, file_system_changed_callback, expected_callback_data, &error);

    assert_error_is_null(error);
    free(error);
    return tree;
}

GNode* uri_list(gboolean include_hidden, gboolean recursive) {
    GNode *tree = NULL;
    GError *error = NULL;
    GSList *uri_list = NULL;

    uri_list = add_path_to_list(uri_list, testdir_path, "/.apa.png");
    uri_list = add_path_to_list(uri_list, testdir_path, "/bepa.png");
    uri_list = add_path_to_list(uri_list, testdir_path, "/cepa.jpg");
    uri_list = add_path_to_list(uri_list, testdir_path, "/.depa.gif");
    uri_list = add_path_to_list(uri_list, testdir_path, "/test.txt");
    uri_list = add_path_to_list(uri_list, testdir_path, "/dir_two");


    tree = create_tree_from_uri_list(uri_list, include_hidden, recursive, file_system_changed_callback, expected_callback_data, &error);

    g_slist_free_full(uri_list, free);
    assert_error_is_null(error);
    free(error);
    return tree;
}

GNode* uri_list_with_some_invalid_entries(gboolean include_hidden, gboolean recursive) {
    GNode *tree = NULL;
    GError *error = NULL;
    GSList *uri_list = NULL;

    uri_list = add_path_to_list(uri_list, testdir_path, "/.apa.png");
    uri_list = add_path_to_list(uri_list, testdir_path, "/bepa.png");
    uri_list = add_path_to_list(uri_list, testdir_path, "/NON_EXISTANT_FILE.jpg");
    uri_list = add_path_to_list(uri_list, testdir_path, "/cepa.jpg");
    uri_list = add_path_to_list(uri_list, testdir_path, "/dir_one/.secrets/img.jpg");
    uri_list = add_path_to_list(uri_list, testdir_path, "/.depa.gif");
    uri_list = add_path_to_list(uri_list, testdir_path, "/test.txt");
    uri_list = add_path_to_list(uri_list, testdir_path, "/dir_two");
    uri_list = add_path_to_list(uri_list, testdir_path, "/NON_EXISTANT_DIR");

    tree = create_tree_from_uri_list(uri_list, include_hidden, recursive, file_system_changed_callback, expected_callback_data, &error);

    g_slist_free_full(uri_list, free);
    assert_error_is_null(error);
    free(error);
    return tree;
}

GNode* uri_list_with_only_invalid_entries(gboolean include_hidden, gboolean recursive) {
    GNode *tree = NULL;
    GError *error = NULL;
    GSList *uri_list = NULL;

    uri_list = add_path_to_list(uri_list, testdir_path, "/NON_EXISTANT_FILE.jpg");
    uri_list = add_path_to_list(uri_list, testdir_path, "/NON_EXISTANT_DIR");


    tree = create_tree_from_uri_list(uri_list, include_hidden, recursive, file_system_changed_callback, expected_callback_data, &error);

    g_slist_free_full(uri_list, free);
    assert_error_is_null(error);
    free(error);
    return tree;
}

GNode* uri_list_with_no_entries(gboolean include_hidden, gboolean recursive) {
    GNode *tree = NULL;
    GError *error = NULL;
    GSList *uri_list = NULL;

    tree = create_tree_from_uri_list(uri_list, include_hidden, recursive, file_system_changed_callback, expected_callback_data, &error);

    assert_error_is_null(error);
    free(error);
    return tree;
}

GNode* get_tree(inputtype type, gboolean include_hidden, gboolean recursive) {

    GNode* tree;
    if(type == SINGLE_FILE) {
        tree = single_file(include_hidden, recursive);
    } else if(type == SINGLE_FOLDER) {
        tree = single_folder(include_hidden, recursive);
    } else if(type == SEMI_INVALID_LIST) {
        tree = uri_list_with_some_invalid_entries(include_hidden, recursive);
    } else if(type == COMPLETELY_INVALID_LIST) {
        tree = uri_list_with_only_invalid_entries(include_hidden, recursive);
    } else {
        tree = uri_list(include_hidden, recursive);
    }
    return tree;
}

char* print_and_free_tree(GNode *tree) {
    pretty_print_tree(tree, output);

    free_whole_tree(tree);
    return output;
}



///////////////////////////////


void create_dir(char *parent_dir, char *dir_name) {
    char *path = append_strings(parent_dir, dir_name);
    struct stat st = {0};

    if(stat(path, &st) == -1) {
        mkdir(path, 0700);
    }
    free(path);
}

void create_file(char *parent_dir, char *filename) {
    char *path = append_strings(parent_dir, filename);

    char *dot = strrchr(path, '.');
    if(dot && (!strcmp(dot, ".jpg") || !strcmp(dot, ".png") || !strcmp(dot, ".gif"))) {
        // Write binary image data.

        size_t size = 73;
        int img[size];
        int i = 0;
        img[i++] = 0x89; img[i++] = 0x50; img[i++] = 0x4e; img[i++] = 0x47; img[i++] = 0x0d; img[i++] = 0x0a;
        img[i++] = 0x1a; img[i++] = 0x0a; img[i++] = 0x00; img[i++] = 0x00; img[i++] = 0x00; img[i++] = 0x0d;
        img[i++] = 0x49; img[i++] = 0x48; img[i++] = 0x44; img[i++] = 0x52; img[i++] = 0x00; img[i++] = 0x00;
        img[i++] = 0x00; img[i++] = 0x01; img[i++] = 0x00; img[i++] = 0x00; img[i++] = 0x00; img[i++] = 0x01;
        img[i++] = 0x01; img[i++] = 0x00; img[i++] = 0x00; img[i++] = 0x00; img[i++] = 0x00; img[i++] = 0x37;
        img[i++] = 0x6e; img[i++] = 0xf9; img[i++] = 0x24; img[i++] = 0x00; img[i++] = 0x00; img[i++] = 0x00;
        img[i++] = 0x10; img[i++] = 0x49; img[i++] = 0x44; img[i++] = 0x41; img[i++] = 0x54; img[i++] = 0x78;
        img[i++] = 0x9c; img[i++] = 0x62; img[i++] = 0x60; img[i++] = 0x01; img[i++] = 0x00; img[i++] = 0x00;
        img[i++] = 0x00; img[i++] = 0xff; img[i++] = 0xff; img[i++] = 0x03; img[i++] = 0x00; img[i++] = 0x00;
        img[i++] = 0x06; img[i++] = 0x00; img[i++] = 0x05; img[i++] = 0x57; img[i++] = 0xbf; img[i++] = 0xab;
        img[i++] = 0xd4; img[i++] = 0x00; img[i++] = 0x00; img[i++] = 0x00; img[i++] = 0x00; img[i++] = 0x49;
        img[i++] = 0x45; img[i++] = 0x4e; img[i++] = 0x44; img[i++] = 0xae; img[i++] = 0x42; img[i++] = 0x60;
        img[i]   = 0x82;

        FILE *fh = fopen(path, "wb");
        if(fh != NULL) {
            fwrite(&img, sizeof(img), 1, fh);
            fclose(fh);
        }

    } else {
        // Create empty file
        fclose(fopen(path, "w"));
    }
    free(path);
}

void create_file_structure() {

    create_dir (testdir_path, "");
    create_file(testdir_path, "/test.txt");
    create_file(testdir_path, "/cepa.jpg");
    create_file(testdir_path, "/.apa.png");
    create_file(testdir_path, "/not_an_image.yo");
    create_file(testdir_path, "/.depa.gif");
    create_file(testdir_path, "/bepa.png");
    create_file(testdir_path, "/epa.png");

    create_dir (testdir_path, "/dir_one");
    create_file(testdir_path, "/dir_one/one.txt");
    create_file(testdir_path, "/dir_one/two.jpg");
    create_file(testdir_path, "/dir_one/.three.png");

    create_dir (testdir_path, "/dir_one/.secrets");
    create_file(testdir_path, "/dir_one/.secrets/img.jpg");

    create_dir (testdir_path, "/dir_two");
    create_file(testdir_path, "/dir_two/bepa.png");

    create_dir (testdir_path, "/dir_two/sub_dir_two");
    create_file(testdir_path, "/dir_two/sub_dir_two/img0.png");
    create_file(testdir_path, "/dir_two/sub_dir_two/img2.png");
    create_file(testdir_path, "/dir_two/sub_dir_two/img1.png");
    create_file(testdir_path, "/dir_two/sub_dir_two/img3.png");

    create_dir (testdir_path, "/dir_two/sub_dir_one");
    create_file(testdir_path, "/dir_two/sub_dir_one/img0.png");
    create_file(testdir_path, "/dir_two/sub_dir_one/img2.png");
    create_file(testdir_path, "/dir_two/sub_dir_one/img1.png");

    create_file(testdir_path, "/dir_two/apa.png");
    create_file(testdir_path, "/dir_two/cepa.png");

    create_dir (testdir_path, "/dir_two/sub_dir_three");
    create_dir (testdir_path, "/dir_two/sub_dir_four");
    create_dir (testdir_path, "/dir_two/sub_dir_four/subsub");
    create_dir (testdir_path, "/dir_two/sub_dir_four/subsub2");
}

void remove_file(char *dir, char *filename) {
    char *path = append_strings(dir, filename);
    unlink(path);
    free(path);
}

int remove_directory(char *parent_dir, char *sub_dir) {
    char *path = append_strings(parent_dir, sub_dir);

    DIR *d = opendir(path);
    size_t path_len = strlen(path);
    int r = -1;

    if(d) {
        struct dirent *p;
        r = 0;

        while(!r && (p = readdir(d))) {
            int r2 = -1;
            char *buf;
            size_t len;

            /* Skip the names "." and ".." as we don't want to recurse on them. */
            if(!strcmp(p->d_name, ".") || !strcmp(p->d_name, "..")) {
                continue;
            }

            len = path_len + strlen(p->d_name) + 2;
            buf = malloc(len);

            if(buf) {
                struct stat statbuf;

                snprintf(buf, len, "%s/%s", path, p->d_name);

                if(!stat(buf, &statbuf)) {
                    if(S_ISDIR(statbuf.st_mode)) {
                        r2 = remove_directory(buf, "");
                    } else {
                        r2 = unlink(buf);
                    }
                }

                free(buf);
            }
            r = r2;
        }
        closedir(d);
    }

    if(!r) {
        r = rmdir(path);
    }

    free(path);
    return r;
}

void remove_test_directory() {
    remove_directory(testdir_path, "");
}

static void create_temp_dir() {
    create_dir(TESTDIRBASE, TESTDIRNAME);
    char template[] = TESTDIRBASE TESTDIRNAME "/XXXXXX";
    char *dir_name = mkdtemp(template);
    testdir_path = append_strings(dir_name, "/" TESTDIRNAME);
}

static void remove_temp_dir() {
    char parent[strlen(testdir_path)];
    const char *last_slash = strrchr(testdir_path, '/');
    if(last_slash) {
        strncpy(parent, testdir_path, last_slash - testdir_path);
        parent[last_slash - testdir_path] = '\0';

        remove_directory(parent, "");
    }
    free(testdir_path);
}



///////////////////////////////



void before_all() {
    create_temp_dir();
}

void after_all() {
    remove_temp_dir();
}

void before() {
    output = (char*) malloc(sizeof(char) * OUTPUTSIZE);
    file_system_changes = 0;
    create_file_structure();
}

void after() {
    if(output != NULL) {
        free(output);
    }
    output = NULL;
    if(monitor_test_tree != NULL) {
        free_whole_tree(monitor_test_tree);
        monitor_test_tree = NULL;
    }
    remove_test_directory();
}

void reset_output() {
    if(output != NULL) {
        free(output);
    }
    output = (char*) malloc(sizeof(char) * OUTPUTSIZE);
}

void assert_equals(char* description, char* expected, char* actual) {
    if(strcmp(expected, actual) == 0) {
        printf(KGRN "[PASS]  %s\n" RESET, description);
    } else {
        printf("\n");
        printf(KRED "[FAIL]  %s\n" RESET, description);
        printf("Expected:\n'%s'\n\nActual:\n'%s'\n\n", expected, actual);
        fprintf(stderr, "* %s\n", description);
        errors++;
    }
}

void assert_numbers_equals(char* description, int expected, int actual) {
    if(expected == actual) {
        printf(KGRN "[PASS]  %s\n" RESET, description);
    } else {
        printf("\n");
        printf(KRED "[FAIL]  %s\n" RESET, description);
        printf("Expected: %i,   Actual: %i\n\n", expected, actual);
        fprintf(stderr, "* %s\n", description);
        errors++;
    }
}

void assert_trees_equal(char* description, GNode* expected, GNode* actual) {
    char *expectedoutput = (char*) malloc(sizeof(char) * OUTPUTSIZE);
    char *actualoutput = (char*) malloc(sizeof(char) * OUTPUTSIZE);

    pretty_print_tree(expected, expectedoutput);
    pretty_print_tree(actual, actualoutput);

    assert_equals(description, expectedoutput, actualoutput);

    free(expectedoutput);
    free(actualoutput);
}

void assert_tree_is_null(char* description, GNode* tree) {
    if(tree == NULL) {
        printf(KGRN "[PASS]  %s\n" RESET, description);
    } else {
        printf("\n");
        printf(KRED "[FAIL]  %s\n" RESET, description);
        fprintf(stderr, "* %s\n", description);
        errors++;
    }
}

void assert_error_is_null(GError* error) {
    if(error != NULL) {
        printf(KRED "[FAIL]  Error should not be set! Was: %s\n" RESET, error->message);
        fprintf(stderr, "* Error should not be set! Was: %s\n", error->message);
        errors++;
    }
}

void assert_error_is_not_null(GError* error) {
    if(error == NULL) {
        printf(KRED "[FAIL]  Error should be set!\n" RESET);
        fprintf(stderr, "* Error should be set!\n");
        errors++;
    }
}

// Due to the non-deterministic way that file monitors are triggered, there could be several triggers for
// one action, if e.g. a directory and its root both have filemonitors, and the directory is deleted.
// Test the minimum amount of expected file system changes, and let the underlying file system raise as
// many triggers as it needs.
void assert_file_system_changes_at_least(int expected) {
    if(file_system_changes < expected) {
        printf("\n");
        printf(KRED "[FAIL]  %s\n" RESET, "Mismatch in number of file system changes");
        printf("Expected: %i,   Actual: %i\n\n", expected, file_system_changes);
        errors++;
    }
}

void wait_until_tree_is_null() {
    clock_t start = clock();
    while((clock() - start) / CLOCKS_PER_SEC < TIMEOUT) {

        g_main_context_iteration(NULL, FALSE);

        if(monitor_test_tree == NULL) {
            break;
        }
    }
}

void wait_until_file_system_changes_is_as_expected(int expected) {
    clock_t start = clock();
    while((clock() - start) / CLOCKS_PER_SEC < TIMEOUT) {

        g_main_context_iteration(NULL, FALSE);

        if(file_system_changes == expected) {
            break;
        }
    }
}

void wait_until_tree_is_as_expected(GNode* tree, char* expected) {
    clock_t start = clock();
    while((clock() - start) / CLOCKS_PER_SEC < TIMEOUT) {

        reset_output();
        pretty_print_tree(tree, output);
        g_main_context_iteration(NULL, FALSE);

        if(strcmp(expected, output) == 0) {
            break;
        }
    }
}



static void assert_iteration(char* description_base, GNode* node, char* expected_file_name) {
    VnrFile* vnrfile = node->data;

    char* description = create_string(description_base, expected_file_name);
    assert_equals(description, expected_file_name, vnrfile->display_name);

    free(description);
}

GNode* assert_forward_iteration(GNode* node, char* expected_file_name) {
    GNode *next = get_next_in_tree(node);
    assert_iteration("Get Next ─ Iterating ─ ", next, expected_file_name);
    return next;
}

GNode* assert_backward_iteration(GNode* node, char* expected_file_name) {
    GNode *prev = get_prev_in_tree(node);
    assert_iteration("Get Prev ─ Iterating ─ ", prev, expected_file_name);
    return prev;
}
