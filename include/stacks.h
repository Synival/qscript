/* stacks.h
 * --------
 * dynamic, resizable stack type used in our scheme. */

#ifndef __QS_STACKS_H
#define __QS_STACKS_H

#include <stdio.h>

#include "defs.h"

/* types. */
struct _qs_stack_t {
   /* data assigned to our stack itself. */
   void *s_data;
   qs_stack_func *s_func;

   /* globs of data. */
   void **data;
   qs_stack_func **func;

   /* storage sizes. */
   size_t type_size;
   int size, count;
};

/* functions for stack management. */
qs_stack_t *qs_stack_new (size_t type_size);
void *qs_stack_last (qs_stack_t *stack);
void *qs_stack_last_n (qs_stack_t *stack, int n);
int qs_stack_push (qs_stack_t *stack, void *data, qs_stack_func *free_func);
int qs_stack_pop (qs_stack_t *stack);
int qs_stack_pop_to (qs_stack_t *stack, void *data);
int qs_stack_empty (qs_stack_t *stack);
int qs_stack_free (qs_stack_t *stack);
int qs_stack_data (qs_stack_t *stack, void *data, qs_stack_func *free_func);
int qs_stack_pop_get (qs_stack_t *stack, void **data, qs_stack_func **func);
void *qs_stack_get (qs_stack_t *stack);
int qs_stack_pop_to_except (qs_stack_t *stack, void *to, void *except);

#endif
