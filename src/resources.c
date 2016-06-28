/* resources.c
 * -----------
 * qscript resource management. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ids.h"
#include "language.h"
#include "link.h"
#include "objects.h"
#include "parser.h"
#include "schemes.h"

#include "resources.h"

qs_resource_t *qs_resource_new (qs_scheme_t *scheme, char *name,
                                qs_list_t *block, int flags)
{
   qs_resource_t *new;

   /* allocate our reosurce and assign data. */
   new = malloc (sizeof (qs_resource_t));
   memset (new, 0, sizeof (qs_resource_t));
   new->scheme = scheme;
   new->name   = strdup (name);
   new->block  = block;
   new->flags  = flags;

   /* assign an id and link. */
   QS_LINK_BACK (scheme, new, resource);

   /* return our new resource. */
   return new;
}

qs_object_t *qs_resource_get_auto_instance (qs_resource_t *rsrc)
   { return qs_object_get_by_id (rsrc->scheme, rsrc->auto_id); }

qs_object_t *qs_resource_auto_instance (qs_resource_t *rsrc)
{
   qs_object_t *obj;
   if ((obj = qs_resource_get_auto_instance (rsrc)) != NULL)
      return obj;
   else if (rsrc->flags & QS_RSRC_AUTO_INSTANTIATE)
      return qs_object_new_auto (rsrc);
   else
      return NULL;
}

qs_resource_t *qs_resource_get_by_id (qs_scheme_t *scheme, qs_id_t id)
   { return qs_id_get (&(scheme->resource_ids), id); }
qs_resource_t *qs_resource_get (qs_scheme_t *scheme, char *name)
{
   qs_resource_t *r;
   for (r = scheme->resource_list_front; r != NULL; r = r->next)
      if (r->scheme == scheme)
         if (strcmp (r->name, name) == 0)
            return r;
   return NULL;
}

int qs_resource_free (qs_resource_t *resource)
{
   /* free auto-instantiated object. */
   qs_object_t *obj;
   if ((obj = qs_resource_get_auto_instance (resource)) != NULL)
      qs_object_free (obj);

   /* TODO: free all rlinks applied from this resource. */

   /* unlink from node. */
   if (resource->node)
      resource->node->data = NULL;

   /* unlink from the scheme's resource list. */
   QS_UNLINK (resource->scheme, resource, resource);

   /* free our own memory. */
   if (resource->name)
      free (resource->name);
   free (resource);

   /* return success. */
   return 1;
}
