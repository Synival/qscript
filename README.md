# qscript

(This document is an early draft.  More coming soon.)

## Mission:

1. Instantiate any number of objects with relationships to each other in a consistent but dynamically modifiable state.
2. Allow interaction between object space and programming space using C.

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
5. External object modification from qscript via resource pushing/popping
6. State-safe object-scoped variables
7. Global (metaspace) variables

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
