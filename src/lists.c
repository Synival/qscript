/* lists.c
 * -------
 * list objects assigned to values. */

#include "schemes.h"
#include "values.h"

#include "lists.h"

qs_list_t *qs_list_new (int size)
{
   qs_list_t *new;
   new = malloc (sizeof (qs_list_t));
   memset (new, 0, sizeof (qs_list_t));
   new->value_count = size;
   new->values = malloc (sizeof (qs_value_t *) * ((size > 0) ? size : 1));
   return new;
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

qs_list_t *qs_list_copy (qs_scheme_t *scheme, qs_list_t *list)
{
   /* allocate a new list of equal size. */
   qs_list_t *new = qs_list_new (list->value_count);
   new->scheme = scheme;

   /* create storage space for our data, indicating the values should be
    * freed when this list is freed. */
   new->values_data = malloc (sizeof (qs_value_t *) *
      ((new->value_count > 0) ? new->value_count : 1));

   /* copy values over. */
   int i;
   for (i = 0; i < list->value_count; i++) {
      /* assign to value pointer and value data lists. */
      new->values[i] = malloc (sizeof (qs_value_t));
      new->values_data[i] = new->values[i];
      memset (new->values[i], 0, sizeof (qs_value_t));

      /* initialize. */
      new->values[i]->list = list;
      new->values[i]->flags |= QS_VALUE_MUTABLE;
      qs_value_copy (new->values[i], list->values[i]);
   }

   /* return our new list. */
   return new;
}
