/* variables.h
 * -----------
 * variable management in multiple scopes. */

#ifndef __QS_VARIABLES_H
#define __QS_VARIABLES_H

#include "defs.h"

#include "values.h"

/* variables linked to blocks, objects, or global. */
struct _qs_variable_t {
   char *name;
   qs_value_t value;

   /* link info. */
   int link_id;
   void *parent;
   qs_variable_t *prev, *next;
};

/* function declarations. */
qs_variable_t *qs_variable_get (qs_rlink_t *rlink, char *name, int scope);
qs_variable_t *qs_variable_get_object (qs_object_t *obj, char *name);
qs_variable_t *qs_variable_get_rlink (qs_rlink_t *rlink, char *name);
qs_variable_t *qs_variable_new_base (qs_scheme_t *scheme, char *name);
qs_variable_t *qs_variable_new_object (qs_object_t *obj, char *name);
qs_variable_t *qs_variable_new_rlink (qs_rlink_t *rlink, char *name);
qs_value_t *qs_variable_value (qs_variable_t *variable);
int qs_variable_free (qs_variable_t *v);
int qs_variable_free_after (qs_execute_t *exe, qs_variable_t *v,
                            qs_value_t **return_val);
int qs_variable_contains (qs_variable_t *var, qs_value_t *val);

#endif
