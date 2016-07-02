/* execute.c
 * ---------
 * execution of rlinks on objects. */

#include <stdlib.h>
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
   new->object  = rlink->object;
   new->type_id = type;
   new->rlink   = rlink;
   new->action  = action;
   new->flags   = flags;
   new->name    = strdup (name);
   new->list    = list;

   /* link to parent. */
   new->parent = exe;

   /* copy certain flags over. */
   if (exe)
      new->flags |= (exe->flags & QS_EXE_READ_ONLY);

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

   /* free any remaining variables. */
   while (exe->variable_list_back)
      qs_variable_free (exe->variable_list_back);

   /* unlink. */
   QS_UNLINK (exe->scheme, exe, exe);

   /* free ourselves and return success. */
   free (exe);
   return 1;
}

qs_execute_t *qs_execute_get_call (qs_execute_t *exe)
{
   /* only resources and lambda functions potentially have arguments.
    * return the first one of those we find, going back from our current
    * execution state. */
   for (; exe != NULL; exe = exe->parent) {
      if (!exe->list)
         continue;
      switch (exe->type_id) {
         case QS_EXE_LAMBDA:
         case QS_EXE_RESOURCE:
            return exe;
      }
   }
   return NULL;
}

qs_execute_t *qs_execute_get_block (qs_execute_t *exe)
{
   /* stop when we reach a code block. */
   for (; exe != NULL; exe = exe->parent)
      if (exe->type_id == QS_EXE_BLOCK)
         return exe;
   return NULL;
}
