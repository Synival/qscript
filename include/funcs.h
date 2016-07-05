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

/* function execution. */
inline qs_value_t *qs_func_run (qs_execute_t *exe, qs_func_t *func);

/* arg processing functions. */
inline char *qs_arg_string (qs_execute_t *exe, qs_value_t *val);
inline float qs_arg_float  (qs_execute_t *exe, qs_value_t *val);
inline int   qs_arg_int    (qs_execute_t *exe, qs_value_t *val);
inline qs_list_t *qs_arg_list (qs_execute_t *exe, qs_value_t *val);
inline qs_object_t *qs_arg_object (qs_execute_t *exe, qs_value_t *val);
inline int qs_func_error (qs_execute_t *exe, char *func_name, p_node_t *node,
   char *format, ...);

/* return value producing functions. */
inline qs_value_t *qs_return_value (qs_scheme_t *scheme);
inline qs_value_t *qs_return_string (qs_scheme_t *scheme, char *s);
inline qs_value_t *qs_return_int (qs_scheme_t *scheme, int i);
inline qs_value_t *qs_return_float (qs_scheme_t *scheme, float f);
inline qs_value_t *qs_return_char (qs_scheme_t *scheme, char c);
inline qs_value_t *qs_return_list (qs_scheme_t *scheme, int count);

/* code flow functions. */
int qs_func_break    (qs_execute_t *e);
int qs_func_return   (qs_execute_t *e);
int qs_func_continue (qs_execute_t *e);

#endif
