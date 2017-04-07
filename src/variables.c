/* variables.c
 * -----------
 * variable management in multiple scopes. */

#include <stdlib.h>
#include <string.h>

#include "qscript/execute.h"
#include "qscript/funcs.h"
#include "qscript/objects.h"
#include "qscript/parser.h"
#include "qscript/rlinks.h"
#include "qscript/schemes.h"
#include "qscript/values.h"

#include "qscript/variables.h"

qs_variable_t *qs_variable_get (qs_execute_t *exe, char *name, int scope)
{
   qs_variable_t *v;
   switch (scope) {
      case QS_SCOPE_RLINK:
         if ((v = qs_variable_get_rlink (exe->rlink, name)) == NULL)
            return qs_variable_new_rlink (exe->rlink, name);
         return v;
      case QS_SCOPE_BLOCK:
         if ((v = qs_variable_get_execute (exe, name)) == NULL)
            return qs_variable_new_execute (exe, name);
         return v;
      default:
         return NULL;
   }
}

qs_variable_t *qs_variable_get_rlink (qs_rlink_t *rlink, char *name)
{
   qs_variable_t *v;
   while (rlink->parent)
      rlink = rlink->parent;
   for (v = rlink->variable_list_back; v != NULL; v = v->prev)
      if (strcmp (v->name, name) == 0)
         return v;
   return NULL;
}

qs_variable_t *qs_variable_get_execute (qs_execute_t *exe, char *name)
{
   qs_variable_t *v;
   while (exe) {
      for (v = exe->variable_list_back; v != NULL; v = v->prev)
         if (strcmp (v->name, name) == 0)
            return v;
      if (exe->execute_id == QS_EXECUTE_RESOURCE)
         break;
      exe = exe->parent;
   }
   return NULL;
}

qs_variable_t *qs_variable_new_base (qs_scheme_t *scheme, char *name)
{
   /* allocate a new variable of default type 'int'. */
   qs_variable_t *new = malloc (sizeof (qs_variable_t));
   memset (new, 0, sizeof (qs_variable_t));
   new->name = strdup (name);
   new->value.link_id  = QS_LINK_VARIABLE;
   new->value.link     = new;
   new->value.scheme   = scheme;
   new->value.flags    = QS_VALUE_MUTABLE;
   qs_value_init (&(new->value), QS_VALUE_UNDEFINED, NULL);
   return new;
}

qs_variable_t *qs_variable_new_rlink (qs_rlink_t *rlink, char *name)
{
   qs_variable_t *new;

   /* declare at the top-most rlink. */
   while (rlink->parent)
      rlink = rlink->parent;

   /* create a new variable to be popped at the end of the current block. */
   new = qs_variable_new_base (rlink->scheme, name);

   /* link to the end of the object. */
   new->scope_id = QS_SCOPE_RLINK;
   new->parent = rlink;
   new->prev = rlink->variable_list_back;
   if (new->prev) new->prev->next            = new;
   else           rlink->variable_list_front = new;
   rlink->variable_list_back = new;
   rlink->variable_count++;

   /* return our new variable. */
   return new;
}

qs_variable_t *qs_variable_new_execute (qs_execute_t *exe, char *name)
{
   qs_variable_t *new;

   /* declare variable at the block. */
   exe = qs_execute_get_block (exe);

   /* create a new variable to be popped at the end of the current block. */
   new = qs_variable_new_base (exe->scheme, name);

   /* link to the end of the rlink. */
   new->scope_id = QS_SCOPE_BLOCK;
   new->parent = exe;
   new->prev = exe->variable_list_back;
   if (new->prev) new->prev->next          = new;
   else           exe->variable_list_front = new;
   exe->variable_list_back = new;
   exe->variable_count++;

   /* return our new variable. */
   return new;
}

qs_value_t *qs_variable_value (qs_variable_t *variable)
   { return &(variable->value); }

int qs_variable_free (qs_variable_t *v)
{
   /* return any allocated strings. */
   if (v->name)
      free (v->name);
   qs_value_cleanup (&(v->value));

   /* remove from whatever list this variable is attached to. */
   switch (v->scope_id) {
      case QS_SCOPE_RLINK: {
         qs_rlink_t *r = v->parent;
         if (v->next) v->next->prev          = v->prev;
         else         r->variable_list_back  = v->prev;
         if (v->prev) v->prev->next          = v->next;
         else         r->variable_list_front = v->next;
         r->variable_count--;
         break;
      }

      case QS_SCOPE_BLOCK: {
         qs_execute_t *e = v->parent;
         if (v->next) v->next->prev          = v->prev;
         else         e->variable_list_back  = v->prev;
         if (v->prev) v->prev->next          = v->next;
         else         e->variable_list_front = v->next;
         e->variable_count--;
         break;
      }

      default:
         p_error (NULL, "qs_variable_free(): unhandled variable scope '%d'.\n",
                  v->scope_id);
   }

   /* free the variable itself and return success. */
   free (v);
   return 1;
}

int qs_variable_free_after (qs_execute_t *exe, qs_variable_t *v,
                            qs_value_t **return_val)
{
   qs_variable_t *v_next;
   int count = 0;

   /* start freeing variables after 'v' (if NULL, start at the beginning). */
   for (v = (v ? v->next : exe->variable_list_front); v != NULL;
        v = v_next) {
      v_next = v->next;

      /* if a return value is the value of a local variable we're about to
       * free, make a copy of it on the heap and return that instead. */
      if (return_val && *return_val && qs_variable_contains (v, *return_val)) {
         qs_value_t *new;
         new = qs_scheme_heap_value (exe->rlink->scheme);
         qs_value_copy_const (exe, new, *return_val);
         *return_val = new;
      }

      /* free the variable. */
      count += qs_variable_free (v);
   }

   /* return the total number of variables freed. */
   return count;
}

int qs_variable_contains (qs_variable_t *var, qs_value_t *val)
{
   return qs_value_contains (&(var->value), val);
}
