/* ids.c
 * -----
 * managing arrays that contain unique identifiers. */

#include <stdio.h>

#include "qscript/ids.h"

int qs_id_next (qs_id_manager_t *manager)
{
   qs_id_t first = 0, i;
   for (i = manager->last_id + 1; 1; ++i) {
      if ((i - 1) >= QS_ID_MAX)
         i = 1;
      if (first == i)
         return 0;
      else if (manager->objects[i - 1] == NULL) {
         manager->last_id = i;
         return i;
      }
      else if (first == 0)
         first = i;
   }
}

qs_id_t qs_id_assign (qs_id_manager_t *manager, void *obj, qs_id_t *id_ref)
{
   qs_id_t id;
   if ((id = qs_id_next (manager)) == 0)
      return 0;
   manager->objects[id - 1] = obj;
   manager->count++;
   *id_ref = id;
   return id;
}

int qs_id_remove (qs_id_manager_t *manager, void *obj, qs_id_t *id_ref)
{
   if (*id_ref <= 0 || *id_ref >= (QS_ID_MAX + 1))
      return 0;
   if (manager->objects[(*id_ref) - 1] != obj)
      return -1;
   manager->objects[*id_ref - 1] = NULL;
   manager->count--;
   *id_ref = 0;
   return 1;
}

void *qs_id_get (qs_id_manager_t *manager, qs_id_t id)
{
   if (id <= 0 || id >= (QS_ID_MAX + 1))
      return NULL;
   return manager->objects[id - 1];
}
