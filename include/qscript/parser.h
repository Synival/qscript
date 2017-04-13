/* parser.h
 * --------
 * generic language parser relying on MPC:
 *    https://github.com/orangeduck/mpc
 *    Daniel Holden - contact@daniel-holden.com
 *    http://www.gameeducationpdx.com/ */

#ifndef __P_PARSER_H
#define __P_PARSER_H

#include <stdint.h>

#include "mpc/mpc.h"
#include "parser_types.h"

/* symbol declarations for parsing and execution. */
struct _p_symbol_t {
   p_id_t type_id;
   char *tag, *rule;
   p_func_t *func, *free_func;
   mpc_parser_t *parser;
};

/* nodes produced by the parser. */
struct _p_node_t {
   p_symbol_t *symbol;
   p_id_t type_id;
   char *tag, *contents;
   void *data, *parse_data;
   p_node_t *parent, *first_child, *last_child;
   p_node_t *prev, *next;
   int child_count, depth;
   char *file;
   int row, col, pos;
};

/* function declarations. */
int p_language_new (p_symbol_t *symbol_list);
p_node_t *p_node_new (p_symbol_t *symbols, p_node_t **parent, mpc_ast_t *ast,
   char *file, char *tag, char *contents, void *data);
int p_node_free (p_node_t *node);
int p_node_print (p_node_t *node);
int p_node_print_recurse (p_node_t *node, int indent);
p_node_t *p_node_build (p_symbol_t *symbols, mpc_ast_t *ast, char *file,
   char *top_symbol, void *data);
int p_node_build_recurse (p_symbol_t *symbols, p_node_t **parent,
   mpc_ast_t *ast, char *file, char *tag, void *data);
p_node_t *p_parse_content (char *filename, char *content, p_symbol_t *symbols,
   p_symbol_t *head, void *data);
int p_node_process (p_node_t *node);
int p_error (p_node_t *node, char *format, ...);

#endif
