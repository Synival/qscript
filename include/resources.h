/* resources.h
 * -----------
 * qscript resource management. */

#ifndef __QS_RESOURCES_H
#define __QS_RESOURCES_H

#include "defs.h"

/* structures. */
struct _qs_resource_t {
   qs_scheme_t *scheme;
   p_node_t *node;
   qs_id_t id, auto_id;
   qs_flags_t flags;
   char *name;
   qs_list_t *block;
   qs_rlink_t *rlink_list_front, *rlink_list_back;
   qs_resource_t *prev, *next;
};

/* function declarations. */
qs_resource_t *qs_resource_new (qs_scheme_t *scheme, char *name,
                                qs_list_t *block, qs_flags_t flags);
qs_resource_t *qs_resource_get_by_id (qs_scheme_t *scheme, qs_id_t id);
qs_resource_t *qs_resource_get (qs_scheme_t *scheme, char *name);
qs_object_t *qs_resource_get_auto_instance (qs_resource_t *rsrc);
qs_object_t *qs_resource_auto_instance (qs_resource_t *rsrc);
int qs_resource_free (qs_resource_t *resource);

#endif
