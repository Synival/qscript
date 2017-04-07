/* print.h
 * -------
 * printing qscripts out to stdout. */

#ifndef __QS_PRINT_H
#define __QS_PRINT_H

#include "defs.h"

/* printing functions. */
int qs_print_value (qs_value_t *v, int indent);
int qs_print_action (qs_action_t *a, int indent);
int qs_print_list (qs_list_t *l, int indent);
int qs_print_resource (qs_resource_t *r);

#endif
