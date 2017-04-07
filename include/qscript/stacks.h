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
   void **data, **pages;
   qs_stack_func **func;
   int page_count;

   /* storage sizes. */
   size_t type_size;
   int size, count;
};

/* functions for stack management. */
#define qs_stack_new(type) \
   qs_stack_new_real (sizeof (type))
qs_stack_t *qs_stack_new_real (size_t type_size);
void *qs_stack_last_n (qs_stack_t *stack, int n);
void *qs_stack_push (qs_stack_t *stack, qs_stack_func *free_func);
int qs_stack_pop (qs_stack_t *stack);
int qs_stack_pop_to (qs_stack_t *stack, void *data);
int qs_stack_empty (qs_stack_t *stack);
int qs_stack_free (qs_stack_t *stack);
int qs_stack_data (qs_stack_t *stack, void *data, qs_stack_func *free_func);
void *qs_stack_pop_get (qs_stack_t *stack, qs_stack_func **func);
int qs_stack_pop_to_except (qs_stack_t *stack, void *to, void *except);

/* inline functions. */
inline void *qs_stack_last (qs_stack_t *stack)
   { return qs_stack_last_n (stack, 0); }

#endif
