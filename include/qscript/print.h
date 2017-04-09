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

/* printing objects to json buffers. */
#define JSON_APPEND(...) \
   len = qs_print_object_json_append (out, size, len, indent, __VA_ARGS__)
#define JSON_OBJECT_MAX 64
#define JSON_ARGS \
   char *out, size_t size, size_t len, int indent
#define JSON_ARGS_OUT \
   out, size, len, indent

size_t qs_print_object_json_append (char *out, size_t size, size_t len,
   int indent, char *format, ...);
size_t qs_print_object_json_list (qs_list_t *list, JSON_ARGS);
size_t qs_print_object_json_value (qs_value_t *val, JSON_ARGS);
size_t qs_print_object_json_object (qs_object_t *obj, JSON_ARGS);
char *qs_print_object_json (qs_object_t *obj, char *out, size_t size);

#endif
