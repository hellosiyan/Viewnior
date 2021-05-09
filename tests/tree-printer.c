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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../src/vnr-tree.h"
#include "tree-printer.h"

char* out;
int output_offset = 0;

static void print_tree(GNode *tree, char* tree_structure_base);


char* create_string(char* base_str, char* append_str) {
    char* new_str = malloc(strlen(base_str) + strlen(append_str) + 1);
    strcpy(new_str, base_str);
    strcat(new_str, append_str);
    return new_str;
}

static char* get_file_name(GNode* node) {
    if(node->data != NULL) {
        VnrFile* vnrfile = node->data;
        return vnrfile->display_name;
    }
    return "<ROOT>";
}

static gboolean is_leaf(GNode *node) {
    VnrFile* vnrfile = node->data;
    return vnrfile != NULL && !vnrfile->is_directory; // A leaf in the tree can represent an empty directory. Otherwise we could do G_NODE_IS_LEAF(node)
}

static void print_node(GNode *node, gpointer data) {
    if(is_leaf(node)) {
        char* append_str = has_more_siblings(node) ? "├─ " : "└─ ";
        char* tree_structure = create_string((char*) data, append_str);

        output_offset += snprintf(out + output_offset, (size_t) (OUTPUTSIZE - output_offset), "%s%s\n", tree_structure, get_file_name(node));

        free(tree_structure);

    } else {
        char* tree_structure = create_string((char*) data, "");
        print_tree(node, tree_structure);
        free(tree_structure);
    }
}

static void print_tree(GNode *tree, char* tree_structure_base) {

    char* tree_structure_end = "";
    if(!G_NODE_IS_ROOT(tree)) {
        if(has_more_siblings(tree)) {
            tree_structure_end = has_children(tree) ? "├─┬" : "├──";
        } else {
            tree_structure_end = has_children(tree) ? "└─┬" : "└──";
        }
    }

    output_offset += snprintf(out + output_offset, (size_t) (OUTPUTSIZE - output_offset), "%s%s" KWHT "%s" RESET " (%i children)\n",
                              tree_structure_base, tree_structure_end, get_file_name(tree), g_node_n_children(tree));

    char* append_str = (G_NODE_IS_ROOT(tree) ? "" : (has_more_siblings(tree) ? "│ " : "  "));
    char* tree_structure = create_string(tree_structure_base, append_str);

    g_node_children_foreach(tree,
                            G_TRAVERSE_ALL,
                            print_node,
                            tree_structure);

    free(tree_structure);
}

void pretty_print_tree(GNode *tree, char* output) {
    out = output;
    output_offset = 0;
    print_tree(get_root_node(tree), "");
}
