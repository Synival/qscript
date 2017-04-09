/* parser.c
 * --------
 * generic language parser relying on MPC:
 *    https://github.com/orangeduck/mpc
 *    Daniel Holden - contact@daniel-holden.com
 *    http://www.gameeducationpdx.com/ */

#ifdef __TARGET_QSCRIPT__
   #include "qscript/qs_config.h"
#endif

#include <signal.h>

#include "qscript/parser.h"

int p_language_new (p_symbol_t *symbol_list)
{
   mpc_err_t *err;
   mpc_parser_t **parsers;
   char language[65536];
   size_t len = 0, size = sizeof (language);
   int i, count;

   /* build our language string to pass to mpca_langa(). */
   language[0] = '\0';
   count = 0;
   for (i = 0; symbol_list[i].tag != NULL; i++) {
      len += snprintf (language + len, size - len,
         "%16s : %s;\n", symbol_list[i].tag, symbol_list[i].rule);
      count++;
   }

   /* build our parser array. */
   parsers = malloc (sizeof (mpc_parser_t *) * (count + 1));
   for (i = 0; symbol_list[i].tag != NULL; i++) {
      if (symbol_list[i].parser == NULL)
         symbol_list[i].parser = mpc_new (symbol_list[i].tag);
      parsers[i] = symbol_list[i].parser;
   }
   parsers[count] = NULL;

   /* parse our language and free memory we allocated. */
   err = mpca_langa (MPCA_LANG_DEFAULT, language, parsers);
   free (parsers);

   /* if there's an error, print it and return failure. */
   if (err) {
      mpc_err_print (err);
      mpc_err_delete (err);
      printf ("Language:\n%s", language);
      return 0;
   }

   /* return success! */
   return 1;
}

p_node_t *p_node_new (p_symbol_t *symbols, p_node_t **parent, mpc_ast_t *ast,
   char *tag, char *contents, void *data)
{
   p_node_t *new;
   p_symbol_t *s;

   /* what kind of tag is this? */
   for (s = symbols; s->tag; s++)
      if (strcmp (s->tag, tag) == 0)
         break;
   if (s == NULL)
      s = NULL;

   /* allocate and initialize our new stack type. */
   new = malloc (sizeof (p_node_t));
   memset (new, 0, sizeof (p_node_t));
   new->symbol      = s;
   new->tag         = tag      ? strdup (tag)      : NULL;
   new->contents    = contents ? strdup (contents) : NULL;
   new->parse_data  = data;
   new->row         = ast->state.row + 1;
   new->col         = ast->state.col + 1;
   new->pos         = ast->state.pos;

   /* record type. */
   if (new->symbol)
      new->type_id = new->symbol->type_id;
   else
      new->type_id = -1;

   /* add to the list. */
   if ((new->parent = *parent) == NULL) {
      *parent = new;
      new->prev = NULL;
      new->next = NULL;
   }
   else {
      new->parent->child_count++;
      new->prev = new->parent->last_child;
      new->next = NULL;
      if (new->prev)
         new->prev->next = new;
      else
         new->parent->first_child = new;
      new->parent->last_child = new;
      new->depth = new->parent->depth + 1;
   }

   /* return success. */
   return new;
}

int p_node_free (p_node_t *node)
{
   /* free children. */
   if (node == NULL)
      return 0;
   while (node->first_child)
      p_node_free (node->first_child);

   /* deallocate contents. */
   if (node->tag)      free (node->tag);
   if (node->contents) free (node->contents);

   /* deallocate data. */
   if (node->symbol && node->symbol->free_func)
      node->symbol->free_func (node);
   else if (node->data)
      free (node->data);

   /* remove from the list. */
   if (node->prev) node->prev->next                 = node->next;
   else if (node->parent) node->parent->first_child = node->next;
   if (node->next) node->next->prev                 = node->prev;
   else if (node->parent) node->parent->last_child  = node->prev;
   if (node->parent)
      node->parent->child_count--;

   /* free the node itself. */
   free (node);

   /* return success. */
   return 1;
}

int p_node_print (p_node_t *node)
   { return node ? p_node_print_recurse (node, 0) : 0; }

int p_node_print_recurse (p_node_t *node, int indent)
{
   p_node_t *n;
   int count = 1; /* number of lines printed. */

   /* print our node and display its contents. */
   if (node->contents)
      printf ("%*s[%d %s] = '%s'\n", indent, "", node->type_id,
         node->tag, node->contents);
   else
      printf ("%*s[%d %s]\n", indent, "", node->type_id,
         node->tag);

   /* recurse through children and continue to print. */
   indent += 3;
   for (n = node->first_child; n != NULL; n = n->next)
      count += p_node_print_recurse (n, indent);

   /* print the total number of lines printed. */
   return count;
}

p_node_t *p_node_build (p_symbol_t *symbols, mpc_ast_t *ast, char *top_symbol,
                        void *data)
{
   p_node_t *head = NULL;
   p_node_new (symbols, &head, ast, top_symbol, NULL, data);
   p_node_build_recurse (symbols, &head, ast, NULL, data);
   return head;
}

int p_node_build_recurse (p_symbol_t *symbols, p_node_t **parent,
                          mpc_ast_t *ast, char *tag, void *data)
{
   p_node_t *node = NULL;
   char tag_buf[256], *next;
   int i, result = 0;

   /* if no explicit tag is provided, use whatever is provided by 'ast'. */
   if (tag == NULL)
      tag = ast->tag;

   /* mpc formats tags as "parent|child|child|etc".
    * look for a nested child symbol.  if it exists, create a new node. */
   snprintf (tag_buf, sizeof (tag_buf), "%s", tag);
   if ((next = strchr (tag_buf, '|'))) {
      *next = '\0';
      next++;
      node = p_node_new (symbols, parent, ast, tag_buf, NULL, data);
      result += p_node_build_recurse (symbols, &node, ast, next, data);
      result++;
   }
   /* no child symbols in tag; are there actual child symbols? */
   else if (ast->children_num > 0) {
      for (i = 0; i < ast->children_num; ++i)
         result += p_node_build_recurse (symbols, parent, ast->children[i],
                                         NULL, data);
   }
   /* no children. simply create the node with its contents. */
   else {
      p_node_new (symbols, parent, ast, tag_buf, ast->contents, data);
      result++;
   }

   /* return the number of tokens parsed. */
   return result;
}

p_node_t *p_parse_content (char *file, char *content, p_symbol_t *symbols,
                           p_symbol_t *head, void *data)
{
   p_node_t *new = NULL;
   mpc_result_t r;

   /* attempt to parse. */
   if (mpc_parse (file, content, head->parser, &r)) {
      new = p_node_build (symbols, r.output, head->tag, data);
      mpc_ast_delete (r.output);
   }
   /* print errors if mpc_parse didn't work. */
   else {
      mpc_err_print (r.error);
      mpc_err_delete (r.error);
   }

   /* recursively process all nodes. */
   p_node_process (new);

   /* return our list or NULL if it didn't work. */
   return new;
}

int p_node_process (p_node_t *node)
{
   p_node_t *n;
   int count = 0;

   /* don't process nothing. */
   if (node == NULL)
      return 0;

   /* recurse through all children. */
   for (n = node->first_child; n != NULL; n = n->next)
      count += p_node_process (n);

   /* is there a function to run? */
   if (node->symbol && node->symbol->func) {
      node->symbol->func (node);
      count++;
   }

   /* return total number of functions run. */
   return count;
}

int p_error (p_node_t *node, char *format, ...)
{
   char buf[256];
   va_list va_args;

   /* get our format string. */
   va_start (va_args, format);
   vsnprintf (buf, sizeof (buf), format, va_args);
   va_end (va_args);

   /* print! */
   fflush (stdout);
   if (node)
      fprintf (stderr, "%d,%d: %s", node->row, node->col, buf);
   else
      fprintf (stderr, "%s", buf);
   fflush (stderr);

   /* signals, wow! */
   return 1;
}
