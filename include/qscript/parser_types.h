/* parser_types.h
 * --------------
 * shared headers between the mpc-reliant parser and other projects
 * (such as qscript). */

#ifndef __P_PARSER_TYPES_H

/* simple typedefs. */
typedef uint32_t p_id_t;

/* typedefs for our parser. */
typedef struct _p_symbol_t p_symbol_t;
typedef struct _p_node_t   p_node_t;

/* functions assigned to our parser. */
#define P_FUNC(x) void x (p_node_t *node)
typedef P_FUNC(p_func_t);

#endif
