/* rlinks.c
 * --------
 * resource linking with objects. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "actions.h"
#include "funcs.h"
#include "language.h"
#include "link.h"
#include "objects.h"
#include "resources.h"
#include "schemes.h"
#include "variables.h"

#include "rlinks.h"

qs_rlink_t *qs_rlink_push (qs_object_t *obj, qs_resource_t *resource,
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
   return qs_rlink_push_at (obj, resource, priority, NULL, rl, NULL, NULL);
}

qs_rlink_t *qs_rlink_push_at (qs_object_t *obj, qs_resource_t *resource,
   int priority, qs_rlink_t *parent, qs_rlink_t *prev, qs_execute_t *exe,
   qs_action_t *action)
{
   qs_rlink_t *new;

   /* allocate an empty rlink. */
   new = malloc (sizeof (qs_rlink_t));
   memset (new, 0, sizeof (qs_rlink_t));
   new->resource = resource;
   new->object   = obj;
   new->scheme   = resource->scheme;

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

int qs_rlink_pop (qs_rlink_t *rlink)
{
   /* TODO: unwind! */

   /* free children first. */
   while (rlink->child_back)
      qs_rlink_pop (rlink->child_back);

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
