/* resources.c
 * -----------
 * qscript resource management. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "qscript/files.h"
#include "qscript/ids.h"
#include "qscript/language.h"
#include "qscript/link.h"
#include "qscript/objects.h"
#include "qscript/parser.h"
#include "qscript/rlinks.h"
#include "qscript/schemes.h"

#include "qscript/resources.h"

qs_resource_t *qs_resource_new (qs_file_t *file, char *name,
                                qs_list_t *block, qs_flags_t flags)
{
   qs_resource_t *new;

   /* allocate our reosurce and assign data. */
   new = calloc (1, sizeof (qs_resource_t));
   new->scheme = file->scheme;
   new->file   = file;
   new->name   = strdup (name);
   new->block  = block;
   new->flags  = flags;

   /* assign an id and link to both the scheme and the file. */
   QS_LINK_BACK       (file->scheme, new, resource);
   QS_LINK_BACK_EXTRA (file,         new, resource, file);

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
   else if (rsrc->flags & QS_RESOURCE_GLOBAL)
      return qs_object_instantiate_auto (rsrc);
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

   /* free all rlinks applied from this resource. */
   while (resource->rlink_list_back)
      qs_rlink_eject (resource->rlink_list_back);

   /* unlink from node. */
   if (resource->node)
      resource->node->data = NULL;

   /* unlink from the scheme and file resource list. */
   QS_UNLINK       (resource->scheme, resource, resource);
   QS_UNLINK_EXTRA (resource->file,   resource, resource, file);

   /* free our own memory. */
   if (resource->name)
      free (resource->name);
   free (resource);

   /* return success. */
   return 1;
}
