/* stacks.c
 * --------
 * dynamic, resizable stack type used in our scheme. */

#include <stdlib.h>
#include <string.h>

#include "stacks.h"

qs_stack_t *qs_stack_new (size_t type_size)
{
   qs_stack_t *new = malloc (sizeof (qs_stack_t));
   memset (new, 0, sizeof (qs_stack_t));
   new->type_size = type_size;
   new->size = 1;
   new->count = 0;
   new->data = malloc (new->size * new->type_size);
   new->func = malloc (new->size * sizeof (qs_stack_func *));
   return new;
}

void *qs_stack_last (qs_stack_t *stack)
   { return qs_stack_last_n (stack, 0); }
void *qs_stack_last_n (qs_stack_t *stack, int n)
{
   if (stack->count - n - 1 < 0)
      return NULL;
   return stack->data[stack->count - n - 1];
}

int qs_stack_push (qs_stack_t *stack, void *data, qs_stack_func *free_func)
{
   if (stack->count == stack->size) {
      stack->size *= 2;
      stack->data = realloc (stack->data, stack->size * stack->type_size);
      stack->func = realloc (stack->func, stack->size *
                             sizeof (qs_stack_func *));
   }
   stack->data[stack->count] = data;
   stack->func[stack->count] = free_func;
   stack->count++;
   return 1;
}

void *qs_stack_get (qs_stack_t *stack)
{
   if (stack->count == 0)
      return NULL;
   return stack->data[stack->count - 1];
}

int qs_stack_pop_get (qs_stack_t *stack, void **data, qs_stack_func **func)
{
   if (stack->count <= 0)
      return 0;
   int i = stack->count - 1;
   if (data) *data = stack->data[i];
   if (func) *func = stack->func[i];
   stack->count--;
   return 1;
}

int qs_stack_pop (qs_stack_t *stack)
{
   if (stack->count <= 0)
      return 0;
   int i = stack->count - 1;
   if (stack->func[i])
      if (!stack->func[i] (stack, stack->data[i]))
         return 0;
   stack->count--;
   return 1;
}

int qs_stack_pop_to (qs_stack_t *stack, void *data)
{
   int count = 0;
   while (stack->count > 0 && stack->data[stack->count - 1] != data)
      count += qs_stack_pop (stack);
   return count;
}

int qs_stack_empty (qs_stack_t *stack)
{
   int count = 0;
   while (stack->count) {
      if (!qs_stack_pop (stack))
         break;
      count++;
   }
   return count;
}

int qs_stack_free (qs_stack_t *stack)
{
   if (stack->s_func)
      stack->s_func (stack, stack->s_data);
   qs_stack_empty (stack);
   free (stack->data);
   free (stack->func);
   free (stack);
   return 1;
}

int qs_stack_data (qs_stack_t *stack, void *data, qs_stack_func *free_func)
{
   /* free any data that was previously there. */
   if (stack->s_func)
      stack->s_func (stack, stack->s_data);
   stack->s_data = data;
   stack->s_func = free_func;
   return 1;
}

int qs_stack_pop_to_except (qs_stack_t *stack, void *to, void *except)
{
   void *pop_data = NULL;
   qs_stack_func *pop_func;
   int count = 0,  i;

   /* pop everything until we reach 'to'.  if we encounter 'except',
    * get its value to push back later. */
   while (stack->count > 0) {
      i = stack->count - 1;
      if (stack->data[i] == to)
         break;
      else if (stack->data[i] == except)
         qs_stack_pop_get (stack, &pop_data, &pop_func);
      else {
         qs_stack_pop (stack);
         count++;
      }
   }

   /* did we find 'except'?  if so, push it back. */
   if (pop_data)
      qs_stack_push (stack, pop_data, pop_func);

   /* return the number of elements popped. */
   return count;
}
