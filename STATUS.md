# Project Status:

## What's in This Project:

1. Source for *libqscript* (contains [mpc](https://github.com/orangeduck/mpc))
2. qscript parser that instantiates object `main`.
3. qscript *test.qs* script to test various functions.
4. Example project *QMaze*.
5. Example project *objdump*.

## What's done:

1. Complete qscript language definition.
2. Parser implementation.
3. Runtime execution.
4. Ongoing feature test script (test.qs)
5. Ongoing test game (qmaze)

## What's not:

1. Fully implemented and tested resource unwinding and rewinding
2. Value "influences" from external sources
3. Object list search functions
4. Object relationship management
5. Object modification from inside qscript via resource pushing/popping
6. Object-scoped variables
7. Globally-scoped variables
8. Reading object properties / variables from external environment
9. External modification of object variables
10. "Instant" execution scripts
11. Evaluated value caching for uninfluenced values
12. Function flags for "only when first activated", "instant-only"
13. Action queues from `qs_rlink_t` for `qs_scheme_update()`.
14. qfunc `free()`, `spawn()`, and more object manipulation.
15. More list-manipulating functions.
16. This documentation.
17. Non-game example... Minimal web browser?