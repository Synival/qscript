/* defs.h
 * ------
 * typedefs and #defines. */

#ifndef __QS_DEFS_H
#define __QS_DEFS_H

#include <stdint.h>

#include "parser.h"

/* variable reference type. */
enum qscript_ref_type_e {
   QS_REF_NONE = 0,
   QS_REF_VARIABLE,
   QS_REF_INDEX,
   QS_REF_LAST
};

/* variable scopes. */
enum qscript_execute_type_e {
   QS_EXE_UNDEFINED = 0,
   QS_EXE_LAMBDA,
   QS_EXE_FUNC,
   QS_EXE_RESOURCE,
   QS_EXE_BLOCK,
   QS_EXE_LOOP,
   QS_EXE_LAST
};

/* variable scopes. */
enum qscript_variable_scope_e {
   QS_SCOPE_AUTO = -2,
   QS_SCOPE_UNDEFINED = -1,
   QS_SCOPE_LITERAL = 0,
   QS_SCOPE_RLINK,
   QS_SCOPE_BLOCK,
   QS_SCOPE_LAST
};

/* action types. */
enum qscript_action_type_e {
   QS_ACTION_UNDEFINED = 0,
   QS_ACTION_CALL,
   QS_ACTION_INDEX,
   QS_ACTION_PROPERTY,
   QS_ACTION_LAST
};

/* our giant list of qscript tokens. */
enum qscript_tag_e {
   QSCRIPT_INVALID = -2,
   QSCRIPT_UNDEFINED = -1,
   QSCRIPT_ROOT = 0,
   QSCRIPT_COMMENT,
   QSCRIPT_RESOURCE,
   QSCRIPT_RTYPE,
   QSCRIPT_RNAME,
   QSCRIPT_BLOCK,
   QSCRIPT_LIST,
   QSCRIPT_OUTER_LIST,
   QSCRIPT_VALUE,
   QSCRIPT_OUTER_BLOCK,
   QSCRIPT_SEPARATOR,
   QSCRIPT_STRING,
   QSCRIPT_SIMPLE_STRING,
   QSCRIPT_COMPLEX_STRING,
   QSCRIPT_NUMBER,
   QSCRIPT_FLOAT,
   QSCRIPT_INT,
   QSCRIPT_VARIABLE,
   QSCRIPT_ACTION,
   QSCRIPT_CALL,
   QSCRIPT_UNWRAP,
   QSCRIPT_INDEX,
   QSCRIPT_CHAR,
   QSCRIPT_OBJECT,
   QSCRIPT_PROPERTY,
   QSCRIPT_MEMBER,
   QSCRIPT_LAST,
};

/* rlink states. */
#define QS_RLINK_ON              0x01

/* flags for property modifiers. */
#define QS_MODIFY_POPPING        0x01

/* flags for execution states. */
#define QS_EXE_BREAK             0x01
#define QS_EXE_FREE_LIST         0x02
#define QS_EXE_READ_ONLY         0x04

/* flags for resources. */
#define QS_RSRC_AUTO_INSTANTIATE 0x01
#define QS_RSRC_LINKED           0x02

/* flags for values. */
#define QS_VALUE_UNWRAP          0x01
#define QS_VALUE_MUTABLE         0x02

/* value link types. */
#define QS_LINK_LITERAL          0
#define QS_LINK_VARIABLE         1
#define QS_LINK_PROPERTY         2

/* flags for objects. */
#define QS_OBJECT_GLOBAL         0x01

/* id limits. */
#define QS_ID_MAX                65536

/* simple types. */
typedef uint32_t qs_id_t;
typedef struct _qs_id_manager_t qs_id_manager_t;

/* typedefs. */
typedef struct _qs_object_t    qs_object_t;
typedef struct _qs_value_t     qs_value_t;
typedef struct _qs_resource_t  qs_resource_t;
typedef struct _qs_rlink_t     qs_rlink_t;
typedef struct _qs_scheme_t    qs_scheme_t;
typedef struct _qs_func_t      qs_func_t;
typedef struct _qs_variable_t  qs_variable_t;
typedef struct _qs_execute_t   qs_execute_t;
typedef struct _qs_list_t      qs_list_t;
typedef struct _qs_action_t    qs_action_t;
typedef struct _qs_stack_t     qs_stack_t;
typedef struct _qs_property_t  qs_property_t;
typedef struct _qs_modify_t    qs_modify_t;

/* function types. */
#define QS_FUNC(x) qs_value_t *x (qs_object_t *object, qs_rlink_t *rlink, \
   qs_execute_t *exe, qs_action_t *action, qs_func_t *func, int sub_func, \
   int args, qs_value_t **arg)
typedef QS_FUNC(qs_func);

#define QS_STACK_FUNC(x) int x (qs_stack_t *stack, void *data)
typedef QS_STACK_FUNC(qs_stack_func);

#define QS_ARGV(x) (qs_arg_value    (exe, arg[x]))
#define QS_ARGS(x) (qs_arg_string   (exe, arg[x]))
#define QS_ARGF(x) (qs_arg_float    (exe, arg[x]))
#define QS_ARGI(x) (qs_arg_int      (exe, arg[x]))
#define QS_ARG_ERROR(x,...) \
   (qs_func_error (exe, func->name, arg[x]->node, __VA_ARGS__))

#define QS_RETS(x)    qs_return_string (rlink->scheme, x)
#define QS_RETF(x)    qs_return_float  (rlink->scheme, x)
#define QS_RETI(x)    qs_return_int    (rlink->scheme, x)
#define QS_RETC(x)    qs_return_char   (rlink->scheme, x)
#define QS_RETURN()   qs_func_return   (exe)
#define QS_BREAK()    qs_func_break    (exe)
#define QS_CONTINUE() qs_func_continue (exe)

#define QSV_DEFINE(n,t,s,i,f) \
   qs_value_t n##_val = { t, s, i, f }; \
   qs_value_t *n = &n##_val

#endif
