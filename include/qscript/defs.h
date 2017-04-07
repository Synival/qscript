/* defs.h
 * ------
 * typedefs and #defines. */

#ifndef __QS_DEFS_H
#define __QS_DEFS_H

#ifdef __TARGET_QSCRIPT__
   #include "qs_config.h"
#endif

#include <stdint.h>

#include "parser_types.h"

/* list types. */
typedef enum _qs_list_type_e {
   QS_LIST_UNDEFINED = 0,
   QS_LIST_LIST,
   QS_LIST_BLOCK,
   QS_LIST_LAST,
} qs_list_type_e;

/* value types. */
typedef enum _qs_value_type_e {
   /* simple real primitives. */
   QS_VALUE_UNDEFINED = 0,
   QS_VALUE_INT,
   QS_VALUE_FLOAT,
   QS_VALUE_STRING,

   /* complex real primitives. */
   QS_VALUE_CHAR,
   QS_VALUE_LIST,
   QS_VALUE_OBJECT,

   /* abstract primitives. */
   QS_VALUE_BLOCK,
   QS_VALUE_VARIABLE,
   QS_VALUE_PROPERTY,

   /* indicate the final one. */
   QS_VALUE_LAST,
} qs_value_type_e;

/* variable scopes. */
typedef enum _qs_execute_type_e {
   QS_EXECUTE_UNDEFINED = 0,
   QS_EXECUTE_LAMBDA,
   QS_EXECUTE_FUNC,
   QS_EXECUTE_RESOURCE,
   QS_EXECUTE_BLOCK,
   QS_EXECUTE_LOOP,
   QS_EXECUTE_LAST
} qs_execute_type_e;

/* variable scopes. */
typedef enum _qs_variable_scope_e {
   QS_SCOPE_AUTO = -1,
   QS_SCOPE_UNDEFINED = 0,
   QS_SCOPE_RLINK,
   QS_SCOPE_BLOCK,
   QS_SCOPE_LAST
} qs_variable_scope_e;

/* action types. */
typedef enum _qs_action_type_e {
   QS_ACTION_UNDEFINED = 0,
   QS_ACTION_CALL,
   QS_ACTION_INDEX,
   QS_ACTION_PROPERTY,
   QS_ACTION_LAST
} qs_action_type_e;

/* our giant list of qscript tokens. */
typedef enum _qs_tag_type_e {
   QSCRIPT_MPC = -1,
   QSCRIPT_UNDEFINED = 0,
   QSCRIPT_ROOT,
   QSCRIPT_COMMENT,
   QSCRIPT_RESOURCE,
   QSCRIPT_RFLAGS,
   QSCRIPT_RNAME,
   QSCRIPT_BLOCK,
   QSCRIPT_LIST,
   QSCRIPT_OUTER_LIST,
   QSCRIPT_VALUE,
   QSCRIPT_VFLAGS,
   QSCRIPT_PRIMITIVE,
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
   QSCRIPT_INDEX,
   QSCRIPT_CHAR,
   QSCRIPT_OBJECT,
   QSCRIPT_PROPERTY,
   QSCRIPT_LAST,
} qs_tag_type_e;

/* value link types. */
typedef enum _qs_value_link_e {
   QS_LINK_UNDEFINED = 0,
   QS_LINK_LITERAL,
   QS_LINK_VARIABLE,
   QS_LINK_PROPERTY,
   QS_LINK_LAST,
} qs_value_link_e;

/* rlink states. */
#define QS_RLINK_ON              0x01

/* flags for property modifiers. */
#define QS_MODIFY_POPPING        0x01

/* flags for execution states. */
#define QS_EXECUTE_BREAK         0x01
#define QS_EXECUTE_FREE_LIST     0x02
#define QS_EXECUTE_READ_ONLY     0x04

/* flags for resources. */
#define QS_RESOURCE_GLOBAL       0x01
#define QS_RESOURCE_LINKED       0x02

/* flags for values. */
#define QS_VALUE_UNWRAP          0x01
#define QS_VALUE_MUTABLE         0x02
#define QS_VALUE_HEAP            0x04
#define QS_VALUE_FREE_ACTIONS    0x08

/* flags for objects. */
#define QS_OBJECT_GLOBAL         0x01

/* id limits. */
#define QS_ID_MAX                65536

/* simple types. */
typedef uint32_t qs_id_t;
typedef uint32_t qs_flags_t;
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

#define QS_ARGV(x) (qs_value_read   (exe, arg[x]))
#define QS_ARGS(x) (qs_arg_string   (exe, arg[x]))
#define QS_ARGF(x) (qs_arg_float    (exe, arg[x]))
#define QS_ARGI(x) (qs_arg_int      (exe, arg[x]))
#define QS_ARGL(x) (qs_arg_list     (exe, arg[x]))
#define QS_ARGO(x) (qs_arg_object   (exe, arg[x]))
#define QS_ARG_ERROR(x,...) \
   (qs_func_error (exe, func->name, arg[x]->node, __VA_ARGS__))

#define QS_RETS(x)    qs_return_string (rlink->scheme, x)
#define QS_RETF(x)    qs_return_float  (rlink->scheme, x)
#define QS_RETI(x)    qs_return_int    (rlink->scheme, x)
#define QS_RETC(x)    qs_return_char   (rlink->scheme, x)
#define QS_RETL(x)    qs_return_list   (rlink->scheme, x)
#define QS_RETURN()   qs_func_return   (exe)
#define QS_BREAK()    qs_func_break    (exe)
#define QS_CONTINUE() qs_func_continue (exe)

#define QSV_DEFINE(n,t,s,i,f) \
   qs_value_t n##_val = { t, s, i, f }; \
   qs_value_t *n = &n##_val

#endif
