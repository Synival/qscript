/* funcs.c
 * -------
 * useful functions to be used in qs_func()s. */

#include <stdio.h>

#include "actions.h"
#include "execute.h"
#include "language.h"
#include "lists.h"
#include "rlinks.h"
#include "schemes.h"
#include "values.h"
#include "variables.h"

#include "funcs.h"

#define qs_arg_start(x) \
   qs_value_t *v; \
   if ((v = qs_value_read (exe, val)) == NULL) \
      return x;

qs_value_t *qs_arg_value (qs_execute_t *exe, qs_action_t *a, qs_value_t *val)
   { return qs_value_read (exe, val); }
char *qs_arg_string (qs_execute_t *exe, qs_action_t *a, qs_value_t *val)
   { qs_arg_start ("<error>"); return v->val_s; }
float qs_arg_float  (qs_execute_t *exe, qs_action_t *a, qs_value_t *val)
   { qs_arg_start (0); return v->val_f; }
int   qs_arg_int    (qs_execute_t *exe, qs_action_t *a, qs_value_t *val)
   { qs_arg_start (0); return v->val_i; }

int qs_func_error (qs_execute_t *exe, char *func_name, p_node_t *node,
                   char *format, ...)
{
   char buf[256];
   va_list va_args;

   /* build our error string. */
   va_start (va_args, format);
   vsnprintf (buf, sizeof (buf), format, va_args);
   va_end (va_args);

   /* use the parser's error function. */
   return p_error (node, "%s(): %s", func_name, buf);
}

qs_value_t *qs_return_value_new (qs_scheme_t *scheme)
{
   return qs_scheme_heap_value (scheme);
}

qs_value_t *qs_return_string (qs_scheme_t *scheme, char *s)
{
   qs_value_t *r = qs_return_value_new (scheme);
   r->type_id = QSCRIPT_STRING;
   qs_value_restring (r, s);
   qs_value_update_from_string (r);
   return r;
}

qs_value_t *qs_return_int (qs_scheme_t *scheme, int i)
{
   char buf[256];
   qs_value_t *r = qs_return_value_new (scheme);
   r->type_id = QSCRIPT_INT;
   snprintf (buf, sizeof (buf), "%d", i);
   qs_value_restring (r, buf);
   r->val_i = i;
   r->val_f = i;
   return r;
}

qs_value_t *qs_return_float (qs_scheme_t *scheme, float f)
{
   char buf[256];
   qs_value_t *r = qs_return_value_new (scheme);
   r->type_id = QSCRIPT_FLOAT;
   snprintf (buf, sizeof (buf), "%g", f);
   qs_value_restring (r, buf);
   r->val_i = f;
   r->val_f = f;
   return r;
}

qs_value_t *qs_return_char (qs_scheme_t *scheme, char c)
{
   char buf[2];
   qs_value_t *r = qs_return_value_new (scheme);
   r->type_id = QSCRIPT_CHAR;
   buf[0] = c;
   buf[1] = '\0';
   qs_value_restring (r, buf);
   r->val_i = c;
   r->val_f = c;
   return r;
}

qs_value_t *qs_func_run (qs_execute_t *exe, qs_func_t *func)
{
   /* are there enough arguments for this function? */
   if (exe->list->value_count < func->min_args) {
      qs_func_error (exe, func->name, (exe->action) ? exe->action->value->node
         : NULL, "at least %d args required, only %d given.\n",
         func->min_args, exe->list->value_count);
      return QSV_NOT_ENOUGH_ARGS;
   }
   else
      return func->func (exe->rlink->object, exe->rlink, exe, exe->action,
         func, func->sub_func, exe->list->value_count, exe->list->values);
}

int qs_func_break (qs_execute_t *e)
{
   int count = 0;
   for (; e != NULL; e = e->parent) {
      e->flags |= QS_EXE_BREAK;
      count++;
      if (e->type_id == QS_EXE_RESOURCE || e->type_id == QS_EXE_LAMBDA ||
          e->type_id == QS_EXE_BLOCK)
         break;
   }
   return count;
}

int qs_func_return (qs_execute_t *e)
{
   int count = 0;
   for (; e != NULL; e = e->parent) {
      e->flags |= QS_EXE_BREAK;
      count++;
      if (e->type_id == QS_EXE_RESOURCE || e->type_id == QS_EXE_LAMBDA)
         break;
   }
   return count;
}
