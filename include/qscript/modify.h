/* modify.h
 * --------
 * individual commands with an effect executed on objects. */

#ifndef __QS_MODIFY_H
#define __QS_MODIFY_H

#include "defs.h"

/* individual modifications on properties. */
struct _qs_modify_t {
   /* link to data we're modifying and data we're modifying it with. */
   void *link, *data;
   size_t data_size;
   qs_flags_t flags;
   qs_modify_t *d_prev, *d_next, **d_top;
   qs_modify_func *func_pop;

   /* links to the rlink and object from which it came. */
   qs_rlink_t *rlink;
   qs_object_t *object;
   qs_modify_t *r_prev, *r_next, *o_prev, *o_next;
};

/* functions called from rlinks (mostly). */
qs_stack_t *qs_modify_stack_get (qs_rlink_t *rlink);
qs_modify_t *qs_modify_push (qs_rlink_t *rlink, void *link, qs_modify_t **top,
   size_t data_size, qs_modify_func *func_pop);
int qs_modify_cleanup (qs_modify_t *m);

/* stack function. */
QS_STACK_FUNC (qs_modify_sf_pop);

#endif
