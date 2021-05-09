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

#ifndef __TREE_PRINTER_H__
#define __TREE_PRINTER_H__

#include <glib.h>

#define KWHT  "\033[1m\033[37m"
#define RESET "\x1B[0m"
#define OUTPUTSIZE 2048

char* create_string(char* base_str, char* append_str);
void  pretty_print_tree(GNode *tree, char* out);

#endif /* __TREE_PRINTER_H__ */
