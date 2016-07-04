/* schemes.c
 * ---------
 * plans for implementation of qscripts: including language set, hooks, etc. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "execute.h"
#include "funcs.h"
#include "language.h"
#include "lists.h"
#include "objects.h"
#include "parser.h"
#include "resources.h"
#include "rlinks.h"
#include "stacks.h"
#include "standard.h"
#include "values.h"
#include "variables.h"

#include "schemes.h"

qs_scheme_t *qs_scheme_new (void)
{
   qs_scheme_t *new;

   /* allocate our new scheme. */
   new = malloc (sizeof (qs_scheme_t));
   memset (new, 0, sizeof (qs_scheme_t));
   new->node_list = calloc (1, sizeof (p_node_t *));

   /* attach our standard function set. */
   qs_scheme_funcs (new, qs_func_list_standard);

   /* create some stacks. */
   new->stack_values   = qs_stack_new (qs_value_t);
   new->stack_executes = qs_stack_new (qs_execute_t);

   /* return our new scheme. */
   return new;
}

int qs_scheme_funcs (qs_scheme_t *new, qs_func_t *func_list)
{
   int count = 0, i;
   size_t new_size;

   /* count the number of functions we're going to add. */
   while (func_list[count].name)
      count++;
   if (count == 0)
      return 0;

   /* reallocate function list.  put old functions at the end. */
   new_size = sizeof (qs_func_t *) * (new->func_count + count);
   if (new->funcs == NULL)
      new->funcs = malloc (new_size);
   else {
      new->funcs = realloc (new->funcs, new_size);
      memmove ((void *) new->funcs + sizeof (qs_func_t *) * count,
               (void *) new->funcs,  sizeof (qs_func_t *) * new->func_count);
   }

   /* assign new functions. */
   new->func_count += count;
   for (i = 0; i < count; i++)
      new->funcs[i] = func_list + i;

   /* return the number of new functions. */
   return count;
}

qs_func_t *qs_scheme_get_func (qs_scheme_t *scheme, char *name)
{
   int i;
   for (i = 0; i < scheme->func_count; i++)
      if (strcmp (scheme->funcs[i]->name, name) == 0)
         return scheme->funcs[i];
   return NULL;
}

int qs_scheme_free (qs_scheme_t *scheme)
{
   int i;

   /* free objects. */
   while (scheme->object_list_front)
      qs_object_free (scheme->object_list_front);

   /* free resources. */
   while (scheme->resource_list_front)
      qs_resource_free (scheme->resource_list_front);

   /* free all nodes. */
   for (i = 0; i < scheme->node_count; i++)
      p_node_free (scheme->node_list[i]);
   free (scheme->node_list);

   /* deallocate our heap. */
   if (scheme->stack_values)
      qs_stack_free (scheme->stack_values);
   if (scheme->stack_executes)
      qs_stack_free (scheme->stack_executes);

   /* free our own information. */
   if (scheme->funcs)
      free (scheme->funcs);

   /* free remaining memory and return success. */
   free (scheme);
   return 1;
}

qs_value_t *qs_scheme_heap_value (qs_scheme_t *scheme)
{
   /* push a new value onto our stack. */
   qs_value_t *new = qs_stack_push (scheme->stack_values, qs_scheme_sf_values);
   memset (new, 0, sizeof (qs_value_t));
   new->scheme = scheme;
   new->flags |= QS_VALUE_HEAP;
   qs_value_init (new, QSCRIPT_UNDEFINED, NULL);

   /* return our new value. */
   return new;
}

int qs_scheme_cleanup (qs_scheme_t *scheme)
{
   int count = 0;
   count += qs_stack_empty (scheme->stack_values);
   count += qs_stack_empty (scheme->stack_executes);
   while (scheme->exe_list_back)
      count += qs_execute_pop (scheme->exe_list_back);
   return count;
}

int qs_scheme_update (qs_scheme_t *scheme)
{
   qs_resource_t *r;
   int count = 0;

   /* auto-instantiate resources. */
   for (r = scheme->resource_list_front; r != NULL; r = r->next) {
      if (r->flags & QS_RSRC_LINKED)
         continue;
      r->flags |= QS_RSRC_LINKED;
      if (!r->auto_id && qs_resource_auto_instance (r))
         count++;
   }

   /* return the number of resources instantiated. */
   return count;
}

QS_STACK_FUNC (qs_scheme_sf_values)
   { return qs_value_free (data); }
