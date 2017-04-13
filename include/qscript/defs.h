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
   QS_LIST_LIST      = 1,
   QS_LIST_BLOCK     = 2,
   QS_LIST_LAST      = 3,
} qs_list_type_e;

/* value types. */
typedef enum _qs_value_type_e {
   /* simple real primitives. */
   QS_VALUE_UNDEFINED      = 0,
   QS_VALUE_BOOLEAN        = 1,
   QS_VALUE_INT            = 2,
   QS_VALUE_FLOAT          = 3,
   QS_VALUE_STRING         = 4,

   /* complex real primitives. */
   QS_VALUE_CHAR           = 5,
   QS_VALUE_LIST           = 6,
   QS_VALUE_OBJECT         = 7,

   /* abstract primitives. */
   QS_VALUE_BLOCK          = 8,
   QS_VALUE_VARIABLE       = 9,
   QS_VALUE_PROPERTY       = 10,
   QS_VALUE_PROPERTY_VALUE = 11,

   /* indicate the final one. */
   QS_VALUE_LAST           = 12
} qs_value_type_e;

/* variable scopes. */
typedef enum _qs_execute_type_e {
   QS_EXECUTE_UNDEFINED = 0,
   QS_EXECUTE_LAMBDA    = 1,
   QS_EXECUTE_FUNC      = 2,
   QS_EXECUTE_RESOURCE  = 3,
   QS_EXECUTE_BLOCK     = 4,
   QS_EXECUTE_LOOP      = 5,
   QS_EXECUTE_LAST      = 6
} qs_execute_type_e;

/* variable scopes. */
typedef enum _qs_variable_scope_e {
   QS_SCOPE_AUTO      = -1,
   QS_SCOPE_UNDEFINED = 0,
   QS_SCOPE_RLINK     = 1,
   QS_SCOPE_BLOCK     = 2,
   QS_SCOPE_SCHEME    = 3,
   QS_SCOPE_LAST      = 4,
} qs_variable_scope_e;

/* action types. */
typedef enum _qs_action_type_e {
   QS_ACTION_UNDEFINED       = 0,
   QS_ACTION_CALL            = 1,
   QS_ACTION_INDEX           = 2,
   QS_ACTION_PROPERTY        = 3,
   QS_ACTION_PROPERTY_VALUE  = 4,
   QS_ACTION_LAST            = 5
} qs_action_type_e;

/* our giant list of qscript tokens. */
typedef enum _qs_tag_type_e {
   QSCRIPT_MPC            = -1,
   QSCRIPT_UNDEFINED      = 0,
   QSCRIPT_ROOT           = 1,
   QSCRIPT_COMMENT        = 2,
   QSCRIPT_RESOURCE       = 3,
   QSCRIPT_RFLAGS         = 4,
   QSCRIPT_RNAME          = 5,
   QSCRIPT_BLOCK          = 6,
   QSCRIPT_LIST           = 7,
   QSCRIPT_OUTER_LIST     = 8,
   QSCRIPT_VALUE          = 9,
   QSCRIPT_VFLAGS         = 10,
   QSCRIPT_PRIMITIVE      = 11,
   QSCRIPT_OUTER_BLOCK    = 12,
   QSCRIPT_SEPARATOR      = 13,
   QSCRIPT_STRING         = 14,
   QSCRIPT_SIMPLE_STRING  = 15,
   QSCRIPT_COMPLEX_STRING = 16,
   QSCRIPT_NUMBER         = 17,
   QSCRIPT_FLOAT          = 18,
   QSCRIPT_INT            = 19,
   QSCRIPT_VARIABLE       = 20,
   QSCRIPT_ACTION         = 21,
   QSCRIPT_CALL           = 22,
   QSCRIPT_INDEX          = 23,
   QSCRIPT_CHAR           = 24,
   QSCRIPT_OBJECT         = 25,
   QSCRIPT_PROPERTY       = 26,
   QSCRIPT_PROPERTY_VALUE = 27,
   QSCRIPT_BOOLEAN        = 28,
   QSCRIPT_LAST           = 29
} qs_tag_type_e;

/* value link types. */
typedef enum _qs_value_link_e {
   QS_LINK_UNDEFINED = 0,
   QS_LINK_LITERAL   = 1,
   QS_LINK_VARIABLE  = 2,
   QS_LINK_PROPERTY  = 3,
   QS_LINK_LAST      = 4
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
typedef struct _qs_file_t      qs_file_t;

/* function types. */
#define QS_FUNC(x) qs_value_t *x (qs_object_t *object, qs_rlink_t *rlink, \
   qs_execute_t *exe, qs_action_t *action, qs_func_t *func, int sub_func, \
   int args, qs_value_t **arg)
typedef QS_FUNC(qs_func);

#define QS_STACK_FUNC(x) int x (qs_stack_t *stack, void *data)
typedef QS_STACK_FUNC(qs_stack_func);

#define QS_ARGV(x) (qs_value_read   (exe, arg[x]))
#define QS_ARGB(x) (qs_arg_boolean  (exe, arg[x]))
#define QS_ARGI(x) (qs_arg_int      (exe, arg[x]))
#define QS_ARGF(x) (qs_arg_float    (exe, arg[x]))
#define QS_ARGS(x) (qs_arg_string   (exe, arg[x]))
#define QS_ARGL(x) (qs_arg_list     (exe, arg[x]))
#define QS_ARGO(x) (qs_arg_object   (exe, arg[x]))
#define QS_ARG_ERROR(x,...) \
   (qs_func_error (exe, func->name, arg[x]->node, __VA_ARGS__))

#define QS_RETB(x)    qs_return_boolean   (rlink->scheme, x)
#define QS_RETI(x)    qs_return_int       (rlink->scheme, x)
#define QS_RETF(x)    qs_return_float     (rlink->scheme, x)
#define QS_RETS(x)    qs_return_string    (rlink->scheme, x)
#define QS_RETC(x)    qs_return_char      (rlink->scheme, x)
#define QS_RETL(x)    qs_return_list      (rlink->scheme, x)
#define QS_RETU(x)    qs_return_undefined (rlink->scheme, x)
#define QS_RETURN()   qs_func_return   (exe)
#define QS_BREAK()    qs_func_break    (exe)
#define QS_CONTINUE() qs_func_continue (exe)

#define QSV_DEFINE(n,t,s,i,f) \
   qs_value_t n##_val = { t, s, i, f }; \
   qs_value_t *n = &n##_val

#endif
