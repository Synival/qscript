/* properties.c
 * ------------
 * values assigned to objects, modifiable via stack. */

#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include "qscript/modify.h"
#include "qscript/objects.h"
#include "qscript/parser.h"
#include "qscript/rlinks.h"
#include "qscript/values.h"

#include "qscript/properties.h"

qs_property_t *qs_property_new (qs_object_t *obj, char *name)
{
   /* create a new property. */
   qs_property_t *new = calloc (1, sizeof (qs_property_t));
   new->name = strdup (name);

   /* set a default uninitialized value. */
   new->value_default.scheme  = obj->scheme;
   new->value_default.link_id = QS_LINK_PROPERTY;
   new->value_default.link    = new;
   new->value_default.flags  |= QS_VALUE_MUTABLE;
   qs_value_copy (NULL, &(new->value_default), QSV_UNDEFINED);
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
   qs_modify_t *new = qs_modify_push (rlink, p, &(p->last_modify),
      sizeof (qs_value_t), qs_property_modify_pop);

   /* initialize the value.  make it a mutable property, with the same value
    * as the last stack value. */
   qs_value_t *value = new->data;
   value->scheme  = rlink->scheme;
   value->link_id = QS_LINK_PROPERTY;
   value->link    = p;
   value->flags  |= QS_VALUE_MUTABLE;
   qs_value_copy (NULL, value, p->value);

   /* make sure our property is using this new value. */
   p->value = value;

   /* return our new modification. */
   return new;
}

QS_MODIFY_FUNC (qs_property_modify_pop)
{
   qs_property_t *property = link;
   qs_value_t *value       = data;
   qs_value_t *prev_value  = prev_data;

   /* make sure our property points to the proper value. */
   if (prev_value) property->value = prev_value;
   else            property->value = &(property->value_default);
   qs_value_cleanup (value);
   return 1;
}

qs_value_t *qs_pgetv (qs_object_t *obj, char *name)
   { return qs_property_value (qs_property_get (obj, name)); }
char *qs_pgets (qs_object_t *obj, char *name)
   { return qs_pgetv (obj, name)->val_s; }
int qs_pgeti (qs_object_t *obj, char *name)
   { return qs_pgetv (obj, name)->val_i; }
float qs_pgetf (qs_object_t *obj, char *name)
   { return qs_pgetv (obj, name)->val_f; }
void *qs_pgetp (qs_object_t *obj, char *name)
   { return qs_pgetv (obj, name)->val_p; }
qs_list_t *qs_pget_list (qs_object_t *obj, char *name) {
   qs_value_t *value = qs_pgetv (obj, name);
   return (value->value_id == QS_VALUE_LIST) ? value->data : NULL;
}
