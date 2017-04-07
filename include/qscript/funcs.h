/* funcs.h
 * -------
 * useful functions to be used in qs_func()s. */

#ifndef __QS_FUNCS_H
#define __QS_FUNCS_H

#include "defs.h"

/* function usage and definitions. */
struct _qs_func_t {
   char *name, *format_string;
   int min_args;
   qs_func *func;
   int sub_func;
};

/* simple primitives. */
char *qs_arg_string (qs_execute_t *exe, qs_value_t *val);
float qs_arg_float  (qs_execute_t *exe, qs_value_t *val);
int   qs_arg_int    (qs_execute_t *exe, qs_value_t *val);

/* complex primitives. */
qs_list_t *qs_arg_list (qs_execute_t *exe, qs_value_t *val);
qs_object_t *qs_arg_object (qs_execute_t *exe, qs_value_t *val);

/* return value producing functions. */
qs_value_t *qs_return_value (qs_scheme_t *scheme);
qs_value_t *qs_return_string (qs_scheme_t *scheme, char *s);
qs_value_t *qs_return_int (qs_scheme_t *scheme, int i);
qs_value_t *qs_return_float (qs_scheme_t *scheme, float f);
qs_value_t *qs_return_char (qs_scheme_t *scheme, char c);
qs_value_t *qs_return_list (qs_scheme_t *scheme, int count);

/* arg processing functions. */
int qs_func_error (qs_execute_t *exe, char *func_name, p_node_t *node,
   char *format, ...);
qs_value_t *qs_func_run (qs_execute_t *exe, qs_func_t *func);

/* code flow functions. */
int qs_func_break    (qs_execute_t *e);
int qs_func_return   (qs_execute_t *e);
int qs_func_continue (qs_execute_t *e);

#endif
