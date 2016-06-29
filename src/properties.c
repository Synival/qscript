/* properties.c
 * ------------
 * values assigned to objects, modifiable via stack. */

#include <signal.h>

#include "link.h"
#include "objects.h"
#include "resources.h"
#include "rlinks.h"
#include "stacks.h"
#include "values.h"

#include "properties.h"

qs_property_t *qs_property_new (qs_object_t *obj, char *name)
{
   /* create a new property. */
   qs_property_t *new = malloc (sizeof (qs_property_t));
   memset (new, 0, sizeof (qs_property_t));
   new->name = strdup (name);

   /* set a default uninitialized value. */
   new->value_default.link_id = QS_LINK_PROPERTY;
   new->value_default.link    = new;
   new->value_default.flags  |= QS_VALUE_MUTABLE;
   qs_value_copy (&(new->value_default), QSV_UNDEFINED);
   new->value = &(new->value_default);

   /* link to the object. */
   new->object = obj;
   new->prev = obj->property_list_back;
   if (new->prev)
      new->prev->next = new;
   else
      obj->property_list_front = new;
   obj->property_list_back = new;
   obj->property_count++;

   /* return our new property. */
   return new;
}

qs_property_t *qs_property_get (qs_object_t *obj, char *name)
{
   qs_property_t *p;

   /* first, look for a matching property... */
   for (p = obj->property_list_front; p != NULL; p = p->next)
      if (strcmp (p->name, name) == 0)
         return p;

   /* second, if it doesn't exist, create it. */
   return qs_property_new (obj, name);
}

int qs_property_free (qs_property_t *prop)
{
   /* toss a big warning if our property is not the default one,
    * meaning there are still modifications we don't know about. */
   if (prop->value != &(prop->value_default) || prop->last_modify) {
      p_error (prop->value->node, "BAD ERROR: attempted to free property "
         "'%s' (set to '%s') in object '%s' (%#d) before modifications were "
         "popped.\n", prop->name, prop->value->val_s, prop->object->name,
         prop->object->id);
      raise (SIGINT);
      return 0;
   }

   /* free our data. */
   qs_value_cleanup (&(prop->value_default));
   if (prop->name)
      free (prop->name);

   /* unlink from object. */
   if (prop->prev) prop->prev->next       = prop->next;
   else prop->object->property_list_front = prop->next;
   if (prop->next) prop->next->prev       = prop->prev;
   else prop->object->property_list_back  = prop->prev;
   prop->object->property_count--;

   /* free property and return success. */
   free (prop);
   return 1;
}

qs_value_t *qs_property_value (qs_property_t *p)
   { return p->value; }

qs_modify_t *qs_property_push (qs_property_t *p, qs_rlink_t *rlink)
{
   /* initialize an empty modification. */
   qs_modify_t *new = malloc (sizeof (qs_modify_t));
   memset (new, 0, sizeof (qs_modify_t));

   /* initialize the value.  make it a mutable property, with the same value
    * as the last stack value. */
   new->value.link_id = QS_LINK_PROPERTY;
   new->value.link    = p;
   new->value.flags  |= QS_VALUE_MUTABLE;
   qs_value_copy (&(new->value), p->value);

   /* make sure our property is using this new value. */
   p->value = &(new->value);

   /* link to the property... */
   new->p_prev = p->last_modify;
   if (new->p_prev)
      new->p_prev->p_next = new;
   p->last_modify = new;
   new->property = p;

   /* ...link to the rlink... */
   new->r_prev = qs_stack_last (rlink->stack_modify);
   if (new->r_prev)
      new->r_prev->r_next = new;
   qs_stack_push (rlink->stack_modify, new, qs_property_sf_modify);
   new->rlink = rlink;

   /* ...and link to the object. */
   new->o_prev = p->object->last_modify;
   if (new->o_prev)
      new->o_prev->o_next = new;
   p->object->last_modify = new;
   new->object = p->object;

   /* return our new modification. */
   return new;
}

int qs_property_modify_free (qs_modify_t *m)
{
   /* this should never, ever happen, but potentially could if programs get
    * crazy enough with unforeseen circumstances.  toss a big error, do nothing
    * unsafe, and bail. */
   if (m->p_next) {
      p_error (NULL, "BAD ERROR: attempted to free non-last property "
         "modification '%s' for property '%s' from '%s'@%d on '%s' (#%2d).\n",
         m->value.val_s, m->property->name, m->rlink->resource->name,
         m->rlink->priority, m->object->name, m->object->id);
      raise (SIGINT);
      return 0;
   }

   /* make sure our property points to the proper value. */
   if (m->p_prev) m->property->value = &(m->p_prev->value);
   else           m->property->value = &(m->property->value_default);
   qs_value_cleanup (&(m->value));

   /* unlink from neighbors.  the rlink should already handle itself
    * because it's using a qs_stack_t. */
   if (m->p_prev) m->p_prev->p_next = m->p_next;
   if (m->p_next) m->p_next->p_prev = m->p_prev;
   else m->property->last_modify    = m->p_prev;
   if (m->r_prev) m->r_prev->r_next = m->r_next;
   if (m->r_next) m->r_next->r_prev = m->r_prev;
   if (m->o_prev) m->o_prev->o_next = m->o_next;
   if (m->o_next) m->o_next->o_prev = m->o_prev;
   else m->object->last_modify      = m->o_prev;

   /* free ourselves and return success. */
   free (m);
   return 1;
}

QS_STACK_FUNC (qs_property_sf_modify)
{
   /* make sure we don't free values twice. */
   qs_modify_t *m = data;
   if (m->flags & QS_MODIFY_POPPING) {
      p_error (NULL, "BAD ERROR: double-free detected for property "
         "modification '%s' for property '%s' from '%s'@%d on '%s' (#%2d).\n",
         m->value.val_s, m->property->name, m->rlink->resource->name,
         m->rlink->priority, m->object->name, m->object->id);
      raise (SIGINT);
      return 0;
   }
   m->flags |= QS_MODIFY_POPPING;

   /* if we're not the last property modification, it (should) mean the
    * parent rlink did some further modifications. because the parent is
    * expected to be popped anway, pop its modifications now in order to
    * preserve stack order. */
   qs_modify_t *last;
   while ((last = m->p_next) != NULL) {
      /* the unwind process should be smart enough to know what needs to
       * be unwound, in the proper (reverse) order. by the time we're popping
       * modifications in the stack, the appropriate rlinks should already
       * be marked for unwinding. if that's not the case, there's a serious
       * error - report it. */
      if (last->rlink->flags & QS_RLINK_ON) {
         p_error (NULL, "BAD ERROR: modifier pop required for currently-wound "
            "'%s'@%d.  called from modification '%s' for property '%s' from "
            "'%s'@%d on '%s' (#%2d).\n", last->rlink->resource->name,
            last->rlink->priority, m->value.val_s, m->property->name,
            m->rlink->resource->name, m->rlink->priority, m->object->name,
            m->object->id);
         raise (SIGINT);
         return 0;
      }
      qs_stack_pop (last->rlink->stack_modify);
   }

   qs_property_modify_free (m);
   return 1;
}
