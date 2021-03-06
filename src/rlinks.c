/* rlinks.c
 * --------
 * resource linking with objects. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "qscript/actions.h"
#include "qscript/execute.h"
#include "qscript/funcs.h"
#include "qscript/language.h"
#include "qscript/link.h"
#include "qscript/objects.h"
#include "qscript/parser.h"
#include "qscript/properties.h"
#include "qscript/resources.h"
#include "qscript/schemes.h"
#include "qscript/stacks.h"
#include "qscript/variables.h"

#include "qscript/rlinks.h"

qs_rlink_t *qs_rlink_inject (qs_object_t *obj, qs_resource_t *resource,
                           int priority)
{
   qs_rlink_t *rl;

   /* where should this resource be linked? */
   if (obj->rlink_list_front == NULL)
      rl = NULL;
   else
      for (rl = obj->rlink_list_front; rl->next != NULL; rl = rl->next)
         if (rl->next->priority > priority)
            break;

   /* create a new object and return it. */
   return qs_rlink_inject_at (obj, resource, priority, NULL, rl, NULL, NULL);
}

qs_rlink_t *qs_rlink_inject_at (qs_object_t *obj, qs_resource_t *resource,
   int priority, qs_rlink_t *parent, qs_rlink_t *prev, qs_execute_t *exe,
   qs_action_t *action)
{
   qs_rlink_t *new;

   /* bail if we have no resource to push. */
   if (resource == NULL)
      return NULL;

   /* allocate an empty rlink. */
   new = calloc (1, sizeof (qs_rlink_t));
   new->resource = resource;
   new->object   = obj;
   new->scheme   = resource->scheme;
   new->priority = priority;

   /* add to our parent. */
   new->parent = parent;
   new->prev   = prev;
   if (new->prev) {
      new->next = new->prev->next;
      new->prev->next = new;
   }
   else if (parent) {
      new->next = parent->child_front;
      parent->child_front = new;
   }
   else {
      new->next = obj->rlink_list_front;
      obj->rlink_list_front = new;
   }
   if (new->next)
      new->next->prev = new;
   else if (parent)
      parent->child_back = new;
   else
      obj->rlink_list_back = new;

   /* link to our resource. */
   new->rsrc_prev = resource->rlink_list_back;
   if (new->rsrc_prev)
      new->rsrc_prev->rsrc_next = new;
   else
      resource->rlink_list_front = new;
   resource->rlink_list_back = new;

   /* keep track of the action and execution state we were called from. */
   new->parent_action = action;
   new->parent_exe = exe;

   /* return our new rlink. */
   return new;
}

qs_value_t *qs_rlink_wind (qs_rlink_t *rlink)
{
   return (rlink->parent ? QSV_UNDEFINED : qs_rlink_wind_from (rlink, NULL));
}

qs_value_t *qs_rlink_wind_from (qs_rlink_t *rlink, qs_execute_t *exe)
{
   /* can't wind what's already wound. */
   if (rlink->flags & QS_RLINK_ON) {
      p_node_t *node = (exe->action) ? exe->action->node : NULL;
      p_error (node, "attempted to wind already-wound resource '%s'@%d "
         "on object '%s' (#%d).\n", rlink->resource->name, rlink->priority,
         rlink->object->name);
      return QSV_ALREADY_WOUND;
   }
   rlink->flags |= QS_RLINK_ON;

   /* if there's no execution state, create one here. */
   qs_execute_t *new_exe = NULL;
   if (exe == NULL)
      exe = new_exe = qs_execute_push (QS_EXECUTE_RESOURCE, rlink, NULL, NULL,
         rlink->resource->name, 0, NULL);

   /* evaluate our rlink's block. */
   qs_value_t *rval = qs_value_evaluate_block (exe, rlink->resource->block);

   /* if we pushed a new execution state, pop it. */
   if (new_exe)
      qs_execute_pop (new_exe);

   /* return the evaluation of the block. */
   return rval;
}

int qs_rlink_unwind (qs_rlink_t *rlink)
{
   /* don't unwind if already wound. */
   if (!(rlink->flags & QS_RLINK_ON))
      return 0;
   rlink->flags &= ~QS_RLINK_ON;

   /* unwind children, in reverse order.  children should have the same
    * priority, so priorty checks aren't necessary. */
   qs_rlink_t *r;
   for (r = rlink->child_back; r != NULL; r = r->prev)
      qs_rlink_unwind (r);

   /* pop everything in our property modify stack. */
   if (rlink->stack_modify)
      qs_stack_empty (rlink->stack_modify);

   /* return success. */
   return 1;
}

int qs_rlink_eject (qs_rlink_t *rlink)
{
   /* unwind before anything else, and free modification stack. */
   qs_rlink_unwind (rlink);
   if (rlink->stack_modify)
      qs_stack_free (rlink->stack_modify);

   /* free children first. */
   while (rlink->child_back)
      qs_rlink_eject (rlink->child_back);

   /* free any remaining variables. */
   while (rlink->variable_list_back)
      qs_variable_free (rlink->variable_list_back);

   /* unlink from our parent (if a child of an rlink)... */
   if (rlink->parent) {
      if (rlink->prev) rlink->prev->next = rlink->next;
      else rlink->parent->child_front = rlink->next;
      if (rlink->next) rlink->next->prev = rlink->prev;
      else rlink->parent->child_back = rlink->prev;
   }
   /* ...or our object (if top level). */
   else {
      if (rlink->prev) rlink->prev->next = rlink->next;
      else rlink->object->rlink_list_front = rlink->next;
      if (rlink->next) rlink->next->prev = rlink->prev;
      else rlink->object->rlink_list_back = rlink->prev;
   }

   /* unlink from our resource. */
   if (rlink->rsrc_prev) rlink->rsrc_prev->rsrc_next = rlink->rsrc_next;
   else rlink->resource->rlink_list_front            = rlink->rsrc_next;
   if (rlink->rsrc_next) rlink->rsrc_next->rsrc_prev = rlink->rsrc_prev;
   else rlink->resource->rlink_list_back             = rlink->rsrc_prev;
   rlink->resource = NULL;

   /* free our own memory and return success. */
   free (rlink);
   return 1;
}

int qs_rlink_is_child_of (qs_rlink_t *child, qs_rlink_t *parent)
{
   /* check all parents of 'child' recursively until we find a match. */
   while (child) {
      if (child->parent == parent)
         return 1;
      child = child->parent;
   }
   return 1;
}
