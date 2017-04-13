/* resources.h
 * -----------
 * qscript resource management. */

#ifndef __QS_RESOURCES_H
#define __QS_RESOURCES_H

#include "defs.h"

/* structures. */
struct _qs_resource_t {
   /* internal data and code. */
   char *name;
   qs_flags_t flags;
   p_node_t *node;
   qs_list_t *block;

   /* run-time information. */
   qs_rlink_t *rlink_list_front, *rlink_list_back;

   /* links. */
   qs_id_t id, auto_id;
   qs_scheme_t *scheme;
   qs_resource_t *prev, *next;
   qs_file_t *file;
   qs_resource_t *file_prev, *file_next;
};

/* function declarations. */
qs_resource_t *qs_resource_new (qs_file_t *file, char *name,
                                qs_list_t *block, qs_flags_t flags);
qs_resource_t *qs_resource_get_by_id (qs_scheme_t *scheme, qs_id_t id);
qs_resource_t *qs_resource_get (qs_scheme_t *scheme, char *name);
qs_object_t *qs_resource_get_auto_instance (qs_resource_t *rsrc);
qs_object_t *qs_resource_auto_instance (qs_resource_t *rsrc);
int qs_resource_free (qs_resource_t *resource);

#endif
