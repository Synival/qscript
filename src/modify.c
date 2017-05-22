/* modify.c
 * --------
 * individual commands with an effect executed on objects. */

#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include "qscript/objects.h"
#include "qscript/parser.h"
#include "qscript/resources.h"
#include "qscript/rlinks.h"
#include "qscript/stacks.h"

#include "qscript/modify.h"

qs_stack_t *qs_modify_stack_get (qs_rlink_t *rlink)
{
   if (rlink->stack_modify == NULL) {
      rlink->stack_modify = qs_stack_new (qs_modify_t);
      qs_stack_data (rlink->stack_modify, rlink, NULL);
   }
   return rlink->stack_modify;
}

qs_modify_t *qs_modify_push (qs_rlink_t *rlink, void *link, qs_modify_t **top,
   size_t data_size, qs_modify_func *func_pop)
{
   /* push a new moification. remember what was on top for linking later. */
   qs_stack_t *stack = qs_modify_stack_get (rlink);
   qs_modify_t *last = qs_stack_last (stack);
   qs_modify_t *new  = qs_stack_push (stack, qs_modify_sf_pop);
   memset (new, 0, sizeof (qs_modify_t));

   /* assign data to our new stack value. */
   new->link = link;
   if (data_size > 0) {
      new->data = calloc (1, data_size);
      new->data_size = data_size;
   }
   new->func_pop = func_pop;

   /* link to the rlink... */
   new->r_prev = last;
   if (new->r_prev)
      new->r_prev->r_next = new;
   new->rlink = rlink;

   /* ...link to the object. */
   new->o_prev = rlink->object->last_modify;
   if (new->o_prev)
      new->o_prev->o_next = new;
   rlink->object->last_modify = new;
   new->object = rlink->object;

   /* ...and link to our data. */
   new->d_top = top;
   new->d_prev = *top;
   if (new->d_prev)
      new->d_prev->d_next = new;
   *top = new;

   /* return our new modification. */
   return new;
}

int qs_modify_cleanup (qs_modify_t *m)
{
   /* this should never, ever happen, but potentially could if programs get
    * crazy enough with unforeseen circumstances.  toss a big error, do nothing
    * unsafe, and bail. */
   if (m->d_next) {
      p_error (NULL, "BAD ERROR: attempted to free non-last modification "
         "from '%s'@%d on '%s' (#%2d).\n",
         m->rlink->resource->name,
         m->rlink->priority, m->object->name, m->object->id);
      raise (SIGINT);
      return 0;
   }

   /* run any pop function specified. */
   if (m->func_pop)
      m->func_pop (m, m->link, m->data, m->d_prev ? m->d_prev->data : NULL);

   /* free data allocated. */
   if (m->data_size > 0) {
      free (m->data);
      m->data = NULL;
   }

   /* unlink from neighbors.  the rlink should already handle itself
    * because it's using a qs_stack_t. */
   if (m->d_prev) m->d_prev->d_next = m->d_next;
   if (m->d_next) m->d_next->d_prev = m->d_prev;
   else *(m->d_top)                 = m->d_prev;
   if (m->r_prev) m->r_prev->r_next = m->r_next;
   if (m->r_next) m->r_next->r_prev = m->r_prev;
   if (m->o_prev) m->o_prev->o_next = m->o_next;
   if (m->o_next) m->o_next->o_prev = m->o_prev;
   else m->object->last_modify      = m->o_prev;

   /* return success. */
   return 1;
}

QS_STACK_FUNC (qs_modify_sf_pop)
{
   /* make sure we don't free values twice. */
   qs_modify_t *m = data;
   if (m->flags & QS_MODIFY_POPPING) {
      p_error (NULL, "BAD ERROR: double-free detected for "
         "modification from '%s'@%d on '%s' (#%2d).\n",
         m, m->rlink->resource->name, m->rlink->priority, m->object->name,
         m->object->id);
      raise (SIGINT);
      return 0;
   }
   m->flags |= QS_MODIFY_POPPING;

   /* if we're not the last property modification, it (should) mean the
    * parent rlink did some further modifications. because the parent is
    * expected to be popped anway, pop its modifications now in order to
    * preserve stack order. */
   qs_modify_t *last;
   while ((last = m->d_next) != NULL) {
      /* the unwind process should be smart enough to know what needs to
       * be unwound, in the proper (reverse) order. by the time we're popping
       * modifications in the stack, the appropriate rlinks should already
       * be marked for unwinding. if that's not the case, there's a serious
       * error - report it. */
      if (last->rlink->flags & QS_RLINK_ON) {
         p_error (NULL, "BAD ERROR: modifier pop required for currently-wound "
            "'%s'@%d from '%s'@%d on '%s' (#%2d).\n",
            last->rlink->resource->name, last->rlink->priority,
            m->rlink->resource->name, m->rlink->priority, m->object->name,
            m->object->id);
         raise (SIGINT);
         return 0;
      }
      qs_stack_pop (last->rlink->stack_modify);
   }

   /* finally do some cleanup. */
   return qs_modify_cleanup (m);
}
