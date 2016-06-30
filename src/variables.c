/* variables.c
 * -----------
 * variable management in multiple scopes. */

#include "execute.h"
#include "funcs.h"
#include "objects.h"
#include "rlinks.h"
#include "schemes.h"
#include "values.h"

#include "variables.h"

qs_variable_t *qs_variable_get (qs_rlink_t *rlink, char *name, int scope)
{
   qs_variable_t *v;
   switch (scope) {
      case QS_SCOPE_OBJECT:
         if ((v = qs_variable_get_object (rlink->object, name)) == NULL)
            return qs_variable_new_object (rlink->object, name);
         return v;
      case QS_SCOPE_BLOCK:
         if ((v = qs_variable_get_rlink (rlink, name)) == NULL)
            return qs_variable_new_rlink (rlink, name);
         return v;
      default:
         return NULL;
   }
}

qs_variable_t *qs_variable_get_object (qs_object_t *obj, char *name)
{
   qs_variable_t *v;
   for (v = obj->variable_list_back; v != NULL; v = v->prev)
      if (strcmp (v->name, name) == 0)
         return v;
   return NULL;
}

qs_variable_t *qs_variable_get_rlink (qs_rlink_t *rlink, char *name)
{
   qs_variable_t *v;
   for (v = rlink->variable_list_back; v != NULL; v = v->prev)
      if (strcmp (v->name, name) == 0)
         return v;
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
   qs_value_copy (NULL, &(new->value), QSV_UNDEFINED);
   return new;
}

qs_variable_t *qs_variable_new_object (qs_object_t *obj, char *name)
{
   qs_variable_t *new;

   /* create a new variable to be popped at the end of the current block. */
   new = qs_variable_new_base (obj->scheme, name);

   /* link to the end of the object. */
   new->link_id = QS_SCOPE_OBJECT;
   new->parent = obj;
   new->prev = obj->variable_list_back;
   if (new->prev) new->prev->next          = new;
   else           obj->variable_list_front = new;
   obj->variable_list_back = new;
   obj->variable_count++;

   /* return our new variable. */
   return new;
}

qs_variable_t *qs_variable_new_rlink (qs_rlink_t *rlink, char *name)
{
   qs_variable_t *new;

   /* create a new variable to be popped at the end of the current block. */
   new = qs_variable_new_base (rlink->scheme, name);

   /* link to the end of the rlink. */
   new->link_id = QS_SCOPE_BLOCK;
   new->parent = rlink;
   new->prev = rlink->variable_list_back;
   if (new->prev) new->prev->next            = new;
   else           rlink->variable_list_front = new;
   rlink->variable_list_back = new;
   rlink->variable_count++;

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
   switch (v->link_id) {
      case QS_SCOPE_OBJECT: {
         qs_object_t *o = v->parent;
         if (v->next) v->next->prev          = v->prev;
         else         o->variable_list_back  = v->prev;
         if (v->prev) v->prev->next          = v->next;
         else         o->variable_list_front = v->next;
         o->variable_count--;
         break;
      }

      case QS_SCOPE_BLOCK: {
         qs_rlink_t *rl = v->parent;
         if (v->next) v->next->prev           = v->prev;
         else         rl->variable_list_back  = v->prev;
         if (v->prev) v->prev->next           = v->next;
         else         rl->variable_list_front = v->next;
         rl->variable_count--;
         break;
      }
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
   for (v = (v ? v->next : exe->rlink->variable_list_front); v != NULL;
        v = v_next) {
      v_next = v->next;

      /* if a return value is the value of a local variable we're about to
       * free, make a copy of it on the heap and return that instead. */
      if (*return_val && qs_variable_contains (v, *return_val)) {
         qs_value_t *new;
         new = qs_return_value_new (exe->rlink->scheme);
         qs_value_copy (exe, new, *return_val);
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
