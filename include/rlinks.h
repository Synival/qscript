/* rlinks.h
 * --------
 * resource linking with objects. */

#ifndef __QS_RLINK_H
#define __QS_RLINK_H

#include "defs.h"

/* structure for each link. */
struct _qs_rlink_t {
   /* identity stuff. */
   qs_scheme_t *scheme;
   qs_resource_t *resource;
   int flags;

   /* modificiations applied to properties. */
   qs_stack_t *stack_modify;

   /* top rlink-scope variables. */
   qs_variable_t *variable_list_front, *variable_list_back;
   int variable_count;

   /* link information. */
   int priority;
   qs_object_t *object;
   qs_rlink_t *parent;
   qs_rlink_t *child_front, *child_back;
   qs_rlink_t *prev, *next, *rsrc_prev, *rsrc_next;
   qs_action_t *parent_action;
   qs_execute_t *parent_exe;
};

/* function declarations. */
qs_rlink_t *qs_rlink_push (qs_object_t *obj, qs_resource_t *resource,
   int priority);
qs_rlink_t *qs_rlink_push_at (qs_object_t *obj, qs_resource_t *resource,
   int priority, qs_rlink_t *parent, qs_rlink_t *prev, qs_execute_t *exe,
   qs_action_t *action);
qs_value_t *qs_rlink_wind (qs_rlink_t *rlink, qs_execute_t *exe);
int qs_rlink_unwind (qs_rlink_t *rlink);
int qs_rlink_pop (qs_rlink_t *rlink);
int qs_rlink_is_child_of (qs_rlink_t *child, qs_rlink_t *parent);

#endif
