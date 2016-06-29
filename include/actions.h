/* actions.h
 * ---------
 * parsing of parameters during execution. */

#ifndef __QS_ACTIONS_H
#define __QS_ACTIONS_H

#include "defs.h"

/* structure for each individual action. */
struct _qs_action_t {
   qs_value_t *value;
   int type_id, flags;
   void *data_p, *data;
};

/* action functions. */
int qs_action_parameters (qs_execute_t *exe, qs_action_t *action,
   char *func_name, qs_list_t *in, qs_list_t **out);
qs_value_t *qs_action_run (qs_execute_t *exe, qs_value_t *val,
   qs_action_t *action);
qs_value_t *qs_action_call (qs_execute_t *exe, qs_value_t *val,
   qs_action_t *action);
qs_value_t *qs_action_index (qs_execute_t *exe, qs_value_t *val,
   qs_action_t *action);
qs_value_t *qs_action_member (qs_execute_t *exe, qs_value_t *val,
   qs_action_t *action);
int qs_action_free (qs_action_t *a);

#endif
