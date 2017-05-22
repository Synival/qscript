/* properties.h
 * ------------
 * values assigned to objects, modifiable via stack. */

#ifndef __QS_PROPERTIES_H
#define __QS_PROPERTIES_H

#include "defs.h"

#include "values.h"

/* properties assigned to objects, modified from rlinks. */
struct _qs_property_t {
   /* name of this property. */
   char *name;
   qs_flags_t flags;

   /* value, a copy of the latest modification in the stack. */
   qs_value_t *value, value_default;
   qs_modify_t *last_modify;

   /* links. */
   qs_object_t *object;
   qs_property_t *prev, *next;
};

/* functions. */
qs_property_t *qs_property_new (qs_object_t *obj, char *name);
qs_property_t *qs_property_get (qs_object_t *obj, char *name);
int qs_property_free (qs_property_t *prop);
qs_value_t *qs_property_value (qs_property_t *p);
qs_modify_t *qs_property_push (qs_property_t *p, qs_rlink_t *rlink);
QS_MODIFY_FUNC (qs_property_modify_pop);

/* convenient property retrieval functions. */
qs_value_t *qs_pgetv (qs_object_t *obj, char *name);
char *qs_pgets (qs_object_t *obj, char *name);
int qs_pgeti (qs_object_t *obj, char *name);
float qs_pgetf (qs_object_t *obj, char *name);
void *qs_pgetp (qs_object_t *obj, char *name);
qs_list_t *qs_pget_list (qs_object_t *obj, char *name);

#endif
