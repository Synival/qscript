/* standard.c
 * ----------
 * standard functions included in all language schemes. */

#include <math.h>
#include <stdio.h>

#include "actions.h"
#include "execute.h"
#include "funcs.h"
#include "language.h"
#include "lists.h"
#include "print.h"
#include "resources.h"
#include "rlinks.h"
#include "schemes.h"
#include "stacks.h"
#include "values.h"
#include "variables.h"

#include "standard.h"

qs_func_t qs_func_list_standard[] = {
   { "print",   "<print1> [... <printn>]", 0, qsf_print,   0 },
   { "echo",    "<print1> [... <printn>]", 0, qsf_print,   1 },
   { "+",       "<val1> [... <valn>]",     2, qsf_math,    0 },
   { "-",       "<val1> [... <valn>]",     2, qsf_math,    1 },
   { "*",       "<val1> [... <valn>]",     2, qsf_math,    2 },
   { "/",       "<val1> [... <valn>]",     2, qsf_math,    3 },
   { "**",      "<val1> [... <valn>]",     2, qsf_math,    4 },
   { "|",       "<val1> [... <valn>]",     2, qsf_math,    5 },
   { "&",       "<val1> [... <valn>]",     2, qsf_math,    6 },
   { "^",       "<val1> [... <valn>]",     2, qsf_math,    7 },
   { "+=","<variable> <val1> [... <valn>]",2, qsf_math,    20 },
   { "-=","<variable> <val1> [... <valn>]",2, qsf_math,    21 },
   { "*=","<variable> <val1> [... <valn>]",2, qsf_math,    22 },
   { "/=","<variable> <val1> [... <valn>]",2, qsf_math,    23 },
   {"**=","<variable> <val1> [... <valn>]",2, qsf_math,    24 },
   { "|=","<variable> <val1> [... <valn>]",2, qsf_math,    25 },
   { "&=","<variable> <val1> [... <valn>]",2, qsf_math,    26 },
   { "^=","<variable> <val1> [... <valn>]",2, qsf_math,    27 },
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
   { "!",       "<value>",                 1, qsf_not,     0 },
   { "inv",     "<value>",                 1, qsf_inv,     0 },
   { "return",  "[<value>]",               0, qsf_return,  0 },
   { "break",   "[<value>]",               0, qsf_return,  1 },
   { "=",       "<variable> <value>",      2, qsf_let,     0 },
   { "++",      "<variable>",              1, qsf_let,     1 },
   { "--",      "<variable>",              1, qsf_let,    -1 },
   { "type",    "<value>",                 1, qsf_type,    0 },
   { "scope",   "<value>",                 1, qsf_scope,   0 },
   { "while",   "<condition> <action>",    2, qsf_while,   0 },
   { "for",     "<init> <cond> <after> <action>",
                                           4, qsf_for,     0 },
   { "switch",  "<cond> [<value1> <action1> ... <valuen> <actionn>] "
                "[<default action>]",      1, qsf_switch,  0 },
   { "args",    "[<ref1> ... <ref2>]",     0, qsf_args,    0 },
   { "arg",     "<index>",                 1, qsf_arg,     0 },
   { "variable","<string>",                1, qsf_variable,0 },
   { "vars",    "<var1> [... <varn>]",     1, qsf_vars,    0 },
   { "run",     "<func> [<arg1> ... <argn>]",1,qsf_run,    0 },
   { "print_resource", "[<resource>]",     0, qsf_print_r, 0 },
   { "print_value",    "[<value>",         1, qsf_print_v, 0 },
   { "eval",    "<arg1> [... <argn>]",     1, qsf_eval,    0 },
   { "length",  "<string | list>",         1, qsf_length,  0 },
   { "string",  "<value>",                 1, qsf_cast,    QSCRIPT_STRING },
   { "int",     "<value>",                 1, qsf_cast,    QSCRIPT_INT },
   { "float",   "<value>",                 1, qsf_cast,    QSCRIPT_FLOAT },
   { "char",    "<value>",                 1, qsf_cast,    QSCRIPT_CHAR },
   { NULL },
};

QS_FUNC (qsf_print)
{
   char buf[65536];
   size_t len = 0;
   int i;

   /* build our print string and print it out. */
   buf[0] = '\0';
   for (i = 0; i < args; i++)
      len += snprintf (buf + len, sizeof (buf) - len, "%s", QS_ARGS (i));

   /* 'print': no newline
    * 'echo':  newline */
   switch (sub_func) {
      case 0: printf ("%s",   buf); break;
      case 1: printf ("%s\n", buf); break;
   }

   /* return whatever string we just printed. */
   return QS_RETS (buf);
}

QS_FUNC (qsf_math)
{
   qs_value_t *val, *lval = NULL, *rval;
   int i, start, math_func;

   /* are we setting a value...? */
   if (sub_func >= 20) {
      /* get our lvalue.  can we modify it? */
      lval = QS_ARGV (0);
      if (!qs_value_can_modify (lval)) {
         QS_ARG_ERROR (0, "first argument cannot be modified.\n");
         return QSV_CANNOT_MODIFY;
      }
      if (lval->type_id == QSCRIPT_UNDEFINED) {
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
   switch (val->type_id) {
      case QSCRIPT_CHAR: {
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
                  if (rch == 0) {
                     QS_ARG_ERROR (i, "division by zero.\n");
                     return QSV_DIVISION_BY_ZERO;
                  }
                  v /= (int) rch;
                  break;
               }
               case 4: v  = (int) powf (v, rch); break;
               case 5: v |= (int) rch; break;
               case 6: v &= (int) rch; break;
               case 7: v ^= (int) rch; break;
               default:
                  QS_ARG_ERROR (i, "invalid operation.\n");
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

      case QSCRIPT_INT: {
         int v = val->val_i, r;
         for (i = start; i < args; i++) {
            r = QS_ARGI (i);
            switch (math_func) {
               case 0: v += r; break;
               case 1: v -= r; break;
               case 2: v *= r; break;
               case 3: {
                  if (r == 0) {
                     QS_ARG_ERROR (i, "division by zero.\n");
                     return QSV_DIVISION_BY_ZERO;
                  }
                  v /= r;
                  break;
               }
               case 4: v = (int) powf (v, r); break;
               case 5: v |= r; break;
               case 6: v &= r; break;
               case 7: v ^= r; break;
               default:
                  QS_ARG_ERROR (i, "invalid operation.\n");
                  return QSV_INVALID_OPER;
            }
         }
         rval = QS_RETI (v);
         break;
      }

      case QSCRIPT_FLOAT: {
         float v = val->val_f, r;
         for (i = start; i < args; i++) {
            r = QS_ARGF (i);
            switch (math_func) {
               case 0: v += r; break;
               case 1: v -= r; break;
               case 2: v *= r; break;
               case 3:
                  if (r == 0.00f) {
                     QS_ARG_ERROR (i, "division by zero.\n");
                     return QSV_DIVISION_BY_ZERO;
                  }
                  v /= r;
                  break;
               case 4: v = powf (v, r); break;
               default:
                  QS_ARG_ERROR (i, "invalid operation.\n");
                  return QSV_INVALID_OPER;
            }
         }
         rval = QS_RETF (v);
         break;
      }

      case QSCRIPT_STRING:
         if (math_func == 0) {
            char buf[65536];
            size_t len = 0;
            buf[0] = '\0';
            for (i = start - 1; i < args; i++)
               len += snprintf (buf + len, sizeof (buf) - len, "%s",
                                QS_ARGS (i));
            rval = QS_RETS (buf);
         }
         else {
            QS_ARG_ERROR (0, "invalid operation.\n");
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
      if (val->type_id == QSCRIPT_CHAR && val->val_p) {
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
         qs_value_copy (lval, rval);
   }

   /* herp derp */
   return rval;
}

QS_FUNC (qsf_if)
{
   qs_value_t *val = QS_ARGV (0);
   if (qs_value_truth (val))
      return QS_ARGV (1);
   else if (args >= 3)
      return QS_ARGV (2);
   else
      return QSV_ONE;
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
      switch (first_val->type_id) {
         case QSCRIPT_CHAR: {
            char *lch, rch;
            if ((lch = qs_value_char_pointer (val1)) == NULL)
               return QSV_ZERO;
            if (!qs_value_as_char (val2, &rch))
               return QSV_ZERO;
            switch (sub_func) {
               case 0: rvalue = (*lch == rch) ? 1 : 0; break;
               case 1: rvalue = (*lch != rch) ? 1 : 0; break;
               case 2: rvalue = (*lch >  rch) ? 1 : 0; break;
               case 3: rvalue = (*lch >= rch) ? 1 : 0; break;
               case 4: rvalue = (*lch <  rch) ? 1 : 0; break;
               case 5: rvalue = (*lch <= rch) ? 1 : 0; break;
               default: return QSV_INVALID_OPER;
            }
            break;
         }

         case QSCRIPT_STRING:
            rvalue = strcmp (val1->val_s, val2->val_s);
            switch (sub_func) {
               case 0: rvalue = (rvalue == 0 ? 1 : 0); break;
               case 1: rvalue = (rvalue != 0 ? 1 : 0); break;
               case 2: rvalue = (rvalue >  0 ? 1 : 0); break;
               case 3: rvalue = (rvalue >= 0 ? 1 : 0); break;
               case 4: rvalue = (rvalue <  0 ? 1 : 0); break;
               case 5: rvalue = (rvalue <= 0 ? 1 : 0); break;
               default: return QSV_INVALID_OPER;
            }
            break;
         case QSCRIPT_INT:
            switch (sub_func) {
               case 0: rvalue = (val1->val_i == val2->val_i) ? 1 : 0; break;
               case 1: rvalue = (val1->val_i != val2->val_i) ? 1 : 0; break;
               case 2: rvalue = (val1->val_i >  val2->val_i) ? 1 : 0; break;
               case 3: rvalue = (val1->val_i >= val2->val_i) ? 1 : 0; break;
               case 4: rvalue = (val1->val_i <  val2->val_i) ? 1 : 0; break;
               case 5: rvalue = (val1->val_i <= val2->val_i) ? 1 : 0; break;
               default: return QSV_INVALID_OPER;
            }
            break;
         case QSCRIPT_FLOAT:
            switch (sub_func) {
               case 0: rvalue = (val1->val_f == val2->val_f) ? 1 : 0; break;
               case 1: rvalue = (val1->val_f != val2->val_f) ? 1 : 0; break;
               case 2: rvalue = (val1->val_f >  val2->val_f) ? 1 : 0; break;
               case 3: rvalue = (val1->val_f >= val2->val_f) ? 1 : 0; break;
               case 4: rvalue = (val1->val_f <  val2->val_f) ? 1 : 0; break;
               case 5: rvalue = (val1->val_f <= val2->val_f) ? 1 : 0; break;
               default: return QSV_INVALID_OPER;
            }
            break;

         default:
            QS_ARG_ERROR (0, "cannot compare type '%s'.\n",
               qs_value_type (first_val));
            return QSV_INVALID_TYPE;
      }

      /* stop immediately as soon as something fails and return 0 (false). */
      if (rvalue == 0)
         return QSV_ZERO;
   }

   /* all checks passed; return 1 (true). */
   return QSV_ONE;
}

QS_FUNC (qsf_return)
{
   switch (sub_func) {
      case 0: QS_RETURN (); break;
      case 1: QS_BREAK ();  break;
   }
   if (args >= 1)
      return QS_ARGV (0);
   else
      return QSV_ZERO;
}

QS_FUNC (qsf_let)
{
   qs_value_t *lval;

   /* get our variable. */
   lval = QS_ARGV (0);
   if (!qs_value_can_modify (lval)) {
      QS_ARG_ERROR (0, "first argument cannot be modified.\n");
      return QSV_CANNOT_MODIFY;
   }

   switch (sub_func) {
      /* = */
      case 0: {
         qs_value_t *rval = QS_ARGV (1);
         switch (lval->type_id) {
            /* if setting a character pointer, string/int behavior is
             * different. */
            case QSCRIPT_CHAR: {
               char *lch, rch;
               if ((lch = qs_value_char_pointer (lval)) == NULL) {
                  QS_ARG_ERROR (0, "couldn't get index for modification.\n");
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
               qs_value_copy (lval, rval);
         }
         break;
      }

      /* ++, -- */
      default:
         switch (lval->type_id) {
            /* if modifying a character pointer, string/int behavior is
             * different. */
            case QSCRIPT_CHAR: {
               char *lch;
               int rch;
               if ((lch = qs_value_char_pointer (lval)) == NULL) {
                  QS_ARG_ERROR (0, "couldn't get index for modification.\n");
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

            case QSCRIPT_INT: {
               char buf[256];
               lval->val_i += sub_func;
               lval->val_f = lval->val_i;
               free (lval->val_s);
               snprintf (buf, sizeof (buf), "%d", lval->val_i);
               lval->val_s = strdup (buf);
               break;
            }
            case QSCRIPT_FLOAT: {
               char buf[256];
               lval->val_f += (float) sub_func;
               lval->val_i = (int) lval->val_f;
               free (lval->val_s);
               snprintf (buf, sizeof (buf), "%g", lval->val_f);
               lval->val_s = strdup (buf);
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
   if (val->variable == NULL)
      return QSV_SCOPE_LITERAL;
   switch (val->variable->link_id) {
      case QS_SCOPE_OBJECT:  return QSV_SCOPE_OBJECT;
      case QS_SCOPE_BLOCK:   return QSV_SCOPE_BLOCK;
      default:               return QSV_SCOPE_UNKNOWN;
   }
}

QS_FUNC (qsf_while)
{
   qs_value_t *r = NULL;
   while (!(exe->flags & QS_EXE_BREAK) && qs_value_truth (QS_ARGV (0)))
      r = QS_ARGV (1);
   if (r == NULL)
      return QSV_ZERO;
   return r;
}

QS_FUNC (qsf_for)
{
   qs_value_t *r = NULL;
   QS_ARGV (0);
   while (!(exe->flags & QS_EXE_BREAK) && qs_value_truth (QS_ARGV (1))) {
      r = QS_ARGV (3);
      QS_ARGV (2);
   }
   if (r == NULL)
      return QSV_ZERO;
   return r;
}

QS_FUNC (qsf_args)
{
   qs_execute_t *e;
   qs_list_t *list;

   /* make sure we came from an action, and there's a previous list
    * of arguments passed to this function call. */
   if ((e = qs_execute_get_call (exe)) == NULL ||
       (list = e->list) == NULL) {
      qs_func_error (exe, func->name, action->value->node,
                     "not a function call.\n");
      QS_RETURN ();
      return QSV_NOT_FUNC_CALL;
   }

   /* if calling with zero arguments, return the size of the list. */
   if (args == 0)
      return QS_RETI (list->value_count);

   /* proper parameter count? */
   if (list->value_count < args) {
      qs_func_error (exe, exe->name, action->value->node,
         "at least %d args required, only %d given.\n",
         args, list->value_count);
      QS_RETURN ();
      return QSV_NOT_ENOUGH_ARGS;
   }

   /* resolve all variables in the PREVIOUS execution state and store
    * them in new local variables. */
   int i, count = 0;
   qs_value_t *val;
   for (i = 0; i < args; i++) {
      val = QS_ARGV (i);
      if (!qs_value_can_modify (val)) {
         QS_ARG_ERROR (i, "argument %d cannot be modified.\n", i + 1);
         QS_RETURN ();
         return QSV_CANNOT_MODIFY;
      }
      qs_value_copy (val, qs_arg_value (e->parent, action, list->values[i]));
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
      qs_func_error (exe, func->name, action->value->node,
         "not a function call.\n");
      return QSV_NOT_FUNC_CALL;
   }

   /* what argument are we executing? */
   int i = QS_ARGI(0);
   if (i < 0) {
      qs_func_error (exe, exe->name, action->value->node,
         "illegal argument index %d.\n", i);
      return QSV_INVALID_INDEX;
   }
   else if (i >= list->value_count) {
      qs_func_error (exe, exe->name, action->value->node,
         "index %d is out of bounds (max: %d).\n", i,
            list->value_count - 1);
      return QSV_INVALID_INDEX;
   }

   /* execute our argument in our current scope. */
   return qs_arg_value (e->parent, action, list->values[i]);
}

QS_FUNC (qsf_variable)
{
   qs_value_t *val = QS_ARGV (0);
   if (val->variable)
      return qs_variable_value (val->variable);

   /* make sure the value is the right type. */
   switch (val->type_id) {
      case QSCRIPT_STRING: {
         qs_variable_t *var;
         if ((var = qs_value_get_variable (rlink, val)) == NULL) {
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

QS_FUNC (qsf_not)
{
   return qs_value_truth (QS_ARGV (0)) ? QSV_ZERO : QSV_ONE;
}

QS_FUNC (qsf_inv)
{
   qs_value_t *val = QS_ARGV (0);
   if (val->type_id != QSCRIPT_INT) {
      QS_ARG_ERROR (0, "invalid operation.\n");
      return QSV_INVALID_OPER;
   }
   return QS_RETI (~(val->val_i));
}

QS_FUNC (qsf_vars)
{
   int i;
   for (i = 0; i < args; i++)
      qs_variable_new_rlink (rlink, QS_ARGS (i));
   return QS_RETI (args);
}

QS_FUNC (qsf_run)
{
   /* TODO: this function shouldn't be allocating and defining these
    * structures itself!! */

   /* create a temporary list referencing parameters 1+ (not 0). */
   qs_list_t *l = malloc (sizeof (qs_list_t));
   l->value_count = args - 1;
   l->values = arg + 1;

   /* create a temporary action representing this function call. */
   qs_action_t *a = malloc (sizeof (qs_action_t));
   memset (a, 0, sizeof (qs_action_t));
   a->type_id = QS_ACTION_CALL;
   a->data_p = l;
   a->value = action->value;

   /* run our action. */
   qs_value_t *rval = qs_action_call (exe, QS_ARGV (0), a);

   /* clean up memory we used and return whatever our call returned. */
   free (a);
   free (l);
   return rval;
}

QS_FUNC (qsf_boolean)
{
   int val1, val2, i, res = 0;

   val1 = qs_value_truth (QS_ARGV (0));
   for (i = 1; i < args; i++, val1 = res) {
      if (!val1 && sub_func == 1)
         return QS_RETI (0);
      else if (val1 && sub_func == 0)
         return QS_RETI (1);
      val2 = qs_value_truth (QS_ARGV (i));
      switch (sub_func) {
         case 0: res =  (val1 || val2); break;
         case 1: res =  (val1 && val2); break;
         case 2: res =  (val1 ^  val2); break;
         case 3: res = !(val1 || val2); break;
         case 4: res = !(val1 && val2); break;
         case 5: res = !(val1 ^  val2); break;
      }
   }

   return QS_RETI (res);
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
   return QSV_ONE;
}

QS_FUNC (qsf_print_v)
{
   qs_print_value (QS_ARGV(0), 0);
   printf ("\n");
   return QSV_ONE;
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
   switch (val1->type_id) {
      case QSCRIPT_STRING:
      case QSCRIPT_INT:
      case QSCRIPT_FLOAT:
         break;

      default:
         QS_ARG_ERROR (0, "cannot compare type '%s'.\n", qs_value_type (val1));
         return QSV_INVALID_TYPE;
   }

   /* check our provided values to see if it matches. */
   for (i = 1; i < args - 1; i += 2) {
      /* compare based on the given type. */
      val2 = QS_ARGV (i);
      switch (val1->type_id) {
         case QSCRIPT_STRING:
            truth = (strcmp (val1->val_s, val2->val_s) == 0);
            break;
         case QSCRIPT_INT:
            truth = (val1->val_i == val2->val_i);
            break;
         case QSCRIPT_FLOAT:
            truth = (val1->val_i == val2->val_i);
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
      return QSV_ZERO;
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

QS_FUNC (qsf_cast)
{
   qs_value_t *val = QS_ARGV (0);

   /* if the type is already what we want, don't bother casting. */
   if (val->type_id == sub_func)
      return val;

   /* looks like we have to pull some shenanegins. */
   switch (sub_func) {
      /* standard primitives. */
      case QSCRIPT_STRING: return QS_RETS (val->val_s);
      case QSCRIPT_INT:    return QS_RETI (val->val_i);
      case QSCRIPT_FLOAT:  return QS_RETF (val->val_f);

      /* complex primitives. */
      case QSCRIPT_CHAR: {
         /* we'll need to do most of this ourselves.
          * add a value to the heap. */
         qs_value_t *rval = qs_scheme_heap_value (exe->scheme);
         rval->type_id = QSCRIPT_CHAR;

         /* what character should it be? */
         char buf[2];
         switch (val->type_id) {
            case QSCRIPT_INT:
            case QSCRIPT_FLOAT:
               buf[0] = (val->val_i >= 1 && val->val_i <= 255) ?
                         val->val_i : '?';
               break;
            case QSCRIPT_STRING:
               buf[0] = (val->val_s[0] != 0) ? val->val_s[0] : '?';
               break;
            default:
               buf[0] = '?';
               break;
         }
         buf[1] = '\0';

         /* set values and return our new variable. */
         qs_value_restring (rval, buf);
         rval->val_i = buf[0];
         rval->val_f = buf[0];
         return rval;
      }

      /* dunno! */
      default:
         return QSV_INVALID_SUB_FUNC;
   }
}
