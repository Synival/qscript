/* values.h
 * --------
 * useful functions for value types. */

#ifndef __QS_VALUES_H
#define __QS_VALUES_H

#include "defs.h"

/* structure for each value. */
struct _qs_value_t {
   /* identification. */
   p_id_t type_id;

   /* stored values. */
   char *val_s;
   int32_t val_i;
   float val_f;
   void *val_p;
   void *data;

   /* extra value. */
   int flags;

   /* link info. */
   int link_id;
   void *link;
   qs_list_t *list;
   qs_value_t *parent, *child;
   p_node_t *node;
   qs_scheme_t *scheme;
};

/* value functions. */
int qs_value_copy (qs_value_t *dest, qs_value_t *src);
char *qs_value_type (qs_value_t *val);
qs_variable_t *qs_value_get_variable (qs_rlink_t *rlink, qs_value_t *v);
qs_property_t *qs_value_get_property (qs_rlink_t *rlink, qs_value_t *v);
int qs_value_cleanup (qs_value_t *value);
int qs_value_free (qs_value_t *value);
int qs_value_truth (qs_execute_t *exe, qs_value_t *val);
int qs_value_length (qs_value_t *v);
qs_value_t *qs_value_evaluate_block (qs_execute_t *exe, qs_value_t *val);
qs_value_t *qs_value_read (qs_execute_t *exe, qs_value_t *val);
qs_variable_t *qs_value_variable (qs_execute_t *exe, qs_value_t *val);
int qs_value_restring (qs_value_t *v, char *str);
int qs_value_can_modify (qs_value_t *val);
qs_value_t *qs_value_modify_value (qs_execute_t *exe, qs_value_t *val);
qs_value_t *qs_value_modify_value_real (qs_execute_t *exe,
   qs_value_t *val, int push);
char *qs_value_char_pointer (qs_value_t *val);
int qs_value_as_char (qs_value_t *rval, char *out);
int qs_value_update_from_string (qs_value_t *val);
qs_object_t *qs_value_object (qs_execute_t *exe, qs_value_t *val);

/* internal, constant return values. */
extern qs_value_t *QSV_ZERO;
extern qs_value_t *QSV_ONE;
extern qs_value_t *QSV_INVALID_TYPE;
extern qs_value_t *QSV_NOT_VARIABLE;
extern qs_value_t *QSV_NOT_PROPERTY;
extern qs_value_t *QSV_CANNOT_MODIFY;
extern qs_value_t *QSV_NOT_FUNC_CALL;
extern qs_value_t *QSV_INVALID_INDEX;
extern qs_value_t *QSV_INVALID_SUB_FUNC;
extern qs_value_t *QSV_NOT_ENOUGH_ARGS;
extern qs_value_t *QSV_INVALID_FUNC;
extern qs_value_t *QSV_INVALID_OPER;
extern qs_value_t *QSV_DIVISION_BY_ZERO;
extern qs_value_t *QSV_INVALID_RESOURCE;
extern qs_value_t *QSV_CANNOT_EXECUTE;
extern qs_value_t *QSV_CANNOT_UNWRAP;
extern qs_value_t *QSV_SUCCESS;
extern qs_value_t *QSV_NOT_IN_HEAP;
extern qs_value_t *QSV_SCOPE_LITERAL;
extern qs_value_t *QSV_SCOPE_OBJECT;
extern qs_value_t *QSV_SCOPE_BLOCK;
extern qs_value_t *QSV_SCOPE_PROPERTY;
extern qs_value_t *QSV_SCOPE_UNKNOWN;
extern qs_value_t *QSV_UNDEFINED;
extern qs_value_t *QSV_INVALID_VALUE;
extern qs_value_t *QSV_ALREADY_WOUND;

#endif
