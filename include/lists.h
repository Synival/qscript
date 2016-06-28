/* lists.h
 * -------
 * list objects assigned to values. */

#ifndef __QS_LISTS_H
#define __QS_LISTS_H

#include "defs.h"

/* structure for our lists. */
struct _qs_list_t {
   p_node_t *node;
   qs_value_t *value;
   int type_id, value_count;
   qs_value_t **values, **values_data;
   qs_scheme_t *scheme;
};

/* list functions. */
int qs_list_free (qs_list_t *list);
qs_list_t *qs_list_new (int size);
qs_list_t *qs_list_copy (qs_scheme_t *scheme, qs_list_t *list);

#endif
