/* objects.c
 * ---------
 * qscript base objects on which qscripts are executed. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "link.h"
#include "execute.h"
#include "properties.h"
#include "resources.h"
#include "rlinks.h"
#include "schemes.h"
#include "variables.h"

#include "objects.h"

qs_object_t *qs_object_new (qs_resource_t *rsrc)
{
   qs_object_t *new;

   /* create an object of 'name'. */
   if (rsrc == NULL) {
      p_error (NULL, "attempted to instantiate object with no resource.\n");
      return NULL;
   }
   if ((new = qs_object_new_base (rsrc->scheme, rsrc->name)) == NULL)
      return NULL;

   /* finish and return our new object. */
   qs_object_new_finish (new, rsrc);
   return new;
}

qs_object_t *qs_object_new_auto (qs_resource_t *rsrc)
{
   char buf[256];
   qs_object_t *new;

   /* sanity checks. */
   if (rsrc == NULL) {
      p_error (NULL, "attempted to auto-instantiate object with no "
                     "resource.\n");
      return NULL;
   }

   /* create a resource with a matching name, prefixed by '*'. */
   snprintf (buf, sizeof (buf), "@%s", rsrc->name);
   if ((new = qs_object_new_base (rsrc->scheme, buf)) == NULL)
      return NULL;

   /* keep an id reference to our resource. */
   new->resource = rsrc;
   rsrc->auto_id = new->id;

   /* finish and return our new object. */
   qs_object_new_finish (new, rsrc);
   return new;
}

qs_object_t *qs_object_new_base (qs_scheme_t *scheme, char *name)
{
   qs_object_t *new;

   /* allocate our new object. */
   new = malloc (sizeof (qs_object_t));
   memset (new, 0, sizeof (qs_object_t));
   new->name = strdup (name);
   new->scheme = scheme;
   new->flags |= QS_OBJECT_GLOBAL;

   /* link to object list and assign id. */
   QS_LINK_BACK (scheme, new, object);

   /* return our new object. */
   return new;
}

int qs_object_new_finish (qs_object_t *obj, qs_resource_t *rsrc)
{
   /* link and run our resource. */
   qs_rlink_t *rl;

   /* run our rlink. */
   /* TODO: unwind! */
   if ((rl = qs_rlink_push (obj, rsrc, 0)) != NULL)
      qs_rlink_wind (rl, NULL);
   /* TODO: rewind! */

   return 1;
}

qs_object_t *qs_object_get_by_id (qs_scheme_t *scheme, qs_id_t id)
   { return qs_id_get (&(scheme->object_ids), id); }
qs_object_t *qs_object_get (qs_scheme_t *scheme, char *name)
{
   qs_object_t *o;
   if (*name >= '0' && *name <= '9')
      return qs_object_get_by_id (scheme, atoi (name + 1));
   for (o = scheme->object_list_front; o != NULL; o = o->next)
      if (o->scheme == scheme)
         if (strcmp (o->name, name) == 0)
            return o;
   return NULL;
}

int qs_object_free (qs_object_t *object)
{
   /* remove links to resources for auto-instantiated objects. */
   if (object->resource) {
      object->resource->auto_id = 0;
      object->resource = NULL;
   }

   /* unwind and pop all rlinks. */
   while (object->rlink_list_back)
      qs_rlink_pop (object->rlink_list_back);

   /* free any remaining variables. */
   while (object->variable_list_back)
      qs_variable_free (object->variable_list_back);

   /* free properties. */
   while (object->property_list_back)
      qs_property_free (object->property_list_back);

   /* unlink object from our scheme. */
   QS_UNLINK (object->scheme, object, object);

   /* free our own allocated memory. */
   if (object->name)
      free (object->name);
   free (object);

   /* return success. */
   return 1;
}
