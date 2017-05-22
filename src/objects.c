/* objects.c
 * ---------
 * qscript base objects on which qscripts are executed. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "qscript/link.h"
#include "qscript/hooks.h"
#include "qscript/execute.h"
#include "qscript/parser.h"
#include "qscript/properties.h"
#include "qscript/resources.h"
#include "qscript/rlinks.h"
#include "qscript/schemes.h"
#include "qscript/variables.h"

#include "qscript/objects.h"

qs_object_t *qs_object_new (qs_scheme_t *scheme, char *name)
   { return qs_object_new_base (scheme, name); }

qs_object_t *qs_object_instantiate (qs_resource_t *rsrc, char *name)
{
   qs_object_t *new;

   /* create an object of 'name'. */
   if (rsrc == NULL) {
      p_error (NULL, "attempted to instantiate object with no resource.\n");
      return NULL;
   }
   if ((new = qs_object_new_base (rsrc->scheme, name)) == NULL)
      return NULL;

   /* finish and return our new object. */
   qs_object_instantiate_finish (new, rsrc);
   return new;
}

qs_object_t *qs_object_instantiate_auto (qs_resource_t *rsrc)
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
   qs_object_instantiate_finish (new, rsrc);
   return new;
}

qs_object_t *qs_object_new_base (qs_scheme_t *scheme, char *name)
{
   qs_object_t *new;

   /* allocate our new object. */
   new = calloc (1, sizeof (qs_object_t));
   new->name = strdup (name);
   new->scheme = scheme;
   new->flags |= QS_OBJECT_GLOBAL;
   new->version = 1;

   /* link to object list and assign id. */
   QS_LINK_BACK (scheme, new, object);

   /* return our new object. */
   return new;
}

int qs_object_instantiate_finish (qs_object_t *obj, qs_resource_t *rsrc)
{
   /* link and run our resource. */
   qs_rlink_t *rl;

   /* add our rlink. */
   if ((rl = qs_rlink_inject (obj, rsrc, 0)) == NULL)
      return 0;

   /* call any hooks. */
   qs_hook_call (obj->scheme, QS_HOOK_OBJECT_NEW, obj, 1);

   /* return success. */
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
   /* call any hooks. */
   qs_hook_call (object->scheme, QS_HOOK_OBJECT_FREE, object, 1);

   /* remove links to resources for auto-instantiated objects. */
   if (object->resource) {
      object->resource->auto_id = 0;
      object->resource = NULL;
   }

   /* unwind and eject all rlinks. */
   while (object->rlink_list_back)
      qs_rlink_eject (object->rlink_list_back);

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
