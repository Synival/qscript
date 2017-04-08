/* actions.c
 * ---------
 * parsing of parameters during execution. */

#include <stdlib.h>
#include <string.h>

#include "qscript/execute.h"
#include "qscript/funcs.h"
#include "qscript/language.h"
#include "qscript/lists.h"
#include "qscript/objects.h"
#include "qscript/parser.h"
#include "qscript/properties.h"
#include "qscript/resources.h"
#include "qscript/rlinks.h"
#include "qscript/schemes.h"
#include "qscript/stacks.h"
#include "qscript/values.h"
#include "qscript/variables.h"

#include "qscript/actions.h"

int qs_action_parameters (qs_execute_t *exe, qs_action_t *action,
   char *func_name, qs_list_t *in, qs_list_t **out)
{
   int args = in->value_count;
   qs_value_t **arg = in->values;

   /* any parameters need to be unwrapped? */
   int i, unwrap = 0;
   for (i = 0; i < args; i++)
      if (arg[i]->flags & QS_VALUE_UNWRAP)
         unwrap++;
   if (unwrap == 0) {
      *out = in;
      return 0;
   }

   /* there are; unwrap them! */
   int w = 0, new_args = args;
   qs_list_t *l;

   /* allocate reference storage for our new values. */
   qs_value_t *v, **vals = malloc (sizeof (qs_value_t *) * unwrap);

   /* gather those values! */
   for (i = 0; i < args; i++) {
      if (!(arg[i]->flags & QS_VALUE_UNWRAP))
         continue;

      /* get our value.  if it's not a list, complain and bail. */
      v = qs_value_read (exe, arg[i]);
      if (v->value_id != QS_VALUE_LIST) {
         qs_func_error (exe, func_name, arg[i]->node, "cannot unwrap; "
            "argument '%s' is type '%s' instead of type <list>.\n",
            qs_value_type (v), v->val_s);
         free (vals);
         *out = NULL;
         return -1;
      }

      /* add to our reference storage and add to our new parameter count. */
      vals[w++] = v;
      l = v->val_p;
      new_args += l->value_count - 1;
   }

   /* build our new argument list. */
   qs_value_t **new_arg = malloc (sizeof (qs_value_t *) * new_args);
   int j;
   for (i = 0, w = 0, v = *vals; i < args; i++) {
      if (arg[i]->flags & QS_VALUE_UNWRAP) {
         l = v->val_p;
         for (j = 0; j < l->value_count; j++)
            new_arg[w++] = l->values[j];
         v++;
      }
      else
         new_arg[w++] = arg[i];
   }

   /* clean up. */
   free (vals);

   /* create a new list. */
   *out = malloc (sizeof (qs_list_t));
   memset (*out, 0, sizeof (qs_list_t));
   (*out)->scheme      = exe->scheme;
   (*out)->value_count = new_args;
   (*out)->values      = new_arg;

   /* return 1 to indicate we allocated our own data. */
   return 1;
}

qs_value_t *qs_action_run (qs_execute_t *exe, qs_value_t *val,
   qs_action_t *action)
{
   /* what kind of action are we performing? */
   switch (action->action_id) {
      case QS_ACTION_CALL:
         return qs_action_call (exe, val, action);
      case QS_ACTION_INDEX:
         return qs_action_index (exe, val, action);
      case QS_ACTION_PROPERTY:
      case QS_ACTION_PROPERTY_VALUE:
         return qs_action_property (exe, val, action);
      default:
         p_error (action->node, "unknown action type '%d'.\n",
                  action->action_id);
         return QSV_CANNOT_EXECUTE;
   }
}

qs_value_t *qs_action_call (qs_execute_t *exe, qs_value_t *val,
   qs_action_t *action)
{
   qs_scheme_t *scheme = action->node->parse_data;
   qs_func_t *func;
   qs_resource_t *r;
   qs_execute_t *e;

   /* we can only execute strings and lists. */
   if (val->value_id != QS_VALUE_STRING && val->value_id != QS_VALUE_LIST) {
      p_error (action->node, "cannot execute call action for value of "
         "type '%s'.\n", qs_value_type (val));
      return QSV_CANNOT_EXECUTE;
   }

   /* determine our arguments. */
   qs_list_t *l_in = action->data_p, *l = NULL;
   if (qs_action_parameters (exe, action, val->val_s, l_in, &l) < 0)
      return QSV_CANNOT_EXECUTE;

   /* new parameters?  push them to our stack so later functions know
    * what they were called with.  make sure they're deallocated later. */
   e = qs_execute_push (-1, exe->rlink, exe, action, val->val_s, 0, l);
   if (l != l_in)
      e->flags |= QS_EXECUTE_FREE_LIST;

   /* if our function is a list, it's a lambda functions; run it. */
   qs_value_t *rval = QSV_CANNOT_EXECUTE;
   if (val->value_id == QS_VALUE_LIST) {
      l = val->val_p;
      e->execute_id = QS_EXECUTE_LAMBDA;
      rval = qs_value_evaluate_block (e, val->val_p);
   }
   /* if there's a resource, run it. */
   else if ((r = qs_resource_get (scheme, val->val_s)) != NULL) {
      e->execute_id = QS_EXECUTE_RESOURCE;
      e->rlink = qs_rlink_inject_at (e->object, r, e->rlink->priority,
         e->rlink, e->rlink->child_back, e, action);
      rval = qs_rlink_wind_from (e->rlink, e);
   }
   /* if there's a built-in function, run it. */
   else if ((func = qs_scheme_get_func (scheme, val->val_s)) != NULL) {
      e->execute_id = QS_EXECUTE_FUNC;
      qs_value_t *last = qs_stack_last (scheme->stack_values);
      rval = qs_func_run (e, func);
      qs_stack_pop_to_except (scheme->stack_values, last, rval);
   }
   /* couldn't find anything. return an error. */
   else {
      p_error (action->node, "invalid function '%s'\n", val->val_s);
      rval = QSV_INVALID_FUNC;
   }

   /* we're done with our execution state. */
   qs_execute_pop (e);

   /* return whatever our function did. */
   return rval;
}

qs_value_t *qs_action_index (qs_execute_t *exe, qs_value_t *val,
   qs_action_t *action)
{
   int length;

   /* complain if our value is not a valid type. */
   length = qs_value_length (val);
   switch (val->value_id) {
      case QS_VALUE_LIST:
         if (length == 0) {
            p_error (action->node, "cannot index empty list.\n");
            return QSV_INVALID_INDEX;
         }
         break;
      case QS_VALUE_STRING:
         if (length == 0) {
            p_error (action->node, "cannot index blank string.\n");
            return QSV_INVALID_INDEX;
         }
         break;
      default:
         p_error (action->node, "cannot index value '%s' of type '%s'.\n",
            val->val_s, qs_value_type (val));
         return QSV_INVALID_INDEX;
   }

   /* get the index value. */
   qs_value_t *ival = qs_value_read (exe, action->data_p);

   /* enforce type 'int'. */
   if (ival->value_id != QS_VALUE_INT) {
      p_error (action->node, "cannot index using type '%s' (must be "
         "'int').\n", qs_value_type (ival));
      return QSV_INVALID_INDEX;
   }

   /* valid index? */
   int index = ival->val_i;
   if (index < 0) {
      p_error (action->node, "index value '%d' cannot be negative.\n",
         index);
      return QSV_INVALID_INDEX;
   }
   if (index >= length) {
      p_error (action->node, "index value '%d' is too high "
         "(max = '%d')\n", index, length - 1);
      return QSV_INVALID_INDEX;
   }

   /* return different sub-values cased on type. */
   qs_value_t *rval = QSV_CANNOT_EXECUTE;
   switch (val->value_id) {
      case QS_VALUE_LIST:
         rval = ((qs_list_t *) val->val_p)->values[ival->val_i];
         break;

      case QS_VALUE_STRING: {
         /* add a value to the heap that contains our character. */
         rval = qs_scheme_heap_value (exe->scheme);
         rval->value_id = QS_VALUE_CHAR;
         rval->flags |= (val->flags & QS_VALUE_MUTABLE);

         /* for reading purposes, it will be a virtual 'char' type. */
         qs_value_restring (rval, "0");
         rval->val_s[0] = val->val_s[ival->val_i];
         rval->val_i = rval->val_s[0];
         rval->val_f = rval->val_s[0];
         rval->val_p = val;

         /* data will contain an integer with the character index. */
         int *data_i = malloc (sizeof (int));
         *data_i = ival->val_i;
         rval->data = data_i;

         /* done!  return it. */
         return rval;
      }

      default:
         p_error (val->node, "cannot index value of type '%s'.\n",
            qs_value_type (val));
         return QSV_INVALID_TYPE;
   }

   /* dereference our rval and return it. */
   return qs_value_read (exe, rval);
}

qs_value_t *qs_action_property (qs_execute_t *exe, qs_value_t *val,
   qs_action_t *action)
{
   /* complain if val isn't of type 'object'. */
   if (val->value_id != QS_VALUE_OBJECT) {
      p_error (action->node, "cannot get member value from "
               "non-object (type is '%s'). \n", qs_value_type (val));
      return QSV_INVALID_TYPE;
   }

   /* get our object. */
   qs_object_t *obj;
   if ((obj = qs_value_object (exe, val)) == NULL) {
      p_error (action->node, "cannot get object from object reference "
         "'%s' (#%d).\n", (char *) val->data, val->val_i);
      return QSV_NO_OBJECT;
   }

   /* get the name of the property to return. */
   char *str = (action->action_id == QS_ACTION_PROPERTY)
      ? action->data_p
      : qs_value_read (exe, action->data_p)->val_s;

   /* try to get it. */
   qs_property_t *p;
   if ((p = qs_property_get (obj, str)) == NULL) {
      p_error (action->node, "cannot get property '%s' from object "
         "'%s' (#%d).\n", str, obj->name, obj->id);
      return QSV_INVALID_PROPERTY;
   }
   return qs_property_value (p);
}

int qs_action_free (qs_action_t *a)
{
   /* detach from node. */
   if (a->node)
      a->node->data = NULL;

   /* TODO: free data based on action type. */
   if (a->data)
      p_error (a->node, "not method for freeing action data.\n");

   /* unlink from the value. */
   if (a->next) a->next->prev = a->prev;
   if (a->prev) a->prev->next = a->next;
   else a->parent_value->action_list = a->next;

   /* free remaining data and return success. */
   free (a);
   return 1;
}
