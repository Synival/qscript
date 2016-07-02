/* print.c
 * -------
 * printing qscripts out to stdout. */

#include <stdio.h>

#include "actions.h"
#include "language.h"
#include "lists.h"
#include "resources.h"
#include "values.h"

#include "print.h"

int qs_print_value (qs_value_t *v, int indent)
{
   if (v == NULL)
      return 0;
   int count = 1;
   switch (v->type_id) {
      case QSCRIPT_CHAR:
         printf ("'\x1b[0;37;1m%s\x1b[0m'", v->val_s);
         break;
      case QSCRIPT_STRING:
         if (v->child && v->child->type_id == QSCRIPT_ACTION &&
             ((qs_action_t *) v->child->val_p)->type_id == QS_ACTION_CALL)
            printf ("\x1b[0;33;1m%s\x1b[0m", v->val_s);
         else
            printf ("\x1b[0;35m\"\x1b[1m%s\x1b[0;35m\"\x1b[0m", v->val_s);
         break;
      case QSCRIPT_FLOAT:
      case QSCRIPT_INT:
         printf ("\x1b[0;35;1m%s\x1b[0m", v->val_s);
         break;
      case QSCRIPT_VARIABLE:
         switch (v->val_i) {
            case QS_SCOPE_RLINK:
               printf ("\x1b[0;36m$$\x1b[1m%s\x1b[0m", v->val_s);
               break;
            case QS_SCOPE_BLOCK:
               printf ("\x1b[0;36m$\x1b[1m%s\x1b[0m", v->val_s);
               break;
         }
         break;
      case QSCRIPT_PROPERTY:
         printf ("\x1b[0;34;1m.%s\x1b[0m", v->val_s);
         break;
      case QSCRIPT_LIST:
         putchar ('[');
         count += qs_print_list (v->val_p, indent);
         putchar (']');
         break;
      case QSCRIPT_BLOCK:
         printf ("{\n");
         count += qs_print_list (v->val_p, indent + 3);
         printf ("%*s}", indent, "");
         break;
      case QSCRIPT_ACTION:
         count += qs_print_action (v->val_p, indent);
         break;
      case QSCRIPT_UNDEFINED:
         printf ("\x1b[0;31;1mundefined\x1b[0m");
         break;
      case QSCRIPT_OBJECT:
         printf ("\x1b[0;32m@\x1b[1m%s\x1b[0m", (char *) v->data);
         break;
      default:
         printf ("\x1b[0;31;1m<unknown value>\x1b[0m");
         break;
   }
   return count + qs_print_value (v->child, indent);
}

int qs_print_action (qs_action_t *a, int indent)
{
   switch (a->type_id) {
      case QS_ACTION_CALL:
         printf (" (");
         qs_print_list (a->data_p, indent + 3);
         printf (")");
         break;
      case QS_ACTION_INDEX:
         printf ("[");
         qs_print_value (a->data_p, indent + 3);
         printf ("]");
         break;
      case QS_ACTION_PROPERTY: {
         qs_value_t *val = a->data_p;
         if (val->type_id == QSCRIPT_STRING && val->child == NULL)
            printf ("\x1b[0;34;1m.%s\x1b[0m", val->val_s);
         else {
            printf ("\x1b[0;34;1m.\x1b[0m");
            qs_print_value (val, indent);
         }
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
      if (l->type_id == QSCRIPT_BLOCK)
         printf ("%*s", indent, "");
      else if (i > 0)
         printf (", ");
      count += qs_print_value (v, indent);
      if (l->type_id == QSCRIPT_BLOCK)
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
   if (r->flags & QS_RSRC_AUTO_INSTANTIATE) flag_str[len++] = '*';
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
