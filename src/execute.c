/* execute.c
 * ---------
 * execution of rlinks on objects. */

#include <string.h>

#include "actions.h"
#include "language.h"
#include "link.h"
#include "lists.h"
#include "rlinks.h"
#include "resources.h"
#include "schemes.h"
#include "values.h"
#include "variables.h"

#include "execute.h"

qs_execute_t *qs_execute_push (int type, qs_rlink_t *rlink, qs_execute_t *exe,
   qs_action_t *action, char *name, int flags, qs_list_t *list)
{
   /* allocate and initialize our new execution state. */
   qs_execute_t *new = malloc (sizeof (qs_execute_t));
   memset (new, 0, sizeof (qs_execute_t));
   new->scheme  = rlink->scheme;
   new->type_id = type;
   new->rlink   = rlink;
   new->action  = action;
   new->flags   = flags;
   new->name    = strdup (name);
   new->list    = list;

   /* link to parent. */
   new->parent = exe;

   /* link to our scheme. */
   QS_LINK_BACK (new->scheme, new, exe);

   /* return our new execution state. */
   return new;
}

int qs_execute_pop (qs_execute_t *exe)
{
   /* free allocated data. */
   if (exe->name)
      free (exe->name);

   /* should we free our list? */
   if (exe->flags & QS_EXE_FREE_LIST)
      qs_list_free (exe->list);

   /* unlink. */
   QS_UNLINK (exe->scheme, exe, exe);

   /* free ourselves and return success. */
   free (exe);
   return 1;
}

qs_value_t *qs_execute_rlink (qs_execute_t *exe, qs_rlink_t *rlink)
{
   qs_execute_t *new_exe = NULL;
   if (exe == NULL)
      exe = new_exe = qs_execute_push (QS_EXE_RESOURCE, rlink, NULL, NULL,
         rlink->resource->name, 0, NULL);

   /* create a new value on the heap to return. */
   qs_value_t *val = qs_scheme_heap_value (exe->scheme);

   /* turn it into a block and get its evaluation. */
   val->type_id = QSCRIPT_BLOCK;
   qs_value_restring (val, "<block>");
   val->val_p = rlink->resource->block;
   qs_value_t *rval = qs_value_read (exe, val);

   /* if we pushed a new execution state, pop it. */
   if (new_exe)
      qs_execute_pop (new_exe);

   /* return the evaluation of the block. */
   return rval;
}

#if 0
qs_value_t *qs_execute_parameter (qs_execute_t *exe, qs_parameter_t *p)
{
   qs_value_t *rval;

   /* blocks are immediately resolved. */
   if ((rval = p->value) == NULL)
      return QSV_CANNOT_EXECUTE;
   if (rval->type_id == QSCRIPT_BLOCK) {
      qs_list_t *l = rval->val_p;
      rval = qs_execute_block (exe, QSCRIPT_BLOCK,
         l->parameter_count, l->parameters);
   }

   /* run actions on that value. */
   qs_action_t *a;
   for (a = p->action_list_front; a != NULL; a = a->next)
      rval = qs_action_run (exe, a, rval);

   /* return our final value. */
   return rval;
}
#endif

#if 0
qs_value_t *qs_execute_block (qs_execute_t *exe, int type_id, int args,
   qs_parameter_t **arg)
{
   qs_variable_t *last_var;
   qs_value_t *rval = NULL;
   qs_execute_t *e = exe;

   /* remember where to pop variables after we're done executing this block. */
   /* evaluate all parameters and return the result of the final one. */
   if (type_id == QSCRIPT_BLOCK) {
      last_var = exe->rlink->variable_list_back;
      e = qs_execute_push (QS_EXE_BLOCK, exe->rlink, exe, exe->action,
         exe->name, 0, NULL);
   }

   int i;
   for (i = 0; i < args && !(e->flags & QS_EXE_BREAK); i++)
      rval = qs_execute_parameter (e, arg[i]);
   e->flags &= ~QS_EXE_BREAK;

   /* for blocks, clear 'break' and get rid of all variables used
    * by the block's scope. */
   if (type_id == QSCRIPT_BLOCK) {
      qs_variable_free_after (e->rlink, last_var, &rval);
      qs_execute_pop (e);
   }

   /* return the result of return(), break(), or our last parameter. */
   return rval;
}
#endif

qs_execute_t *qs_execute_get_call (qs_execute_t *exe)
{
   while (exe) {
      if (exe->list && (exe->type_id == QS_EXE_LAMBDA ||
                        exe->type_id == QS_EXE_RESOURCE))
         return exe;
      exe = exe->parent;
   }
   return NULL;
}
