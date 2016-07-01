/* execute.c
 * ---------
 * execution of rlinks on objects. */

#ifndef __QS_EXECUTE_H
#define __QS_EXECUTE_H

#include "defs.h"

/* execution states pushed onto schemes. */
struct _qs_execute_t {
   qs_id_t id;
   int flags, type_id;
   char *name;
   qs_action_t *action;
   qs_list_t *list;
   qs_rlink_t *rlink;
   qs_object_t *object;
   qs_execute_t *parent, *prev, *next;
   qs_scheme_t *scheme;

   /* variables. */
   int variable_count;
   qs_variable_t *variable_list_front, *variable_list_back;
};

/* execution functions. */
qs_execute_t *qs_execute_push (int type, qs_rlink_t *rlink, qs_execute_t *exe,
   qs_action_t *action, char *name, int flags, qs_list_t *list);
int qs_execute_pop (qs_execute_t *exe);
qs_execute_t *qs_execute_get_call (qs_execute_t *exe);
qs_execute_t *qs_execute_get_block (qs_execute_t *exe);

#endif
