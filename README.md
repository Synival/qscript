# qscript

(This document is an early draft.  More coming soon.)

## Why:

1. Need to keep persistant object states that can be modified without potential state corruption.
2. Need to let objects interact with each other and efficiently respond to all changes from other objects.

## Mission:

1. Instantiate any number of objects with relationships to each other in a consistent but dynamically modifiable state.
2. Allow interaction between object space and programming space using C.
3. Resource execution on objects in arbitrary order determined by priority values.
4. Allow language extension through schemes.

## Language Grammar:

1. TBC

## Internal structures:

1. qs_object_t
2. qs_value_t
3. qs_resource_t
4. qs_rlink_t
5. qs_scheme_t
6. qs_func_t
7. qs_variable_t
8. qs_execute_t
9. qs_list_t
10. qs_action_t
11. qs_stack_t
12. qs_property_t
13. qs_modify_t

## What's done:

1. Complete qscript language definition.
2. Parser implementation.
3. Runtime execution.
4. Ongoing feature test script (test.qs)
5. Ongoing test game (qmaze)

## What's not:

1. Fully implemented and testing resource unwinding and rewinding
2. Object "metaspace"
3. Keep track of property access between objects
4. Object list search functions
5. Object modification from qscript via resource pushing/popping
6. State-safe object-scoped variables
7. Global (metaspace) variables
8. Reading object properties / variables from external environment
9. External modification of object variables
10. "Execution" scripts

## Primitives:

### Simple:

1. undefined
2. int
3. float
4. string

### Complex:

1. char
2. list
3. object

### Internal:

1. block
2. variable
3. property
4. action

## Enumerated Values:

### Tags:

1. QSCRIPT_INVALID
2. QSCRIPT_UNDEFINED
3. QSCRIPT_ROOT
4. QSCRIPT_COMMENT
5. QSCRIPT_RESOURCE
6. QSCRIPT_RTYPE6
7. QSCRIPT_RNAME
8. QSCRIPT_BLOCK
9. QSCRIPT_LIST
10. QSCRIPT_OUTER_LIST
11. QSCRIPT_VALUE
12. QSCRIPT_OUTER_BLOCK
13. QSCRIPT_SEPARATOR
14. QSCRIPT_STRING
15. QSCRIPT_SIMPLE_STRING
16. QSCRIPT_COMPLEX_STRING
17. QSCRIPT_NUMBER
18. QSCRIPT_FLOAT
19. QSCRIPT_INT
20. QSCRIPT_VARIABLE
21. QSCRIPT_ACTION
22. QSCRIPT_CALL
23. QSCRIPT_UNWRAP
24. QSCRIPT_INDEX
25. QSCRIPT_CHAR
26. QSCRIPT_OBJECT
27. QSCRIPT_PROPERTY
28. QSCRIPT_MEMBER

### Action Types:

1. QS_ACTION_UNDEFINED <-- not used
2. QS_ACTION_CALL
3. QS_ACTION_INDEX
4. QS_ACTION_PROPERTY

### Variable Scopes:

1. QS_SCOPE_AUTO
2. QS_SCOPE_UNDEFINED <-- not used
3. QS_SCOPE_RLINK
4. QS_SCOPE_BLOCK

### Execution State Types:

1. QS_EXE_UNDEFINED <-- not used
2. QS_EXE_LAMBDA
3. QS_EXE_FUNC
4. QS_EXE_RESOURCE
5. QS_EXE_BLOCK
6. QS_EXE_LOOP

## Variable scopes:

1. Block
2. RLink
3. Object
4. Global

## Action types:

1. call
2. index
3. property

## Standard library:

### Text output:

1. print
2. echo
3. print_resource
4. print_value

### Functional:

1. +
2. -
3. *
4. /
5. **
6. |
7. &
8. ^
9. %
10. ==
11. !=
12. >
13. >=
14. <
15. <=
16. ||
17. &&
18. ^^
19. !||
20. !&&
21. !^^
22. multi

### Informative:

1. ?
2. !
3. inv
4. type
5. scope
6. length
7. this
8. id
9. name
10. func_exists
11. tokenize

### Assigning / Modifying:

1. +=
2. -=
3. *=
4. /=
5. **=
6. |=
7. &=
8. ^=
9. %=
10. =
11. ++
12. --
13. vars

### Code Flow:

1. if
2. return
3. break
4. continue
5. while
6. for
7. switch
8. run
9. eval
10. for_each

### Argument Processing:

1. args
2. arg
3. arg_list

### Casting:

1. int
2. string
3. float
4. char
5. object
6. variable
7. property

## Public Macros:

### Definitions:

1. QS_FUNC
2. QS_STACK_FUNC
3. QSV_DEFINE

### Inside qs_func call:

1. QS_ARGV
2. QS_ARGS
3. QS_ARGF
4. QS_ARGI
5. QS_ARGL
6. QS_ARGO
7. QS_ARG_ERROR
8. QS_RETS
9. QS_RETF
10. QS_RETI
11. QS_RETC
12. QS_RETURN
13. QS_BREAK
14. QS_CONTINUE

## Public Functions:

qs_arg_float
qs_arg_int
qs_arg_string
qs_arg_value
qs_arg_list
qs_arg_value
qs_execute_get_block
qs_execute_get_call
qs_execute_pop
qs_execute_push
qs_func_break
qs_func_continue
qs_func_error
qs_func_return
qs_func_run
qs_list_copy
qs_list_free
qs_list_new
qs_list_value
qs_object_free
qs_object_get
qs_object_get_by_id
qs_object_new
qs_parse_content
qs_parse_init
qs_parse_file
qs_print_action
qs_print_list
qs_print_resource
qs_print_value
qs_property_get
qs_property_value
qs_resource_get
qs_resource_get_auto_instance
qs_resource_get_by_id
qs_return_char
qs_return_float
qs_return_int
qs_return_string
qs_return_value_new
qs_rlink_is_child_of
qs_rlink_pop
qs_rlink_push
qs_rlink_push_at
qs_rlink_unwind
qs_rlink_wind
qs_scheme_cleanup
qs_scheme_free
qs_scheme_funcs
qs_scheme_get_func
qs_scheme_heap_value
qs_scheme_new
qs_scheme_update
qs_value_can_modify
qs_value_cleanup
qs_value_cleanup_self
qs_value_contains
qs_value_copy
qs_value_copy_const
qs_value_init
qs_value_length
qs_value_list
qs_value_list_internalize
qs_value_lvalue
qs_value_object
qs_value_property
qs_value_read
qs_value_restring
qs_value_truth
qs_value_type
qs_value_update_from_string
qs_value_variable
qs_variable_contains
qs_variable_get
qs_variable_value

## Pre-defined Values:

1. QSV_ZERO
2. QSV_ONE
3. QSV_INVALID_TYPE
4. QSV_NOT_VARIABLE
5. QSV_NOT_PROPERTY
6. QSV_CANNOT_MODIFY
7. QSV_NOT_FUNC_CALL
8. QSV_INVALID_INDEX
9. QSV_NO_INDEX
10. QSV_INVALID_SUB_FUNC
11. QSV_NOT_ENOUGH_ARGS
12. QSV_INVALID_FUNC
13. QSV_INVALID_OPER
14. QSV_DIVISION_BY_ZERO
15. QSV_MOD_BY_ZERO
16. QSV_INVALID_RESOURCE
17. QSV_CANNOT_EXECUTE
18. QSV_CANNOT_UNWRAP
19. QSV_SCOPE_LITERAL
20. QSV_SCOPE_RLINK
21. QSV_SCOPE_BLOCK
22. QSV_SCOPE_PROPERTY
23. QSV_SCOPE_UNKNOWN
24. QSV_UNDEFINED
25. QSV_INVALID_VALUE
26. QSV_NO_OBJECT
27. QSV_ALREADY_WOUND
28. QSV_INVALID_PROPERTY

## Implementation:

1. Your first qscript
2. Example of a simple parser.
3. More complex qscript with multiple objects.
4. Instantiating and freeing objects.
5. Reading properties from objects.
6. Reading and writing object variables.
7. Internal cross-object access.
8. External pushing / popping resources onto objects.
9. Internal pushing / popping.
10. Priorities, winding, unwinding, rewinding
