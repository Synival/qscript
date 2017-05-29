/* stacks.c
 * --------
 * dynamic, resizable stack type used in our scheme. */

#include <stdlib.h>
#include <string.h>

#include "qscript/stacks.h"

qs_stack_t *qs_stack_new_real (size_t type_size)
{
   /* allocate our stack. */
   qs_stack_t *new = calloc (1, sizeof (qs_stack_t));
   new->type_size = type_size;
   new->size       = 1;
   new->count      = 0;
   new->page_count = 1;

   /* intialize all of our data space. */
   new->pages    = malloc (1 * sizeof (void *));
   new->pages[0] = malloc (new->size * new->type_size);
   new->data     = malloc (new->size * sizeof (void *));
   new->func     = malloc (new->size * sizeof (qs_stack_func *));

   /* point data[] to pages[]. */
   int i;
   for (i = 0; i < new->size; i++)
      new->data[i] = new->pages[0] + (new->type_size * i);

   /* return our new stack. */
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

void *qs_stack_push (qs_stack_t *stack, qs_stack_func *free_func)
{
   int i;

   /* resize our data if need be. */
   if (stack->count == stack->size) {
      /* allocate another page of data. */
      stack->page_count++;
      stack->pages = realloc (stack->pages, stack->page_count
         * sizeof (void *));
      stack->pages[stack->page_count - 1] = malloc (stack->type_size *
         stack->size);

      /* increase our data pointer array. */
      int old_size = stack->size;
      stack->size *= 2;
      stack->data = realloc (stack->data, stack->size * sizeof (void *));
      stack->func = realloc (stack->func, stack->size *
                             sizeof (qs_stack_func *));

      /* point our new data to the page. */
      for (i = 0; i < old_size; i++)
         stack->data[i + old_size] = stack->pages[stack->page_count - 1]
            + (stack->type_size * i);
   }

   /* copy data over. */
   i = stack->count;
   stack->func[i] = free_func;

   /* increase our stack size and return success. */
   stack->count++;
   return stack->data[i];
}

void *qs_stack_pop_get (qs_stack_t *stack, qs_stack_func **func)
{
   if (stack->count <= 0)
      return NULL;
   int i = stack->count - 1;
   if (func) *func = stack->func[i];
   stack->count--;
   return stack->data[i];
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
   int i;
   for (i = 0; i < stack->page_count; i++)
      free (stack->pages[i]);
   free (stack->pages);
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
   int count = 0, i, data_loc;

   /* pop everything until we reach 'to'.  if we encounter 'except',
    * get its value to push back later. */
   while (stack->count > 0) {
      i = stack->count - 1;
      if (stack->data[i] == to)
         break;
      else if (stack->data[i] == except) {
         data_loc = i;
         pop_data = qs_stack_pop_get (stack, &pop_func);
      }
      else {
         qs_stack_pop (stack);
         count++;
      }
   }

   /* did we find 'except'?  if so, swap page data addresses with its old
    * location and the new one so memory doesn't have to be physically moved
    * around.  this will preserve all pointers that point directly to the
    * object on the stack. */
   if (pop_data) {
      stack->data[data_loc] = stack->data[stack->count];
      stack->data[stack->count] = pop_data;
      stack->func[stack->count] = pop_func;
      stack->count++;
   }

   /* return the number of elements popped. */
   return count;
}
