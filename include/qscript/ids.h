/* ids.h
 * -----
 * managing arrays that contain unique identifiers. */

#ifndef __QS_ID_H
#define __QS_ID_H

#include "defs.h"

/* id manager. */
struct _qs_id_manager_t {
   void *objects[QS_ID_MAX];
   qs_id_t last_id;
   int count;
};

/* useful generic id functions. */
int qs_id_next (qs_id_manager_t *manager);
qs_id_t qs_id_assign (qs_id_manager_t *manager, void *obj, qs_id_t *id_ref);
int qs_id_remove (qs_id_manager_t *manager, void *obj, qs_id_t *id_ref);
void *qs_id_get (qs_id_manager_t *manager, qs_id_t id);

#endif
