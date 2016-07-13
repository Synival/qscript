/* values.h
 * --------
 * useful functions for value types. */

#ifndef __QS_VALUES_H
#define __QS_VALUES_H

#include "defs.h"

/* structure for each value. */
struct _qs_value_t {
   /* identification. */
   qs_value_type_e value_id;

   /* stored values. */
   char *val_s;
   int32_t val_i;
   float val_f;
   void *val_p;
   void *data;

   /* extra value. */
   qs_flags_t flags;

   /* link info. */
   int link_id;
   void *link;
   qs_list_t *list;
   qs_action_t *action_list;
   p_node_t *node;
   qs_scheme_t *scheme;
};

/* value functions. */
int qs_value_copy (qs_execute_t *exe, qs_value_t *dest, qs_value_t *src);
int qs_value_copy_const (qs_execute_t *exe, qs_value_t *dest, qs_value_t *src);
int qs_value_copy_real (qs_execute_t *exe, qs_value_t *dest, qs_value_t *src,
   int force);
char *qs_value_type (qs_value_t *val);
qs_variable_t *qs_value_variable (qs_execute_t *exe, qs_value_t *v);
qs_property_t *qs_value_property (qs_execute_t *exe, qs_value_t *v);
int qs_value_cleanup (qs_value_t *value);
int qs_value_cleanup_base (qs_value_t *value);
int qs_value_free (qs_value_t *value);
int qs_value_truth (qs_execute_t *exe, qs_value_t *val);
int qs_value_length (qs_value_t *v);
qs_value_t *qs_value_evaluate_block (qs_execute_t *exe, qs_list_t *list);
qs_value_t *qs_value_evaluate (qs_execute_t *exe, qs_value_t *val);
int qs_value_restring (qs_value_t *v, char *str);
inline int qs_value_can_modify (qs_execute_t *exe, qs_value_t *val);
inline qs_value_t *qs_value_lvalue (qs_execute_t *exe, qs_value_t *val);
qs_value_t *qs_value_lvalue_real (qs_execute_t *exe, qs_value_t *val,
   int push);
char *qs_value_char_pointer (qs_value_t *val);
int qs_value_as_char (qs_value_t *rval, char *out);
int qs_value_update_from_string (qs_value_t *val);
qs_object_t *qs_value_object (qs_execute_t *exe, qs_value_t *val);
int qs_value_contains (qs_value_t *haystack, qs_value_t *needle);
int qs_value_init (qs_value_t *val, qs_value_type_e type, ...);
int qs_value_list_internalize (qs_value_t *value);
qs_list_t *qs_value_list (qs_value_t *value);

/* macros. */
#define qs_value_read(x,y) \
   qs_value_evaluate (x, y)

/* internal, constant return values. */
extern qs_value_t *QSV_ZERO;
extern qs_value_t *QSV_ONE;
extern qs_value_t *QSV_INVALID_TYPE;
extern qs_value_t *QSV_NOT_VARIABLE;
extern qs_value_t *QSV_NOT_PROPERTY;
extern qs_value_t *QSV_CANNOT_MODIFY;
extern qs_value_t *QSV_NOT_FUNC_CALL;
extern qs_value_t *QSV_INVALID_INDEX;
extern qs_value_t *QSV_NO_INDEX;
extern qs_value_t *QSV_INVALID_SUB_FUNC;
extern qs_value_t *QSV_NOT_ENOUGH_ARGS;
extern qs_value_t *QSV_INVALID_FUNC;
extern qs_value_t *QSV_INVALID_OPER;
extern qs_value_t *QSV_DIVISION_BY_ZERO;
extern qs_value_t *QSV_MOD_BY_ZERO;
extern qs_value_t *QSV_INVALID_RESOURCE;
extern qs_value_t *QSV_CANNOT_EXECUTE;
extern qs_value_t *QSV_CANNOT_UNWRAP;
extern qs_value_t *QSV_SCOPE_LITERAL;
extern qs_value_t *QSV_SCOPE_RLINK;
extern qs_value_t *QSV_SCOPE_BLOCK;
extern qs_value_t *QSV_SCOPE_PROPERTY;
extern qs_value_t *QSV_SCOPE_UNKNOWN;
extern qs_value_t *QSV_UNDEFINED;
extern qs_value_t *QSV_INVALID_VALUE;
extern qs_value_t *QSV_NO_OBJECT;
extern qs_value_t *QSV_ALREADY_WOUND;
extern qs_value_t *QSV_INVALID_PROPERTY;

#endif
