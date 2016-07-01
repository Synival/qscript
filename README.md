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

1. int
2. float
3. string
4. char
5. list
6. object
7. undefined

## Internal value types:

1. variable
2. property
3. action

## Variable scopes:

1. Block
2. Object
3. Global

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
