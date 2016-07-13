/* print.h
 * -------
 * printing qscripts out to stdout. */

#ifndef __QS_PRINT_H
#define __QS_PRINT_H

#include "defs.h"

/* printing functions. */
int qs_print_value (const qs_value_t *v, int indent);
int qs_print_action (const qs_action_t *a, int indent);
int qs_print_list (const qs_list_t *l, int indent);
int qs_print_resource (const qs_resource_t *r);

#endif
