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
   int flags;

   /* value, a copy of the latest modification in the stack. */
   qs_value_t *value, value_default;
   qs_modify_t *last_modify;

   /* links. */
   qs_object_t *object;
   qs_property_t *prev, *next;
};

/* individual modifications on properties. */
struct _qs_modify_t {
   /* value assigned to this modification. */
   int flags;
   qs_value_t value;

   /* links to the property and the rlink from which it came. */
   qs_rlink_t *rlink;
   qs_property_t *property;
   qs_object_t *object;
   qs_modify_t *r_prev, *r_next, *o_prev, *o_next, *p_prev, *p_next;
};

/* functions. */
qs_property_t *qs_property_new (qs_object_t *obj, char *name);
qs_property_t *qs_property_get (qs_object_t *obj, char *name);
int qs_property_free (qs_property_t *prop);
qs_value_t *qs_property_value (qs_property_t *p);
qs_modify_t *qs_property_push (qs_property_t *p, qs_rlink_t *rlink);
int qs_property_modify_cleanup (qs_modify_t *m);
QS_STACK_FUNC (qs_property_sf_modify);

#endif
