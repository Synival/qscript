/* schemes.h
 * ---------
 * plans for implementation of qscripts: including language set, hooks, etc. */

#ifndef __QS_SCHEMES_H
#define __QS_SCHEMES_H

#include "defs.h"

#include "ids.h"

/* scheme definition. */
struct _qs_scheme_t {
   int func_count;
   qs_func_t **funcs;

   /* some stacks to be regularly cleaned-up. */
   qs_stack_t *stack_values, *stack_executes;
   qs_id_manager_t exe_ids;
   qs_execute_t *exe_list_front, *exe_list_back;
   int exe_count;

   /* files. */
   qs_id_manager_t file_ids;
   qs_file_t *file_list_front, *file_list_back;
   int file_count;

   /* resources. */
   qs_id_manager_t resource_ids;
   qs_resource_t *resource_list_front, *resource_list_back;
   int resource_count;

   /* objects. */
   qs_id_manager_t object_ids;
   qs_object_t *object_list_front, *object_list_back;
   int object_count;

   /* variables. */
   qs_variable_t *variable_list_front, *variable_list_back;
   int variable_count;
};

/* function declarations. */
qs_scheme_t *qs_scheme_new (void);
int qs_scheme_funcs (qs_scheme_t *new, qs_func_t *func_list);
qs_func_t *qs_scheme_get_func (qs_scheme_t *scheme, char *name);
int qs_scheme_free (qs_scheme_t *scheme);
qs_value_t *qs_scheme_heap_value (qs_scheme_t *scheme);
int qs_scheme_cleanup (qs_scheme_t *scheme);
int qs_scheme_update (qs_scheme_t *scheme);
qs_scheme_t *qs_scheme_from_node (p_node_t *node);

/* stack free functions. */
QS_STACK_FUNC (qs_scheme_sf_values);

#endif
