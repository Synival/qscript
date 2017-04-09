/* print.c
 * -------
 * printing qscripts out to stdout. */

#include <stdio.h>
#include <stdarg.h>

#include "qscript/actions.h"
#include "qscript/language.h"
#include "qscript/lists.h"
#include "qscript/objects.h"
#include "qscript/properties.h"
#include "qscript/resources.h"
#include "qscript/values.h"

#include "qscript/print.h"

int qs_print_value (qs_value_t *v, int indent)
{
   if (v == NULL)
      return 0;
   int count = 1;
   switch (v->value_id) {
      /* simple real primitives. */
      case QS_VALUE_UNDEFINED:
         printf ("\x1b[0;31;1mundefined\x1b[0m");
         break;
      case QS_VALUE_INT:
      case QS_VALUE_FLOAT:
         printf ("\x1b[0;35;1m%s\x1b[0m", v->val_s);
         break;
      case QS_VALUE_STRING:
         if (v->action_list && v->action_list->action_id == QS_ACTION_CALL)
            printf ("\x1b[0;33;1m%s\x1b[0m", v->val_s);
         else
            printf ("\x1b[0;35m\"\x1b[1m%s\x1b[0;35m\"\x1b[0m", v->val_s);
         break;

      /* complex real primitives. */
      case QS_VALUE_CHAR:
         printf ("'\x1b[0;37;1m%s\x1b[0m'", v->val_s);
         break;
      case QS_VALUE_LIST:
         putchar ('[');
         count += qs_print_list (v->val_p, indent);
         putchar (']');
         break;
      case QS_VALUE_OBJECT: {
         char *name = v->data;
         if (*name == '@')
            printf ("\x1b[0;32m@@\x1b[1m%s\x1b[0m", name + 1);
         else
            printf ("\x1b[0;32m@\x1b[1m%s\x1b[0m", name);
         break;
      }

      /* abstract primitives. */
      case QS_VALUE_BLOCK:
         printf ("{\n");
         count += qs_print_list (v->val_p, indent + 3);
         printf ("%*s}", indent, "");
         break;
      case QS_VALUE_VARIABLE:
         switch (v->val_i) {
            case QS_SCOPE_RLINK:
               printf ("\x1b[0;36m$$\x1b[1m%s\x1b[0m", v->val_s);
               break;
            case QS_SCOPE_BLOCK:
               printf ("\x1b[0;36m$\x1b[1m%s\x1b[0m", v->val_s);
               break;
         }
         break;
      case QS_VALUE_PROPERTY:
         printf ("\x1b[0;34;1m.%s\x1b[0m", v->val_s);
         break;
      case QS_VALUE_PROPERTY_VALUE:
         printf ("\x1b[0;34;1m.(\x1b[0m");
         count += qs_print_value (v->val_p, indent);
         printf ("\x1b[0;34;1m)\x1b[0m");
         break;

      /* unhandled value type. */
      default:
         printf ("\x1b[0;31;1m<unknown value>\x1b[0m");
         break;
   }
   qs_action_t *a;
   for (a = v->action_list; a != NULL; a = a->next)
      count += qs_print_action (a, indent);
   return count;
}

int qs_print_action (qs_action_t *a, int indent)
{
   int count = 1;
   switch (a->action_id) {
      case QS_ACTION_CALL:
         printf (" (");
         count += qs_print_list (a->data_p, indent + 3);
         printf (")");
         break;
      case QS_ACTION_INDEX:
         printf ("[");
         count += qs_print_value (a->data_p, indent + 3);
         printf ("]");
         break;
      case QS_ACTION_PROPERTY: {
         char *val = a->data_p;
         printf ("\x1b[0;34;1m.%s\x1b[0m", val);
         break;
      }
      case QS_ACTION_PROPERTY_VALUE: {
         printf ("\x1b[0;34;1m.(\x1b[0m");
         count += qs_print_value (a->data_p, indent);
         printf ("\x1b[0;34;1m)\x1b[0m");
         break;
      }
      default:
         printf (" \x1b[0;31;1m<unknown action>\x1b[0m");
         break;
   }
   return 1;
}

int qs_print_list (qs_list_t *l, int indent)
{
   qs_value_t *v;
   int i, count = 0;

   /* print all parameters in this list. */
   for (i = 0; i < l->value_count; i++) {
      v = l->values[i];
      if (l->list_id == QS_LIST_BLOCK)
         printf ("%*s", indent, "");
      else if (i > 0)
         printf (", ");
      count += qs_print_value (v, indent);
      if (l->list_id == QS_LIST_BLOCK)
         printf (";\n");
   }

   /* return the number of parameters printed. */
   return count;
}

int qs_print_resource (qs_resource_t *r)
{
   char flag_str[16];
   int len;

   /* build a string containing our flags. */
   len = 0;
   if (r->flags & QS_RESOURCE_GLOBAL) flag_str[len++] = '*';
   flag_str[len] = '\0';

   /* print our resource title. */
   if (len > 0)
      printf ("\x1b[0;32;1m%s\x1b[37m%s\x1b[0m {\n", flag_str, r->name);
   else
      printf ("\x1b[0;37;1m%s\x1b[0m {\n", r->name);

   /* print our block. */
   int rval = qs_print_list (r->block, 3);
   printf ("}\n");
   return rval;
}

size_t qs_print_object_json_append (char *out, size_t size, size_t len,
   int indent, char *format, ...)
{
   if (indent > 0 && (len == 0 || out[len - 1] == '\n'))
      len += snprintf (out + len, size - len, "%*s", indent * 3, "");

   /* print to our buffer. */
   va_list args;
   va_start (args, format);
   len += vsnprintf (out + len, size - len, format, args);
   va_end (args);

   return len;
}

size_t qs_print_object_json_list (qs_list_t *list, JSON_ARGS)
{
   int i;
   JSON_APPEND ("[\n");
   indent++;
   for (i = 0; i < list->value_count; i++) {
      len = qs_print_object_json_value (list->values[i], JSON_ARGS_OUT);
      JSON_APPEND ("%s\n", i + 1 < list->value_count ? "," : "");
   }
   indent--;
   JSON_APPEND ("]");
   return len;
}

size_t qs_print_object_json_value (qs_value_t *val, JSON_ARGS)
{
   switch (val->value_id) {
      case QS_VALUE_UNDEFINED:
         JSON_APPEND ("null");
         break;

      case QS_VALUE_INT:
      case QS_VALUE_FLOAT:
         JSON_APPEND ("%s", val->val_s);
         break;

      case QS_VALUE_STRING:
      case QS_VALUE_CHAR:
         JSON_APPEND ("\"%s\"", val->val_s);
         break;

      case QS_VALUE_LIST: {
         qs_list_t *list = val->val_p;
         len = qs_print_object_json_list (list, JSON_ARGS_OUT);
         break;
      }

      /* TODO: recursive nightmare! */
      case QS_VALUE_OBJECT: {
         qs_object_t *obj = qs_value_object_scheme (val->scheme, val);
         if (obj)
            JSON_APPEND ("\"__obj_%d\"", obj->id);
         else
            JSON_APPEND ("\"__obj_no_object\"");
         break;
      }

      default:
         JSON_APPEND ("(unhandled '%d')", val->value_id);
   }
   return len;
}

size_t qs_print_object_json_object (qs_object_t *obj, JSON_ARGS)
{
   JSON_APPEND ("{\n");
   qs_property_t *p;
   indent++;

   /* internal properties. */
   JSON_APPEND ("\"__id\": %d,\n", obj->id);
   JSON_APPEND ("\"__name\": \"%s\"%s\n", obj->name,
      obj->property_list_front ? "," : "");

   /* user-defined properties. */
   for (p = obj->property_list_front; p != NULL; p = p->next) {
      JSON_APPEND ("\"%s\": ", p->name);
      len = qs_print_object_json_value (p->value, JSON_ARGS_OUT);
      JSON_APPEND ("%s\n", p->next ? "," : "");
   }
   indent--;
   JSON_APPEND ("}");

   return len;
}

char *qs_print_object_json (qs_object_t *obj, char *out, size_t size)
{
   int indent = 0;
   size_t len = 0;
   len = qs_print_object_json_object (obj, JSON_ARGS_OUT);
   return out;
}
