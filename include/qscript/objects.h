/* objects.h
 * ---------
 * qscript base objects on which qscripts are executed. */

#ifndef __QS_OBJECTS_H
#define __QS_OBJECTS_H

#include "defs.h"

/* qscript object type. */
struct _qs_object_t {
   /* identifing info. */
   char *name;
   qs_id_t id;
   qs_flags_t flags;
   qs_version_t version;

   /* rlinks and property lists. */
   qs_rlink_t *rlink_list_front, *rlink_list_back;
   qs_property_t *property_list_front, *property_list_back;
   int property_count;
   qs_modify_t *last_modify;

   /* link info. */
   qs_resource_t *resource;
   qs_scheme_t *scheme;
   qs_object_t *prev, *next;
};

/* function declarations. */
qs_object_t *qs_object_new (qs_scheme_t *scheme, char *name);
qs_object_t *qs_object_instantiate (qs_resource_t *rsrc, char *name);
qs_object_t *qs_object_instantiate_auto (qs_resource_t *rsrc);
qs_object_t *qs_object_new_base (qs_scheme_t *scheme, char *name);
qs_object_t *qs_object_get_by_id (qs_scheme_t *scheme, qs_id_t id);
int qs_object_instantiate_finish (qs_object_t *obj, qs_resource_t *rsrc);
qs_object_t *qs_object_new_base (qs_scheme_t *scheme, char *name);
int qs_object_free (qs_object_t *object);
qs_object_t *qs_object_get (qs_scheme_t *scheme, char *name);

#endif
