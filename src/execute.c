/* execute.c
 * ---------
 * execution of rlinks on objects. */

#include <stdlib.h>
#include <string.h>

#include "qscript/actions.h"
#include "qscript/language.h"
#include "qscript/link.h"
#include "qscript/lists.h"
#include "qscript/parser.h"
#include "qscript/rlinks.h"
#include "qscript/resources.h"
#include "qscript/schemes.h"
#include "qscript/stacks.h"
#include "qscript/values.h"
#include "qscript/variables.h"

#include "qscript/execute.h"

qs_execute_t *qs_execute_push (qs_execute_type_e type, qs_rlink_t *rlink,
   qs_execute_t *exe, qs_action_t *action, char *name, qs_flags_t flags,
   qs_list_t *list)
{
   /* allocate and initialize our new execution state. */
   qs_execute_t *new = qs_stack_push (rlink->scheme->stack_executes,
      qs_execute_sf);
   memset (new, 0, sizeof (qs_execute_t));
   new->scheme     = rlink->scheme;
   new->object     = rlink->object;
   new->execute_id = type;
   new->rlink      = rlink;
   new->action     = action;
   new->flags      = flags;
   new->name_p     = name;
   new->list       = list;

   /* link to parent. */
   new->parent = exe;

   /* copy certain flags over. */
   if (exe)
      new->flags |= (exe->flags & QS_EXECUTE_READ_ONLY);

   /* return our new execution state. */
   return new;
}

int qs_execute_pop (qs_execute_t *exe)
{
   qs_execute_t *e = qs_stack_last (exe->scheme->stack_executes);
   if (exe != e) {
      p_error (exe->action ? exe->action->node : NULL,
         "attempted to execution state that wasn't on top.\n");
      return 0;
   }
   qs_stack_pop (exe->scheme->stack_executes);
   return 1;
}

int qs_execute_cleanup (qs_execute_t *exe)
{
   /* should we free our list? */
   if (exe->flags & QS_EXECUTE_FREE_LIST)
      qs_list_free (exe->list);

   /* free any remaining variables. */
   while (exe->variable_list_back)
      qs_variable_free (exe->variable_list_back);

   /* return success. */
   return 1;
}

QS_STACK_FUNC (qs_execute_sf)
   { return qs_execute_cleanup (data); }

qs_execute_t *qs_execute_get_call (qs_execute_t *exe)
{
   /* only resources and lambda functions potentially have arguments.
    * return the first one of those we find, going back from our current
    * execution state. */
   for (; exe != NULL; exe = exe->parent) {
      if (!exe->list)
         continue;
      if (exe->execute_id == QS_EXECUTE_LAMBDA ||
          exe->execute_id == QS_EXECUTE_RESOURCE)
         return exe;
   }
   return NULL;
}

qs_execute_t *qs_execute_get_block (qs_execute_t *exe)
{
   /* stop when we reach a code block. */
   for (; exe != NULL; exe = exe->parent)
      if (exe->execute_id == QS_EXECUTE_BLOCK ||
          exe->execute_id == QS_EXECUTE_RESOURCE)
         return exe;
   return NULL;
}
