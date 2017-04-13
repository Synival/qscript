/* standard.c
 * ----------
 * standard functions included in all language schemes. */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "qscript/actions.h"
#include "qscript/execute.h"
#include "qscript/funcs.h"
#include "qscript/language.h"
#include "qscript/lists.h"
#include "qscript/objects.h"
#include "qscript/print.h"
#include "qscript/properties.h"
#include "qscript/resources.h"
#include "qscript/rlinks.h"
#include "qscript/schemes.h"
#include "qscript/stacks.h"
#include "qscript/values.h"
#include "qscript/variables.h"

#include "qscript/standard.h"

qs_func_t qs_func_list_standard[] = {
   { "print",   "<text1> [... <textn>]",   0, qsf_print,   0 },
   { "echo",    "<text1> [... <textn>]",   0, qsf_print,   1 },
   { "+",       "<val1> [... <valn>]",     2, qsf_math,    0 },
   { "-",       "<val1> [... <valn>]",     2, qsf_math,    1 },
   { "*",       "<val1> [... <valn>]",     2, qsf_math,    2 },
   { "/",       "<val1> [... <valn>]",     2, qsf_math,    3 },
   { "**",      "<val1> [... <valn>]",     2, qsf_math,    4 },
   { "|",       "<val1> [... <valn>]",     2, qsf_math,    5 },
   { "&",       "<val1> [... <valn>]",     2, qsf_math,    6 },
   { "^",       "<val1> [... <valn>]",     2, qsf_math,    7 },
   { "%",       "<val1> [... <valn>]",     2, qsf_math,    8 },
   { "+=","<variable> <val1> [... <valn>]",2, qsf_math,    20 },
   { "-=","<variable> <val1> [... <valn>]",2, qsf_math,    21 },
   { "*=","<variable> <val1> [... <valn>]",2, qsf_math,    22 },
   { "/=","<variable> <val1> [... <valn>]",2, qsf_math,    23 },
   {"**=","<variable> <val1> [... <valn>]",2, qsf_math,    24 },
   { "|=","<variable> <val1> [... <valn>]",2, qsf_math,    25 },
   { "&=","<variable> <val1> [... <valn>]",2, qsf_math,    26 },
   { "^=","<variable> <val1> [... <valn>]",2, qsf_math,    27 },
   { "%=","<variable> <val1> [... <valn>]",2, qsf_math,    28 },
   { "if",      "<cond> <then> [<else>]",  2, qsf_if,      0 },
   { "==",      "<left> <right> [...]",    2, qsf_compare, 0 },
   { "!=",      "<left> <right> [...]",    2, qsf_compare, 1 },
   { ">",       "<left> <right> [...]",    2, qsf_compare, 2 },
   { ">=",      "<left> <right> [...]",    2, qsf_compare, 3 },
   { "<",       "<left> <right> [...]",    2, qsf_compare, 4 },
   { "<=",      "<left> <right> [...]",    2, qsf_compare, 5 },
   { "||",      "<left> <right> [...]",    2, qsf_boolean, 0 },
   { "&&",      "<left> <right> [...]",    2, qsf_boolean, 1 },
   { "^^",      "<left> <right> [...]",    2, qsf_boolean, 2 },
   { "!||",     "<left> <right> [...]",    2, qsf_boolean, 3 },
   { "!&&",     "<left> <right> [...]",    2, qsf_boolean, 4 },
   { "!^^",     "<left> <right> [...]",    2, qsf_boolean, 5 },
   { "?",       "<value> [<if!prev> [...]]",1,qsf_truth,   0 },
   { "??",      "<value1> <value2> [...]", 2, qsf_first,   0 },
   { "!",       "<value>",                 1, qsf_truth,   1 },
   { "inv",     "<value>",                 1, qsf_inv,     0 },
   { "return",  "[<value>]",               0, qsf_return,  0 },
   { "break",   "[<value>]",               0, qsf_return,  1 },
   { "continue","",                        0, qsf_return,  2 },
   { "=",       "<variable> <value>",      2, qsf_let,     0 },
   { "++",      "<variable>",              1, qsf_let,     1 },
   { "--",      "<variable>",              1, qsf_let,    -1 },
   { "type",    "<value>",                 1, qsf_type,    0 },
   { "scope",   "<value>",                 1, qsf_scope,   0 },
   { "index",   "<value>",                 1, qsf_index,   0 },
   { "while",   "<condition> <action>",    2, qsf_while,   0 },
   { "for",     "<init> <cond> <after> <action>",
                                           4, qsf_for,     0 },
   { "switch",  "<cond> [<value1> <action1> ... <valuen> <actionn>] "
                "[<default action>]",      1, qsf_switch,  0 },
   { "args",    "[<ref1> ... <ref2>]",     0, qsf_args,    0 },
   { "arg",     "<index>",                 1, qsf_arg,     0 },
   { "arg_list","",                        0, qsf_arg_list,0 },
   { "variable","<value>",                 1, qsf_variable,0 },
   { "property","<value>",                 1, qsf_property,1 },
   { "vars",    "<var1> [... <varn>]",     1, qsf_vars,    0 },
   { "run",     "<func> [<arg1> ... <argn>]",1,qsf_run,    0 },
   { "print_resource", "[<resource>]",     0, qsf_print_r, 0 },
   { "print_value",    "<value>",          1, qsf_print_v, 0 },
   { "eval",    "<arg1> [... <argn>]",     1, qsf_eval,    0 },
   { "length",  "<string | list>",         1, qsf_length,  0 },
   { "boolean", "<value>",                 1, qsf_cast,    QS_VALUE_BOOLEAN },
   { "int",     "<value>",                 1, qsf_cast,    QS_VALUE_INT },
   { "float",   "<value>",                 1, qsf_cast,    QS_VALUE_FLOAT },
   { "string",  "<value>",                 1, qsf_cast,    QS_VALUE_STRING },
   { "char",    "<value>",                 1, qsf_cast,    QS_VALUE_CHAR },
   { "object",  "<value>",                 1, qsf_cast,    QS_VALUE_OBJECT },
   { "this",    "",                        0, qsf_this,    0 },
   { "id",      "[<object>]",              0, qsf_id,      0 },
   { "name",    "[<object>]",              0, qsf_id,      1 },
   { "print_json", "[<object>]",           0, qsf_id,      2 },
   { "func_exists", "<value>",             1, qsf_func_exists, 0 },
   { "tokenize","<value>",                 1, qsf_tokenize,0 },
   { "multi",   "[<line1> ... <linen>]",   0, qsf_multi,   0 },
   { "for_each","<list> <storage> <action>",1,qsf_for_each,0 },
   { "new", "<name> [list(<resource> [<priority>])1 ... listn]",
                                           1, qsf_new,     0 },
   { "error",   "<string>",                1, qsf_error,   0 },
   { NULL },
};

QS_FUNC (qsf_print)
{
   char buf[65536];
   size_t len = 0;
   int i;

   /* build our print string and print it out. */
   buf[0] = '\0';
   for (i = 0; i < args; i++) {
      qs_value_t *v = QS_ARGV (i);
      len += snprintf (buf + len, sizeof (buf) - len, "%s", v->val_s);
   }

   /* 'print': no newline
    * 'echo':  newline */
   switch (sub_func) {
      case 0: printf ("%s",   buf); break;
      case 1: printf ("%s\n", buf); break;
   }
   fflush (stdout);

   /* return whatever string we just printed. */
   return QS_RETS (buf);
}

QS_FUNC (qsf_math)
{
   qs_value_t *val, *lval = NULL, *rval;
   int i, start, math_func;

   /* are we setting a value...? */
   qs_value_t *aval = QS_ARGV(0);
   if (sub_func >= 20) {
      /* get our lvalue.  can we modify it? */
      if ((lval = qs_value_lvalue (exe, aval)) == NULL) {
         QS_ARG_ERROR (0, "value '%s' cannot be modified.\n", aval->val_s);
         return QSV_CANNOT_MODIFY;
      }
      if (lval->value_id == QS_VALUE_UNDEFINED) {
         QS_ARG_ERROR (0, "reference is invalid type 'undefined'.\n");
         return QSV_INVALID_TYPE;
      }
      val = lval;
      math_func = sub_func - 20;
      start = 1;
   }
   /* ...or just doing math? */
   else {
      val = QS_ARGV (0);
      math_func = sub_func;
      start = 1;
   }

   /* perform different calculations based on type. */
   switch (val->value_id) {
      /* only '^' is allowed for booleans. */
      case QS_VALUE_BOOLEAN: {
         if (math_func != 7) {
            QS_ARG_ERROR (0, "invalid operation for 'boolean'.\n");
            return QSV_INVALID_OPER;
         }
         int v = val->val_i;
         for (i = start; i < args; i++)
            v ^= QS_ARGB (i);
         rval = QS_RETB (v);
         break;
      }

      /* only '+' is allowed for lists. */
      case QS_VALUE_LIST: {
         if (math_func != 0) {
            QS_ARG_ERROR (0, "invalid operation for 'list'.\n");
            return QSV_INVALID_OPER;
         }

         /* build a new list with the contents of val + every other arg. */
         qs_list_t *old_list = val->val_p;
         rval = QS_RETL (old_list->value_count + (args - start));
         qs_list_t *new_list = rval->val_p;

         /* copy links to all previous data. */
         int pos = 0;
         for (i = 0; i < old_list->value_count; i++)
            new_list->values[pos++] = old_list->values[i];

         /* add links to new data. */
         for (i = start; i < args; i++)
            new_list->values[pos++] = QS_ARGV (i);

         /* make sure our list has this data internalized. */
         qs_value_list_internalize (rval);
         break;
      }

      case QS_VALUE_CHAR: {
         qs_value_t *ival;
         int v = val->val_i;
         char rch;
         for (i = start; i < args; i++) {
            ival = QS_ARGV (i);
            if (!qs_value_as_char (ival, &rch)) {
               QS_ARG_ERROR (i, "cannot cast argument '%d' (%s) as 'char'.\n",
                             i, ival->val_s);
               return QSV_INVALID_VALUE;
            }
            switch (math_func) {
               case 0: v += (int) rch; break;
               case 1: v -= (int) rch; break;
               case 2: v *= (int) rch; break;
               case 3: {
                  if (rch == 0)
                     return QSV_DIVISION_BY_ZERO;
                  else
                     v /= (int) rch;
                  break;
               }
               case 4: v  = (int) powf (v, rch); break;
               case 5: v |= (int) rch; break;
               case 6: v &= (int) rch; break;
               case 7: v ^= (int) rch; break;
               case 8:
                  if (rch == 0)
                     return QSV_MOD_BY_ZERO;
                  else
                     v %= (int) rch;
                  break;
               default:
                  QS_ARG_ERROR (0, "invalid operation for 'char'.\n");
                  return QSV_INVALID_OPER;
            }
            if (v < 1 || v > 255) {
               QS_ARG_ERROR (i, "resulting char '%d' is out of range. "
                                "must be >= 1, <= 255.\n", v);
               return QSV_INVALID_VALUE;
            }
         }
         rval = QS_RETC (v);
         break;
      }

      case QS_VALUE_INT: {
         int v = val->val_i, r;
         for (i = start; i < args; i++) {
            r = QS_ARGI (i);
            switch (math_func) {
               case 0: v += r; break;
               case 1: v -= r; break;
               case 2: v *= r; break;
               case 3:
                  if (r == 0)
                     return QSV_DIVISION_BY_ZERO;
                  else
                     v /= r;
                  break;
               case 4: v = (int) powf (v, r); break;
               case 5: v |= r; break;
               case 6: v &= r; break;
               case 7: v ^= r; break;
               case 8:
                  if (r == 0)
                     return QSV_MOD_BY_ZERO;
                  else
                     v %= r;
                  break;
               default:
                  QS_ARG_ERROR (0, "invalid operation for 'int'.\n");
                  return QSV_INVALID_OPER;
            }
         }
         rval = QS_RETI (v);
         break;
      }

      case QS_VALUE_FLOAT: {
         float v = val->val_f, r;
         for (i = start; i < args; i++) {
            r = QS_ARGF (i);
            switch (math_func) {
               case 0: v += r; break;
               case 1: v -= r; break;
               case 2: v *= r; break;
               case 3:
                  if (r == 0.00f)
                     return QSV_DIVISION_BY_ZERO;
                  else
                     v /= r;
                  break;
               case 4: v = powf (v, r);  break;
               case 8:
                  if (r == 0.00f)
                     return QSV_MOD_BY_ZERO;
                  else
                     v = fmodf (v, r);
                  break;
               default:
                  QS_ARG_ERROR (0, "invalid operation for 'float'.\n");
                  return QSV_INVALID_OPER;
            }
         }
         rval = QS_RETF (v);
         break;
      }

      case QS_VALUE_STRING:
         if (math_func == 0) {
            char buf[65536];
            size_t len = 0;
            buf[0] = '\0';
            for (i = start - 1; i < args; i++)
               len += snprintf (buf + len, sizeof (buf) - len, "%s",
                                QS_ARGS (i));
            rval = QS_RETS (buf);
         }
         else if (math_func == 2) {
            char buf1[65536], buf2[65536];
            char *in = val->val_s, *out = buf1;
            size_t len;
            int j, mult;
            for (i = start; i < args; i++) {
               mult = QS_ARGI (i);
               len = 0;
               out[0] = '\0';
               for (j = 0; j < mult; j++) {
                  len += snprintf (out + len, sizeof (char) * 65536 - len,
                     "%s", in);
               }
               if (out == buf1) {
                  out = buf2;
                  in  = buf1;
               }
               else {
                  out = buf1;
                  in  = buf2;
               }
            }
            rval = QS_RETS (in);
         }
         else {
            QS_ARG_ERROR (0, "invalid operation for 'string'.\n");
            return QSV_INVALID_OPER;
         }
         break;

      default:
         QS_ARG_ERROR (0, "invalid type '%s'.\n", qs_value_type (val));
         return QSV_INVALID_TYPE;
   }

   /* set variables. */
   if (lval) {
      /* setting characters is complicated because they can belong to
       * a string... */
      if (val->value_id == QS_VALUE_CHAR && val->val_p) {
         if (val->val_p) {
            qs_value_t *v = val->val_p;
            if (val->data) {
               int index = *((int *) val->data);
               v->val_s[index] = rval->val_i;
            }
            else
               v->val_s[0] = rval->val_i;
            qs_value_update_from_string (v);
         }
      }
      /* for more cases, just copy the value over. */
      else
         qs_value_copy_const (exe, lval, rval);
   }

   /* herp derp */
   return rval;
}

QS_FUNC (qsf_if)
{
   int i;

   /* evaluate all '<condition> <then>' pairs until <condition> is true. */
   for (i = 0; i < args - 1; i += 2)
      if (qs_value_truth (exe, QS_ARGV (i)))
         return QS_ARGV (i + 1);

   /* none of them were true.  is there an '<else>'? */
   if (i < args)
      return QS_ARGV (i);
   /* nope.  undefined result. */
   else
      return QSV_UNDEFINED;
}

QS_FUNC (qsf_compare)
{
   int rvalue, i;
   qs_value_t *first_val, *val1, *val2;

   /* get our first value.  all comparisons will be determined by its type. */
   first_val = QS_ARGV (0);
   val1 = first_val;

   /* compare all arguments in a chain. */
   for (i = 1; i < args; i++, val1 = val2) {
      val2 = QS_ARGV (i);

      /* check for equality based on type. */
      switch (first_val->value_id) {
         /* boolean: ==, != */
         case QS_VALUE_BOOLEAN: {
            rvalue = qs_value_truth (exe, val2);
            switch (sub_func) {
               case 0: rvalue = (rvalue == val1->val_i); break;
               case 1: rvalue = (rvalue != val1->val_i); break;
               default:
                  QS_ARG_ERROR (i, "invalid operation on 'boolean'.\n");
                  return QSV_INVALID_OPER;
            }
            break;
         }

         /* char: ==, !=, >, >=, <, <= */
         case QS_VALUE_CHAR: {
            char *lch, rch;
            if ((lch = qs_value_char_pointer (val1)) == NULL) {
               QS_ARG_ERROR (i - 1, "couldn't get char for modification.\n");
               return QSV_INVALID_INDEX;
            }
            if (!qs_value_as_char (val2, &rch)) {
               QS_ARG_ERROR (i, "cannot cast rvalue '%s' of type '%s' "
                  "as a char.\n", val2->val_s, qs_value_type (val2));
               return QSV_INVALID_OPER;
            }
            switch (sub_func) {
               case 0: rvalue = (*lch == rch) ? 1 : 0; break;
               case 1: rvalue = (*lch != rch) ? 1 : 0; break;
               case 2: rvalue = (*lch >  rch) ? 1 : 0; break;
               case 3: rvalue = (*lch >= rch) ? 1 : 0; break;
               case 4: rvalue = (*lch <  rch) ? 1 : 0; break;
               case 5: rvalue = (*lch <= rch) ? 1 : 0; break;
               default:
                  QS_ARG_ERROR (i, "invalid operation on 'char'.\n");
                  return QSV_INVALID_OPER;
            }
            break;
         }

         /* string: ==, !=, >, >=, <, <= */
         case QS_VALUE_STRING:
            rvalue = strcmp (val1->val_s, val2->val_s);
            switch (sub_func) {
               case 0: rvalue = (rvalue == 0 ? 1 : 0); break;
               case 1: rvalue = (rvalue != 0 ? 1 : 0); break;
               case 2: rvalue = (rvalue >  0 ? 1 : 0); break;
               case 3: rvalue = (rvalue >= 0 ? 1 : 0); break;
               case 4: rvalue = (rvalue <  0 ? 1 : 0); break;
               case 5: rvalue = (rvalue <= 0 ? 1 : 0); break;
               default:
                  QS_ARG_ERROR (i, "invalid operation on 'string'.\n");
                  return QSV_INVALID_OPER;
            }
            break;

         /* int: ==, !=, >, >=, <, <= */
         case QS_VALUE_INT:
            switch (sub_func) {
               case 0: rvalue = (val1->val_i == val2->val_i) ? 1 : 0; break;
               case 1: rvalue = (val1->val_i != val2->val_i) ? 1 : 0; break;
               case 2: rvalue = (val1->val_i >  val2->val_i) ? 1 : 0; break;
               case 3: rvalue = (val1->val_i >= val2->val_i) ? 1 : 0; break;
               case 4: rvalue = (val1->val_i <  val2->val_i) ? 1 : 0; break;
               case 5: rvalue = (val1->val_i <= val2->val_i) ? 1 : 0; break;
               default:
                  QS_ARG_ERROR (i, "invalid operation on 'int'.\n");
                  return QSV_INVALID_OPER;
            }
            break;

         /* floats: ==, !=, >, >=, <, <= */
         case QS_VALUE_FLOAT:
            switch (sub_func) {
               case 0: rvalue = (val1->val_f == val2->val_f) ? 1 : 0; break;
               case 1: rvalue = (val1->val_f != val2->val_f) ? 1 : 0; break;
               case 2: rvalue = (val1->val_f >  val2->val_f) ? 1 : 0; break;
               case 3: rvalue = (val1->val_f >= val2->val_f) ? 1 : 0; break;
               case 4: rvalue = (val1->val_f <  val2->val_f) ? 1 : 0; break;
               case 5: rvalue = (val1->val_f <= val2->val_f) ? 1 : 0; break;
               default:
                  QS_ARG_ERROR (i, "invalid operation on 'float'.\n");
                  return QSV_INVALID_OPER;
            }
            break;

         /* objects: ==, != (only with other objects) */
         case QS_VALUE_OBJECT: {
            /* filter out bad comparisons early. */
            if (sub_func != 0 && sub_func != 1) {
               QS_ARG_ERROR (i, "invalid operation on 'object'.\n");
               return QSV_INVALID_OPER;
            }

            /* forbid comparison of objects to non-objects. */
            if (val2->value_id != QS_VALUE_OBJECT) {
               QS_ARG_ERROR (i, "cannot compare object to type '%s'.\n",
                  qs_value_type (val2));
               return QSV_INVALID_TYPE;
            }

            /* update our object references. */
            qs_value_object (exe, val1);
            qs_value_object (exe, val2);

            /* if both ids are zero, compare the string. */
            if (val1->val_i == 0 && val2->val_i == 0)
               rvalue = (strcmp (val1->data, val2->data) == 0) ? 1 : 0;
            /* otherwise, compare ids. */
            else
               rvalue = (val1->val_i == val2->val_i) ? 1 : 0;
            /* flip the return value for != function. */
            if (sub_func == 1)
               rvalue ^= 1;
            break;
         }

         /* undefined: ==, != */
         case QS_VALUE_UNDEFINED: {
            switch (sub_func) {
               case 0:
                  rvalue = (val2->value_id == QS_VALUE_UNDEFINED) ? 1 : 0;
                  break;
               case 1:
                  rvalue = (val2->value_id == QS_VALUE_UNDEFINED) ? 0 : 1;
                  break;
               default:
                  QS_ARG_ERROR (i, "invalid operation on 'undefined'.\n");
                  return QSV_INVALID_OPER;
            }
            break;
         }

         default:
            QS_ARG_ERROR (0, "cannot compare type '%s'.\n",
               qs_value_type (first_val));
            return QSV_INVALID_TYPE;
      }

      /* stop immediately as soon as something fails and return 'false'. */
      if (rvalue == 0)
         return QSV_FALSE;
   }

   /* all checks passed; return 'true'. */
   return QSV_TRUE;
}

QS_FUNC (qsf_return)
{
   switch (sub_func) {
      case 0: QS_RETURN ();   break;
      case 1: QS_BREAK ();    break;
      case 2: QS_CONTINUE (); break;
   }
   if (args >= 1)
      return QS_ARGV (0);
   else
      return QSV_FALSE;
}

QS_FUNC (qsf_let)
{
   qs_value_t *lval;

   /* get our variable. */
   qs_value_t *aval = QS_ARGV(0);
   if ((lval = qs_value_lvalue (exe, aval)) == NULL) {
      QS_ARG_ERROR (0, "value '%s' cannot be modified.\n", aval->val_s);
      return QSV_CANNOT_MODIFY;
   }

   switch (sub_func) {
      /* = */
      case 0: {
         qs_value_t *rval = QS_ARGV (1);
         switch (lval->value_id) {
            /* if setting a character pointer, string/int behavior is
             * different. */
            case QS_VALUE_CHAR: {
               char *lch, rch;
               if ((lch = qs_value_char_pointer (lval)) == NULL) {
                  QS_ARG_ERROR (0, "couldn't get char for modification.\n");
                  return QSV_INVALID_INDEX;
               }
               if (!qs_value_as_char (rval, &rch)) {
                  QS_ARG_ERROR (1, "cannot cast rvalue '%s' of type '%s' "
                     "as a char.\n", rval->val_s, qs_value_type (rval));
                  return QSV_INVALID_VALUE;
               }
               *lch = rch;
               qs_value_update_from_string (lval);

               /* produce a return value. */
               char buf[2];
               buf[0] = rch;
               buf[1] = '\0';
               lval = QS_RETS (buf);
               break;
            }

            /* otherwise, lvalue <= rvalue */
            default:
               qs_value_copy (exe, lval, rval);
               break;
         }
         break;
      }

      /* ++, -- */
      default:
         switch (lval->value_id) {
            /* if modifying a character pointer, string/int behavior is
             * different. */
            case QS_VALUE_CHAR: {
               char *lch;
               int rch;
               if ((lch = qs_value_char_pointer (lval)) == NULL) {
                  QS_ARG_ERROR (0, "couldn't get char for modification.\n");
                  return QSV_INVALID_INDEX;
               }
               rch = *lch + sub_func;
               if (rch < 1 || rch > 255) {
                  QS_ARG_ERROR (1, "resulting char '%d' is out of range. "
                     "must be >= 1, <= 255.\n", rch);
                  return QSV_INVALID_VALUE;
               }
               *lch = rch;
               qs_value_update_from_string (lval);

               /* produce a return value. */
               char buf[2];
               buf[0] = rch;
               buf[1] = '\0';
               lval = QS_RETS (buf);
               break;
            }

            case QS_VALUE_INT: {
               char buf[256];
               lval->val_i += sub_func;
               lval->val_f = lval->val_i;
               snprintf (buf, sizeof (buf), "%d", lval->val_i);
               qs_value_restring (lval, buf);
               break;
            }
            case QS_VALUE_FLOAT: {
               char buf[256];
               lval->val_f += (float) sub_func;
               lval->val_i = (int) lval->val_f;
               snprintf (buf, sizeof (buf), "%g", lval->val_f);
               qs_value_restring (lval, buf);
               break;
            }
            default:
               QS_ARG_ERROR (0, "reference is invalid type '%s'.\n",
                             qs_value_type (lval));
               return QSV_INVALID_TYPE;
         }
         break;
   }

   /* return our new lvalue. */
   return lval;
}

QS_FUNC (qsf_type)
{
   qs_value_t *val = QS_ARGV (0);
   return QS_RETS (qs_value_type (val));
}

QS_FUNC (qsf_scope)
{
   /* no variable? */
   qs_value_t *val = QS_ARGV (0);
   switch (val->link_id) {
      case QS_LINK_LITERAL:
         return QSV_SCOPE_LITERAL;
      case QS_LINK_VARIABLE: {
         qs_variable_t *var = val->link;
         switch (var->scope_id) {
            case QS_SCOPE_RLINK:  return QSV_SCOPE_RLINK;
            case QS_SCOPE_BLOCK:  return QSV_SCOPE_BLOCK;
            case QS_SCOPE_SCHEME: return QSV_SCOPE_SCHEME;
            default:              return QSV_SCOPE_UNKNOWN;
         }
      }
      case QS_LINK_PROPERTY:
         return QSV_SCOPE_PROPERTY;
      default:
         QS_ARG_ERROR (0, "unknown scope for value.\n");
         return QSV_INVALID_VALUE;
   }
}

QS_FUNC (qsf_index)
{
   qs_value_t *val = QS_ARGV (0);
   if (val->value_id != QS_VALUE_CHAR)
      return QSV_INVALID_TYPE;
   if (val->val_p == NULL)
      return QSV_NO_INDEX;
   return QS_RETI (*((int *) val->data));
}

QS_FUNC (qsf_while)
{
   /* push a new execution state for break() and continue(). */
   qs_value_t *r = QSV_UNDEFINED;
   qs_execute_t *e = qs_execute_push (QS_EXECUTE_LOOP, exe->rlink, exe, action,
      func->name, 0, NULL);

   /* execute argument 1 until we can't anymore. */
   while (qs_value_truth (e, qs_value_read (e, arg[0])) &&
          !(e->flags & QS_EXECUTE_BREAK))
      r = qs_value_read (e, arg[1]);
   qs_execute_pop (e);

   /* return our last return value (or QSV_UNDEFINED for none). */
   return r;
}

QS_FUNC (qsf_for)
{
   /* push a new execution state for break() and continue(). */
   qs_value_t *r = QSV_UNDEFINED;
   qs_execute_t *e = qs_execute_push (QS_EXECUTE_LOOP, exe->rlink, exe, action,
      func->name, 0, NULL);

   /* c-style for() loop wrapped around a single execution state (exited
    * by break(). */
   qs_value_read (e, arg[0]);
   while (qs_value_truth (e, qs_value_read (e, arg[1])) &&
          !(e->flags & QS_EXECUTE_BREAK)) {
      r = qs_value_read (e, arg[3]);
      if (e->flags & QS_EXECUTE_BREAK)
         break;
      qs_value_read (e, arg[2]);
   }
   qs_execute_pop (e);

   /* return our last return value (or QSV_UNDEFINED for none). */
   return r;
}

QS_FUNC (qsf_for_each)
{
   /* make sure our first argument is a list. */
   qs_value_t *val = QS_ARGV (0);
   if (val->value_id != QS_VALUE_LIST) {
      QS_ARG_ERROR (0, "argument must be type 'list' (is type '%s').\n",
         qs_value_type (val));
      return QSV_INVALID_TYPE;
   }
   qs_list_t *list = val->val_p;

   /* make sure our second argument can be modified. */
   qs_value_t *aval = QS_ARGV(1),
              *lval = qs_value_lvalue (exe, QS_ARGV (1));
   if (lval == NULL) {
      QS_ARG_ERROR (1, "value '%s' cannot be modified.\n", aval->val_s);
      return QSV_CANNOT_MODIFY;
   }

   /* if the list is zero-length, bail here (without an error). */
   int length = list->value_count;
   if (length <= 0)
      return QSV_UNDEFINED;

   /* push a new execution state for break() and continue(). */
   qs_execute_t *e = qs_execute_push (QS_EXECUTE_LOOP, exe->rlink, exe, action,
      func->name, 0, NULL);

   /* iterate through list. */
   qs_value_t *rval = QSV_UNDEFINED;
   int i;
   for (i = 0; !(e->flags & QS_EXECUTE_BREAK) && i < length; i++) {
      qs_value_copy (e, lval, list->values[i]);
      rval = qs_value_read (e, arg[2]);
   }

   /* pop our loop execution state and return the last processed value. */
   qs_execute_pop (e);
   return rval;
}

QS_FUNC (qsf_args)
{
   qs_execute_t *e;
   qs_list_t *list;

   /* make sure we came from an action, and there's a previous list
    * of arguments passed to this function call. */
   if ((e = qs_execute_get_call (exe)) == NULL ||
       (list = e->list) == NULL) {
      /* if we're calling args(), we can safely return zero to indicate
       * that we're not a function. */
      if (args == 0)
         return QSV_FALSE;
      /* otherwise, toss an error. */
      else {
         qs_func_error (exe, func->name, action->node,
                        "not a function call.\n");
         QS_RETURN ();
         return QSV_NOT_FUNC_CALL;
      }
   }

   /* if calling with zero arguments, return the size of the list. */
   if (args == 0)
      return QS_RETI (list->value_count);

   /* proper parameter count? */
   if (list->value_count < args) {
      qs_func_error (exe, exe->name_p, action->node,
         "at least %d args required, only %d given.\n",
         args, list->value_count);
      QS_RETURN ();
      return QSV_NOT_ENOUGH_ARGS;
   }

   /* resolve all variables in the PREVIOUS execution state and store
    * them in new local variables. */
   int i, count = 0;
   qs_value_t *aval, *lval;
   for (i = 0; i < args; i++) {
      aval = QS_ARGV(i);
      if ((lval = qs_value_lvalue (exe, aval)) == NULL) {
         QS_ARG_ERROR (i, "value '%s' cannot be modified.\n", aval->val_s);
         QS_RETURN ();
         return QSV_CANNOT_MODIFY;
      }
      qs_value_copy (e, lval, qs_value_read (e->parent, list->values[i]));
      count++;
   }

   /* return the number of parameters used. */
   return QS_RETI (count);
}

QS_FUNC (qsf_arg)
{
   qs_execute_t *e;
   qs_list_t *list;

   /* make sure we came from an action, and there's a previous list
    * of arguments passed to this function call. */
   if ((e = qs_execute_get_call (exe)) == NULL ||
       (list = e->list) == NULL) {
      qs_func_error (exe, func->name, action->node,
         "not a function call.\n");
      return QSV_NOT_FUNC_CALL;
   }

   /* what argument are we executing? */
   int i = QS_ARGI(0);
   if (i < 0) {
      qs_func_error (exe, exe->name_p, action->node,
         "illegal argument index %d.\n", i);
      return QSV_INVALID_INDEX;
   }
   else if (i >= list->value_count) {
      qs_func_error (exe, exe->name_p, action->node,
         "index %d is out of bounds (max: %d).\n", i,
            list->value_count - 1);
      return QSV_INVALID_INDEX;
   }

   /* execute our argument in our current scope. */
   return qs_value_read (e->parent, list->values[i]);
}

QS_FUNC (qsf_arg_list)
{
   qs_execute_t *e;
   qs_list_t *list;

   /* make sure we came from an action, and there's a previous list
    * of arguments passed to this function call. */
   if ((e = qs_execute_get_call (exe)) == NULL ||
       (list = e->list) == NULL) {
      qs_func_error (exe, func->name, action->node,
         "not a function call.\n");
      return QSV_NOT_FUNC_CALL;
   }

   /* create a value that contains this list.  make certain it's immutable. */
   qs_value_t *rval = qs_scheme_heap_value (exe->scheme);
   rval->flags &= ~QS_VALUE_MUTABLE;
   rval->value_id = QS_VALUE_LIST;
   qs_value_restring (rval, "<list>");
   rval->val_p = list;

   /* return our new value. */
   return rval;
}

QS_FUNC (qsf_variable)
{
   qs_value_t *val = QS_ARGV (0);
   if (val->link_id == QS_LINK_VARIABLE)
      return qs_variable_value (val->link);

   /* make sure the value is the right type. */
   switch (val->value_id) {
      case QS_VALUE_STRING: {
         qs_variable_t *var;
         if ((var = qs_value_variable (exe, val)) == NULL) {
            QS_ARG_ERROR (0, "invalid variable scope or name from '%s'.\n",
                          val->val_s);
            return QSV_NOT_VARIABLE;
         }
         return qs_variable_value (var);
      }
      default:
         QS_ARG_ERROR (0, "can't lookup variable from type '%s'.\n",
                       qs_value_type (val));
         return QSV_INVALID_TYPE;
   }
}

QS_FUNC (qsf_property)
{
   qs_value_t *val = QS_ARGV (0);
   if (val->link_id == QS_LINK_PROPERTY)
      return qs_property_value (val->link);

   /* make sure the value is the right type. */
   switch (val->value_id) {
      case QS_VALUE_STRING: {
         qs_property_t *p;
         if ((p = qs_value_property (exe, val)) == NULL) {
            QS_ARG_ERROR (0, "invalid property from '%s'.\n", val->val_s);
            return QSV_NOT_PROPERTY;
         }
         return qs_property_value (p);
      }
      default:
         QS_ARG_ERROR (0, "can't lookup property from type '%s'.\n",
                       qs_value_type (val));
         return QSV_INVALID_TYPE;
   }
}

QS_FUNC (qsf_truth)
{
   switch (sub_func) {
      case 0:
         if (args == 1)
            return qs_value_truth (exe, QS_ARGV (0))
               ? QSV_TRUE
               : QSV_FALSE;
         else if (args == 3) {
            return qs_value_truth (exe, QS_ARGV (0))
               ? QS_ARGV (1)
               : QS_ARGV (2);
         }
         else {
            QS_ARG_ERROR (0, "invalid arg count; must be 1 or 3.\n", args);
            return QSV_INVALID_ARGS;
         }
      case 1:
         return qs_value_truth (exe, QS_ARGV (0))
            ? QSV_FALSE
            : QSV_TRUE;
      default:
         return QSV_INVALID_SUB_FUNC;
   }
}

QS_FUNC (qsf_first)
{
   int i;
   qs_value_t *v;
   for (i = 0; i < args; i++)
      if ((v = QS_ARGV (i))->value_id != QS_VALUE_UNDEFINED)
         return v;
   return QSV_UNDEFINED;
}

QS_FUNC (qsf_inv)
{
   qs_value_t *val = QS_ARGV (0);
   if (val->value_id != QS_VALUE_INT) {
      QS_ARG_ERROR (0, "invalid operation.\n");
      return QSV_INVALID_OPER;
   }
   return QS_RETI (~(val->val_i));
}

QS_FUNC (qsf_vars)
{
   /* get whatever code block we're in. */
   qs_execute_t *e = qs_execute_get_block (exe);

   /* create new variables in that code block. */
   int i;
   for (i = 0; i < args; i++)
      qs_variable_new_execute (e, QS_ARGS (i));

   /* return the number of new variables. */
   return QS_RETI (args);
}

QS_FUNC (qsf_run)
{
   /* TODO: this function shouldn't be allocating and defining these
    * structures itself!! */

   /* get the function name from argument zero. */
   qs_value_t *func_v = QS_ARGV (0);

   /* create a temporary list referencing parameters 1+ (not 0). */
   qs_list_t *l = malloc (sizeof (qs_list_t));
   l->scheme = exe->scheme;
   l->value_count = args - 1;
   l->values = arg + 1;

   /* create a temporary action representing this function call. */
   qs_action_t *a = calloc (1, sizeof (qs_action_t));
   a->action_id = QS_ACTION_CALL;
   a->data_p = l;
   a->parent_value = action->parent_value;
   a->node = action->node;

   /* run our action. */
   qs_value_t *rval = qs_action_call (exe, func_v, a);

   /* clean up memory we used and return whatever our call returned. */
   free (a);
   free (l);
   return rval;
}

QS_FUNC (qsf_boolean)
{
   int val1, val2, i, res = 0;

   val1 = qs_value_truth (exe, QS_ARGV (0));
   for (i = 1; i < args; i++, val1 = res) {
      if (!val1 && sub_func == 1)
         return QSV_FALSE;
      else if (val1 && sub_func == 0)
         return QSV_TRUE;
      val2 = qs_value_truth (exe, QS_ARGV (i));
      switch (sub_func) {
         case 0: res =  (val1 || val2); break;
         case 1: res =  (val1 && val2); break;
         case 2: res =  (val1 ^  val2); break;
         case 3: res = !(val1 || val2); break;
         case 4: res = !(val1 && val2); break;
         case 5: res = !(val1 ^  val2); break;
      }
   }
   return res ? QSV_TRUE : QSV_FALSE;
}

QS_FUNC (qsf_print_r)
{
   char *s;
   qs_resource_t *r;

   /* if no arguments are provided, use current resource. */
   if (args == 0)
      r = rlink->resource;
   /* otherwise, look it up. */
   else {
      s = QS_ARGS (0);
      if ((r = qs_resource_get (rlink->scheme, s)) == NULL) {
         QS_ARG_ERROR (0, "couldn't find resource '%s'.\n", s);
         return QSV_INVALID_RESOURCE;
      }
   }

   /* print and return 1 for success. */
   qs_print_resource (r);
   return QSV_TRUE;
}

QS_FUNC (qsf_print_v)
{
   qs_print_value (QS_ARGV(0), 0);
   printf ("\n");
   return QSV_TRUE;
}

QS_FUNC (qsf_eval)
{
   qs_value_t *r = QSV_UNDEFINED;
   int i;
   for (i = 0; i < args; i++)
      r = QS_ARGV (i);
   return r;
}

QS_FUNC (qsf_switch)
{
   qs_value_t *val1, *val2;
   int i, truth;

   /* get the value to be compared against. */
   val1 = QS_ARGV (0);
   switch (val1->value_id) {
      case QS_VALUE_STRING:
      case QS_VALUE_INT:
      case QS_VALUE_FLOAT:
         break;

      default:
         QS_ARG_ERROR (0, "cannot compare type '%s'.\n", qs_value_type (val1));
         return QSV_INVALID_TYPE;
   }

   /* check our provided values to see if it matches. */
   for (i = 1; i < args - 1; i += 2) {
      /* compare based on the given type. */
      val2 = QS_ARGV (i);
      switch (val1->value_id) {
         case QS_VALUE_STRING:
            truth = (strcmp (val1->val_s, val2->val_s) == 0);
            break;
         case QS_VALUE_INT:
            truth = (val1->val_i == val2->val_i);
            break;
         case QS_VALUE_FLOAT:
            truth = (val1->val_i == val2->val_i);
            break;

         /* never actually happens, but the compiler complains. */
         default:
            break;
      }

      /* if we had a truthful comparison, execute our action. */
      if (truth)
         return QS_ARGV (i + 1);
   }

   /* no actions executed.  run a default action if we have one. */
   if (i < args)
      return QS_ARGV (i);
   else
      return QSV_FALSE;
}

QS_FUNC (qsf_length)
{
   /* get our value. */
   qs_value_t *v = QS_ARGV (0);
   int length = qs_value_length (v);

   /* complain for bad types. */
   if (length < 0) {
      QS_ARG_ERROR (0, "cannot get length for value of type '%s'.\n",
                    qs_value_type (v));
      return QSV_INVALID_TYPE;
   }

   /* return our length. */
   return QS_RETI (length);
}

QS_FUNC (qsf_this)
{
   /* create an object type, set reference to ourselves, and return it. */
   qs_value_t *rval = qs_scheme_heap_value (exe->scheme);
   qs_value_init (rval, QS_VALUE_OBJECT, object->name, object->id);
   return rval;
}

QS_FUNC (qsf_cast)
{
   /* get value to cast.  if the type is already what we want,
    * don't bother casting. */
   qs_value_t *val = QS_ARGV (0);
   if (val->value_id == sub_func)
      return val;

   /* looks like we have to pull some shenanegins. */
   switch (sub_func) {
      /* standard primitives. */
      case QS_VALUE_BOOLEAN: return QS_RETB (qs_value_truth (exe, val));
      case QS_VALUE_INT:     return QS_RETI (val->val_i);
      case QS_VALUE_FLOAT:   return QS_RETF (val->val_f);
      case QS_VALUE_STRING:  return QS_RETS (val->val_s);

      /* complex primitives.  start with 'char'. */
      case QS_VALUE_CHAR: {
         /* create a char value. */
         qs_value_t *rval = qs_scheme_heap_value (exe->scheme);

         /* what character should it be? */
         int ch;
         switch (val->value_id) {
            case QS_VALUE_INT:
            case QS_VALUE_FLOAT:
               ch = val->val_i;
               break;
            case QS_VALUE_STRING:
               ch = val->val_s[0];
               break;
            default:
               ch = '?';
               break;
         }
         if (ch < 1 || ch > 255)
            ch = '?';

         /* set value and return our new variable. */
         qs_value_init (rval, QS_VALUE_CHAR, ch, NULL, NULL);
         return rval;
      }

      /* objects. */
      case QS_VALUE_OBJECT: {
         /* create an object value. */
         qs_value_t *rval = qs_scheme_heap_value (exe->scheme);
         rval->value_id = QS_VALUE_OBJECT;

         /* default values. */
         char *name = "<no object>";
         int id = 0l;

         /* should we return our own object? */
         qs_object_t *obj = NULL;
         if (val == NULL)
            obj = object;
         /* nope - attempt to get an object by type. */
         else {
            switch (val->value_id) {
               /* strings do a simple name lookup. */
               case QS_VALUE_STRING:
                  name = val->val_s;
                  obj = qs_object_get (exe->scheme, name);
                  break;

               /* integers perform an id lookup. */
               case QS_VALUE_INT:
                  id = val->val_i;
                  obj = qs_object_get_by_id (exe->scheme, val->val_i);
                  break;

               /* otherwise, this is bogus. */
               default:
                  obj = NULL;
                  break;
            }
         }

         /* if we found an object, use its values. */
         if (obj) {
            name = obj->name;
            id = obj->id;
         }

         /* set properties and return our object. */
         qs_value_restring (rval, "<object>");
         rval->val_i = id;
         rval->data = strdup (name);
         return rval;
      }

      /* dunno what to cast! */
      default:
         qs_func_error (exe, func->name, action->node,
            "unknown function '%s'.\n", func->name);
         return QSV_INVALID_SUB_FUNC;
   }
}

QS_FUNC (qsf_id)
{
   qs_object_t *obj;

   /* if no argument provided, use ourselves. */
   if (args == 0)
      obj = object;
   /* otherwise, make sure are argument is type 'object'. */
   else {
      qs_value_t *val = QS_ARGV (0);
      if (val->value_id != QS_VALUE_OBJECT) {
         QS_ARG_ERROR (0, "value is type '%s' instead of 'object'.\n",
            qs_value_type (val));
         return QSV_INVALID_TYPE;
      }
      obj = qs_value_object (exe, val);
   }

   /* get our object.  if we couldn't find it, return zero to indicate
    * 'no object'. */
   if (obj == NULL)
      return QSV_NO_OBJECT;

   /* return our object's name/id. */
   switch (sub_func) {
      case 0:
         return QS_RETI (obj->id);
      case 1:
         return QS_RETS (obj->name);
      case 2: {
         char buf[65536];
         qs_print_object_json (obj, buf, sizeof (buf));
         printf ("%s", buf);
         return QSV_TRUE;
      }
      default:
         return QSV_INVALID_SUB_FUNC;
   }
}

QS_FUNC (qsf_func_exists)
{
   qs_value_t *val = QS_ARGV(0);
   if (qs_resource_get (exe->scheme, val->val_s))
      return QSV_TRUE;
   else if (qs_scheme_get_func (exe->scheme, val->val_s))
      return QSV_TRUE;
   else
      return QSV_FALSE;
}

QS_FUNC (qsf_tokenize)
{
   /* get the string we're going to tokenize. */
   char *str = QS_ARGS(0);

   /* get an argument count for our new list. */
   int count, ws, i;
   for (count = 0, ws = 1, i = 0; str[i] != '\0'; i++) {
      if (str[i] == ' ' || str[i] == '\n')
         ws++;
      else if (ws) {
         count++;
         ws = 0;
      }
   }

   /* allocate a new list. */
   qs_value_t *rval = qs_scheme_heap_value (exe->scheme);
   qs_value_init (rval, QS_VALUE_LIST, count);
   qs_list_t *list = rval->val_p;

   /* copy token values. */
   int start = 0;
   for (count = -1, ws = 1, i = 0; 1; i++) {
      if (str[i] == ' ' || str[i] == '\n' || str[i] == '\0') {
         if (ws == 0) {
            /* add a new value to the list. */
            qs_value_t *lval = calloc (1, sizeof (qs_value_t));
            list->values[count] = lval;
            list->values_data[count] = lval;
            lval->scheme = exe->scheme;
            lval->value_id = QS_VALUE_STRING;
            lval->list = list;
            lval->flags |= QS_VALUE_MUTABLE;

            /* allocate string value. */
            lval->val_s = malloc (sizeof (char) * (i - start + 1));
            memcpy (lval->val_s, str + start, i - start);
            lval->val_s[i - start] = '\0';
            qs_value_update_from_string (lval);
         }
         if (str[i] == '\0')
            break;
         ws++;
      }
      else if (ws) {
         start = i;
         count++;
         ws = 0;
      }
   }

   /* return the new value. */
   return rval;
}


QS_FUNC (qsf_multi)
{
   /* no value if no lines. */
   if (args == 0)
      return QSV_UNDEFINED;

   /* keep track of string values. */
   char **strings = malloc (sizeof (char *) * args);
   int total_len = 0, i;
   for (i = 0; i < args; i++) {
      strings[i] = QS_ARGS(i);
      total_len += strlen (strings[i]) + 1;
   }

   /* create our return value. */
   qs_value_t *rval = qs_scheme_heap_value (exe->scheme);
   rval->value_id = QS_VALUE_STRING;

   /* build our multiline string in place of the original. */
   if (rval->val_s)
      free (rval->val_s);
   rval->val_s = malloc (sizeof (char) * total_len);
   int pos = 0, len;
   for (i = 0; i < args; i++) {
      len = strlen (strings[i]);
      memcpy (rval->val_s + pos, strings[i], len);
      pos += len;
      rval->val_s[pos++] = (i + 1 == args) ? '\0' : '\n';
   }

   /* clean up and return our new multi-line string. */
   free (strings);
   return rval;
}

QS_FUNC (qsf_new)
{
   /* make sure the base resource exists. */
   char *obj_name = QS_ARGS (0);

   /* instantiate the object. */
   qs_object_t *new = qs_object_new (exe->scheme, obj_name);
   if (new == NULL) {
      QS_ARG_ERROR (0, "unable to instantiate object '%s'.\n", obj_name);
      return QSV_NO_OBJECT;
   }

   /* variables required for the next loop. */
   qs_list_t *list;
   qs_resource_t *rsrc;
   char *rsrc_name;
   int i, priority = 0;

   /* apply resources at priorities. */
   for (i = 1; i < args; i++) {
      /* argument must be a list. */
      qs_value_t *v = QS_ARGV (i);
      if (v->value_id != QS_VALUE_LIST) {
         QS_ARG_ERROR (i, "argument '%s' is incorrect type.  must be a list "
            "with a resource name and (optional) priority.\n", v->val_s);
         continue;
      }

      /* list must have at least one element. */
      list = v->val_p;
      if (list->value_count == 0) {
         QS_ARG_ERROR (i, "argument cannot be empty list.\n");
         continue;
      }

      /* first list element must be type 'string'. */
      v = qs_value_read (exe, list->values[0]);
      if (v->value_id != QS_VALUE_STRING) {
         QS_ARG_ERROR (i, "first list element '%s' for resource name must be "
                          "type 'string'.\n", v->val_s);
         continue;
      }
      rsrc_name = v->val_s;

      /* second list element must be type 'int'. */
      if (list->value_count >= 2) {
         v = qs_value_read (exe, list->values[1]);
         if (v->value_id != QS_VALUE_INT) {
            QS_ARG_ERROR (i, "second list element '%s' for priority must be "
                             "type 'int'.\n", v->val_s);
            continue;
         }
         priority = v->val_i;
      }

      /* report an error if it didn't work - but don't abort. */
      if ((rsrc = qs_resource_get (exe->scheme, rsrc_name)) == NULL) {
         QS_ARG_ERROR (i, "cannot find resource '%s' to make rlink on "
            "object '%s' at priority %d.\n", rsrc_name, new->name, priority);
         continue;
      }

      /* looks like it worked!  push rlink. */
      qs_rlink_inject (new, rsrc, priority);
   }

   /* TODO: this should be handled by the scheme in a different way. */
   /* wind all unwound rlinks. */
   qs_rlink_t *r;
   for (r = new->rlink_list_front; r != NULL; r = r->next)
      if (!(r->flags & QS_RLINK_ON))
         qs_rlink_wind (r);

   /* return the new object's id. */
   return QS_RETI (new->id);
}

QS_FUNC (qsf_error)
{
   char *val = QS_ARGS (0), buf[256];
   snprintf (buf, sizeof (buf), "<%s>", val);
   return QS_RETU (buf);
}
