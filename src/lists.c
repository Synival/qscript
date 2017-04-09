/* lists.c
 * -------
 * list objects assigned to values. */

#include <stdlib.h>
#include <string.h>

#include "qscript/parser.h"
#include "qscript/schemes.h"
#include "qscript/values.h"

#include "qscript/lists.h"

qs_list_t *qs_list_new (qs_scheme_t *scheme, int size)
{
   qs_list_t *new;
   int mem_size = (size > 0) ? size : 1;

   /* allocate our list. */
   new = malloc (sizeof (qs_list_t));
   memset (new, 0, sizeof (qs_list_t));
   new->scheme = scheme;
   new->value_count = size;

   /* populate it with blank values. */
   new->values      = malloc (sizeof (qs_value_t *) * mem_size);
   new->values_data = malloc (sizeof (qs_value_t *) * mem_size);
   memset (new->values,      0, sizeof (qs_value_t *) * mem_size);
   memset (new->values_data, 0, sizeof (qs_value_t *) * mem_size);

   /* return the new list. */
   return new;
}

int qs_list_internalize (qs_list_t *list)
{
   int i, count = 0;
   for (i = 0; i < list->value_count; i++) {
      /* if the data is already set properly, do nothing. */
      if (list->values_data[i] == list->values[i])
         continue;

      /* otherwise, free internalized data. */
      if (list->values_data[i])
         qs_value_free (list->values_data[i]);

      /* allocate our internal data... */
      list->values_data[i] = malloc (sizeof (qs_value_t));
      memset (list->values_data[i], 0, sizeof (qs_value_t));
      list->values_data[i]->scheme = list->scheme;

      /* ...and copy external data to it.  use that instead from now on. */
      qs_value_copy_const (NULL, list->values_data[i], list->values[i]);
      list->values_data[i]->flags |= QS_VALUE_MUTABLE;
      list->values[i] = list->values_data[i];
      count++;
   }

   /* return the number of elements interanlized. */
   return count;
}

int qs_list_free (qs_list_t *list)
{
   if (list->node)
      list->node->data = NULL;
   if (list->values)
      free (list->values);
   if (list->values_data) {
      int i;
      for (i = 0; i < list->value_count; i++)
         if (list->values_data[i])
            qs_value_free (list->values_data[i]);
      free (list->values_data);
   }
   free (list);
   return 1;
}

qs_list_t *qs_list_copy (qs_list_t *list)
{
   /* allocate a new list of equal size. */
   qs_list_t *new = qs_list_new (list->scheme, list->value_count);

   /* copy values over. */
   int i;
   for (i = 0; i < list->value_count; i++)
      new->values[i] = list->values[i];
   qs_list_internalize (new);

   /* return our new list. */
   return new;
}

qs_value_t *qs_list_value (qs_execute_t *exe, qs_list_t *list, int index)
{
   if (index < 0)
      return NULL;
   if (index >= list->value_count)
      return NULL;
   return qs_value_read (exe, list->values[index]);
}
