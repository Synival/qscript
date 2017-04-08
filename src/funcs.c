/* funcs.c
 * -------
 * useful functions to be used in qs_func()s. */

#include <stdio.h>
#include <stdarg.h>

#include "qscript/actions.h"
#include "qscript/execute.h"
#include "qscript/language.h"
#include "qscript/lists.h"
#include "qscript/parser.h"
#include "qscript/rlinks.h"
#include "qscript/schemes.h"
#include "qscript/stacks.h"
#include "qscript/values.h"
#include "qscript/variables.h"

#include "qscript/funcs.h"

#define qs_arg_start(x) \
   qs_value_t *v; \
   if ((v = qs_value_read (exe, val)) == NULL) \
      return x;

/* simple primitives. */
char *qs_arg_string (qs_execute_t *exe, qs_value_t *val)
   { qs_arg_start ("<error>"); return v->val_s; }
float qs_arg_float  (qs_execute_t *exe, qs_value_t *val)
   { qs_arg_start (0); return v->val_f; }
int   qs_arg_int    (qs_execute_t *exe, qs_value_t *val)
   { qs_arg_start (0); return v->val_i; }

/* complex primitives. */
qs_list_t *qs_arg_list (qs_execute_t *exe, qs_value_t *val) {
   qs_arg_start (NULL);
   return (v->value_id != QS_VALUE_LIST) ? NULL : v->val_p;
}
qs_object_t *qs_arg_object (qs_execute_t *exe, qs_value_t *val) {
   qs_arg_start (NULL);
   return (v->value_id != QS_VALUE_OBJECT) ? NULL : qs_value_object (exe, val);
}

/* return value producing functions. */
qs_value_t *qs_return_value (qs_scheme_t *scheme)
   { return qs_scheme_heap_value (scheme); }
qs_value_t *qs_return_string (qs_scheme_t *scheme, char *s) {
   qs_value_t *r = qs_return_value (scheme);
   qs_value_init (r, QS_VALUE_STRING, s);
   return r;
}
qs_value_t *qs_return_int (qs_scheme_t *scheme, int i) {
   qs_value_t *r = qs_return_value (scheme);
   qs_value_init (r, QS_VALUE_INT, i);
   return r;
}
qs_value_t *qs_return_float (qs_scheme_t *scheme, float f) {
   qs_value_t *r = qs_return_value (scheme);
   qs_value_init (r, QS_VALUE_FLOAT, f);
   return r;
}
qs_value_t *qs_return_char (qs_scheme_t *scheme, char c) {
   qs_value_t *r = qs_return_value (scheme);
   qs_value_init (r, QS_VALUE_CHAR, c, NULL, NULL);
   return r;
}
qs_value_t *qs_return_list (qs_scheme_t *scheme, int count) {
   qs_value_t *r = qs_return_value (scheme);
   qs_value_init (r, QS_VALUE_LIST, count);
   return r;
}
qs_value_t *qs_return_undefined (qs_scheme_t *scheme, char *s) {
   qs_value_t *r = qs_return_value (scheme);
   qs_value_init (r, QS_VALUE_UNDEFINED, s);
   return r;
}

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

qs_value_t *qs_func_run (qs_execute_t *exe, qs_func_t *func)
{
   /* are there enough arguments for this function? */
   if (exe->list->value_count < func->min_args) {
      qs_func_error (exe, func->name, (exe->action) ? exe->action->node
         : NULL, "at least %d args required, only %d given.\n",
         func->min_args, exe->list->value_count);
      return QSV_NOT_ENOUGH_ARGS;
   }
   else
      return func->func (exe->object, exe->rlink, exe, exe->action,
         func, func->sub_func, exe->list->value_count, exe->list->values);
}

int qs_func_break (qs_execute_t *e)
{
   int count = 0;
   for (; e != NULL; e = e->parent) {
      e->flags |= QS_EXECUTE_BREAK;
      count++;
      if (e->execute_id == QS_EXECUTE_RESOURCE ||
          e->execute_id == QS_EXECUTE_LAMBDA ||
          e->execute_id == QS_EXECUTE_LOOP)
         break;
   }
   return count;
}

int qs_func_return (qs_execute_t *e)
{
   int count = 0;
   for (; e != NULL; e = e->parent) {
      e->flags |= QS_EXECUTE_BREAK;
      count++;
      if (e->execute_id == QS_EXECUTE_RESOURCE ||
          e->execute_id == QS_EXECUTE_LAMBDA)
         break;
   }
   return count;
}

int qs_func_continue (qs_execute_t *e)
{
   int count = 0;
   for (; e != NULL; e = e->parent) {
      if (e->execute_id == QS_EXECUTE_LOOP)
         break;
      e->flags |= QS_EXECUTE_BREAK;
      count++;
      if (e->execute_id == QS_EXECUTE_RESOURCE ||
          e->execute_id == QS_EXECUTE_LAMBDA)
         break;
   }
   return count;
}
