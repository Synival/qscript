/* values.c
 * --------
 * useful functions for value types. */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "qscript/actions.h"
#include "qscript/execute.h"
#include "qscript/language.h"
#include "qscript/lists.h"
#include "qscript/objects.h"
#include "qscript/parser.h"
#include "qscript/properties.h"
#include "qscript/rlinks.h"
#include "qscript/schemes.h"
#include "qscript/stacks.h"
#include "qscript/variables.h"

#include "qscript/values.h"

/* some constant return values used all the time. */
#define QSV_ERR QS_VALUE_UNDEFINED
QSV_DEFINE (QSV_ZERO,          QS_VALUE_INT, "0",                   0, 0.00f);
QSV_DEFINE (QSV_ONE,           QS_VALUE_INT, "1",                   1, 1.00f);
QSV_DEFINE (QSV_INVALID_TYPE,       QSV_ERR, "<invalid type>",      0, 0.00f);
QSV_DEFINE (QSV_NOT_VARIABLE,       QSV_ERR, "<not variable>",      0, 0.00f);
QSV_DEFINE (QSV_NOT_PROPERTY,       QSV_ERR, "<not property>",      0, 0.00f);
QSV_DEFINE (QSV_CANNOT_MODIFY,      QSV_ERR, "<cannot modify>",     0, 0.00f);
QSV_DEFINE (QSV_NOT_FUNC_CALL,      QSV_ERR, "<not function call>", 0, 0.00f);
QSV_DEFINE (QSV_INVALID_INDEX,      QSV_ERR, "<invalid index>",     0, 0.00f);
QSV_DEFINE (QSV_NO_INDEX,           QSV_ERR, "<no index>",          0, 0.00f);
QSV_DEFINE (QSV_INVALID_SUB_FUNC,   QSV_ERR, "<invalid sub_func>",  0, 0.00f);
QSV_DEFINE (QSV_NOT_ENOUGH_ARGS,    QSV_ERR, "<not enough args>" ,  0, 0.00f);
QSV_DEFINE (QSV_INVALID_FUNC,       QSV_ERR, "<invalid func>",      0, 0.00f);
QSV_DEFINE (QSV_INVALID_OPER,       QSV_ERR, "<invalid operation>", 0, 0.00f);
QSV_DEFINE (QSV_DIVISION_BY_ZERO,   QSV_ERR, "<division by zero>",  0, 0.00f);
QSV_DEFINE (QSV_MOD_BY_ZERO,        QSV_ERR, "<mod by zero>",       0, 0.00f);
QSV_DEFINE (QSV_INVALID_RESOURCE,   QSV_ERR, "<invalid resource>",  0, 0.00f);
QSV_DEFINE (QSV_CANNOT_EXECUTE,     QSV_ERR, "<cannot execute>",    0, 0.00f);
QSV_DEFINE (QSV_CANNOT_UNWRAP,      QSV_ERR, "<cannot unwrap>",     0, 0.00f);
QSV_DEFINE (QSV_SCOPE_LITERAL,      QSV_ERR, "literal",             0, 0.00f);
QSV_DEFINE (QSV_SCOPE_RLINK,        QSV_ERR, "rlink",               0, 0.00f);
QSV_DEFINE (QSV_SCOPE_BLOCK,        QSV_ERR, "block",               0, 0.00f);
QSV_DEFINE (QSV_SCOPE_PROPERTY,     QSV_ERR, "property",            0, 0.00f);
QSV_DEFINE (QSV_SCOPE_UNKNOWN,      QSV_ERR, "unknown",             0, 0.00f);
QSV_DEFINE (QSV_UNDEFINED,QS_VALUE_UNDEFINED,"<undefined>",         0, 0.00f);
QSV_DEFINE (QSV_INVALID_VALUE,      QSV_ERR, "<invalid value>",     0, 0.00f);
QSV_DEFINE (QSV_NO_OBJECT,          QSV_ERR, "<no object>",         0, 0.00f);
QSV_DEFINE (QSV_ALREADY_WOUND,      QSV_ERR, "<already wound>",     0, 0.00f);
QSV_DEFINE (QSV_INVALID_PROPERTY,   QSV_ERR, "<invalid property>",  0, 0.00f);
QSV_DEFINE (QSV_INVALID_ARGS,       QSV_ERR, "<invalid args>",      0, 0.00f);

int qs_value_copy (qs_execute_t *exe, qs_value_t *dest, qs_value_t *src)
   { return qs_value_copy_real (exe, dest, src, 0); }
int qs_value_copy_const (qs_execute_t *exe, qs_value_t *dest, qs_value_t *src)
   { return qs_value_copy_real (exe, dest, src, 1); }

int qs_value_copy_real (qs_execute_t *exe, qs_value_t *dest, qs_value_t *src,
   int force)
{
   /* don't copy anything into itself. */
   if (dest == src)
      return 0;

   /* make sure it's modifiable. */
   if (!force && !qs_value_can_modify (exe, dest)) {
      p_error (exe->action ? exe->action->node : NULL,
         "attempted to modify immutable value '%s'.\n", dest->val_s);
      return 0;
   }

   /* get rid of our own data. */
   qs_value_cleanup (dest);

   /* copy basic data. */
   dest->value_id = src->value_id;
   dest->val_s = strdup (src->val_s);
   dest->val_i = src->val_i;
   dest->val_f = src->val_f;

   /* how do we handle the pointer? */
   switch (dest->value_id) {
      case QS_VALUE_CHAR:
         if (src->data) {
            int *data_i = malloc (sizeof (int));
            *data_i = *((int *) src->data);
            dest->data = data_i;
         }
         break;
      case QS_VALUE_LIST:
         dest->val_p = src->val_p;
         qs_value_list_internalize (dest);
         break;
      case QS_VALUE_OBJECT:
         if (src->data)
            dest->data = strdup (src->data);
         break;
      default:
         dest->val_p = src->val_p;
         if (src->data)
            p_error (dest->node, "BAD ERROR: didn't copy data for type '%s'. "
               "expect undefined behavior.\n", qs_value_type (dest));
         break;
   }

   /* keep a reference to actions. */
   dest->action_list = src->action_list;

   /* return success. */
   return 1;
}

char *qs_value_type (qs_value_t *val)
{
   switch (val->value_id) {
      /* real primitives:
       * ---simple: */
      case QS_VALUE_UNDEFINED: return "undefined";
      case QS_VALUE_INT:       return "int";
      case QS_VALUE_FLOAT:     return "float";
      case QS_VALUE_STRING:    return "string";

      /* ---complex: */
      case QS_VALUE_CHAR:      return "char";
      case QS_VALUE_LIST:      return "list";
      case QS_VALUE_OBJECT:    return "object";

      /* abstract primitives: */
      case QS_VALUE_BLOCK:     return "block";
      case QS_VALUE_VARIABLE:  return "variable";
      case QS_VALUE_PROPERTY:  return "property";
      case QS_VALUE_PROPERTY_VALUE: return "property_value";

      /* who knows! */
      default:                return "unknown";
   }
}

qs_variable_t *qs_value_variable (qs_execute_t *exe, qs_value_t *v)
{
   if (v->link_id == QS_LINK_VARIABLE)
      return v->link;
   switch (v->value_id) {
      case QS_VALUE_VARIABLE:
         return qs_variable_get (exe, v->val_s, v->val_i);
      case QS_VALUE_STRING: {
         char *str = v->val_s;
         int scope = QS_SCOPE_BLOCK;
         if (*str == '$') {
            str++;
            scope = QS_SCOPE_RLINK;
         }
         if (*str == '\0')
            return NULL;
         return qs_variable_get (exe, str, scope);
      }
      default:
         return NULL;
   }
}

qs_property_t *qs_value_property (qs_execute_t *exe, qs_value_t *v)
{
   if (v->link_id == QS_LINK_PROPERTY)
      return v->link;
   return qs_property_get (exe->object, v->val_s);
}

int qs_value_cleanup (qs_value_t *value)
{
   /* free children... */
   if (value->flags & QS_VALUE_FREE_ACTIONS)
      while (value->action_list)
         qs_action_free (value->action_list);
   value->action_list = NULL;

   /* ...and free ourselves. */
   return qs_value_cleanup_base (value);
}

int qs_value_cleanup_base (qs_value_t *value)
{
   /* if there's some type-specific data, free it. */
   if (value->data) {
      switch (value->value_id) {
         case QS_VALUE_LIST:
            qs_list_free (value->data);
            break;
         case QS_VALUE_CHAR:
         case QS_VALUE_OBJECT:
            free (value->data);
            break;
         default:
            p_error (value->node, "no method for data clean-up for value "
               "of type '%s' (%s).\n", qs_value_type (value), value->val_s);
      }
      value->data = NULL;
   }

   /* reset everything else. */
   if (value->val_s) {
      free (value->val_s);
      value->val_s = NULL;
   }
   value->val_i = 0;
   value->val_f = 0.00f;
   value->val_p = 0;

   /* return success. */
   return 1;
}

int qs_value_free (qs_value_t *value)
{
   /* if value's not on the stack and there's a node reference, it's directly
    * linked to the code.  remove reference in the code. */
   if (!value->scheme && value->node)
         value->node->data = NULL;

   /* clean up other stuff and return true. */
   qs_value_cleanup (value);
   if (!(value->flags & QS_VALUE_HEAP))
      free (value);
   return 1;
}

int qs_value_truth (qs_execute_t *exe, qs_value_t *val)
{
   if (val->val_f != 0.00f && 
      (val->value_id == QS_VALUE_INT ||
       val->value_id == QS_VALUE_FLOAT ||
       val->value_id == QS_VALUE_STRING))
      return 1;
   else if (val->value_id == QS_VALUE_CHAR) {
      switch (val->val_s[0]) {
         case 'y': case 'Y': case '1': return 1;
         default:                      return 0;
      }
   }
   else if (val->value_id == QS_VALUE_STRING) {
      if (!(val->val_s[0] == 't' || val->val_s[0] == 'T' ||
            val->val_s[0] == 'y' || val->val_s[0] == 'Y' ||
            val->val_s[0] == 'o' || val->val_s[0] == 'O'))
         return 0;
      if (strcasecmp (val->val_s, "true") == 0) return 1;
      if (strcasecmp (val->val_s, "yes")  == 0) return 1;
      if (strcasecmp (val->val_s, "on")   == 0) return 1;
      return 0;
   }
   else if (val->value_id == QS_VALUE_OBJECT) {
      if (qs_value_object (exe, val))
         return 1;
      else
         return 0;
   }
   else
      return 0;
}

qs_value_t *qs_value_evaluate_block (qs_execute_t *exe, qs_list_t *list)
{
   qs_execute_t *new_exe = NULL;
   int i;

   /* don't bother executing empty blocks. */
   if (list->value_count < 1)
      return QSV_UNDEFINED;

   /* remember where to pop variables after we're done executing this block. */
   if (list->list_id == QS_LIST_BLOCK) {
      if (exe == NULL)
         return QSV_UNDEFINED;
      new_exe = qs_execute_push (QS_EXECUTE_BLOCK, exe->rlink, exe,
         exe->action, exe->name_p, 0, NULL);
      exe = new_exe;
   }

   /* remember the last value we pushed to the stack so we know where to
    * revert to. */
   qs_value_t *last = qs_stack_last (exe->scheme->stack_values);

   /* get the first value.  if there are any more, roll everything on
    * the heap back (it's useless after this point). */
   qs_value_t *rval = qs_value_read (exe, list->values[0]);
   for (i = 1; i < list->value_count &&
               !(exe->flags & QS_EXECUTE_BREAK); i++) {
      qs_stack_pop_to (exe->scheme->stack_values, last);
      rval = qs_value_read (exe, list->values[i]);
   }

   /* clear everything on the heap but our return value. */
   qs_stack_pop_to_except (exe->scheme->stack_values, last, rval);

   /* for blocks, free all variables declared here. */
   if (new_exe) {
      qs_variable_free_after (new_exe, NULL, &rval);
      qs_execute_pop (new_exe);
   }

   /* return our return value. */
   return rval;
}

qs_value_t *qs_value_evaluate (qs_execute_t *exe, qs_value_t *val)
{
   qs_value_t *rval = QSV_CANNOT_EXECUTE;

   /* is this a real primitive or an abstract one? */
   switch (val->value_id) {
      /* real primitives: */
      case QS_VALUE_UNDEFINED:
      case QS_VALUE_INT:
      case QS_VALUE_FLOAT:
      case QS_VALUE_STRING:
      case QS_VALUE_CHAR:
      case QS_VALUE_LIST:
      case QS_VALUE_OBJECT:
         rval = val;
         break;

      /* abstract primitives: */
      case QS_VALUE_BLOCK:
         rval = qs_value_evaluate_block (exe, val->val_p);
         break;
      case QS_VALUE_VARIABLE: {
         qs_variable_t *var = qs_value_variable (exe, val);
         if (var == NULL) {
            p_error (val->node, "cannot get variable for '%s'.\n",
               val->val_s);
            rval = QSV_NOT_VARIABLE;
         }
         else
            rval = qs_variable_value (var);
         break;
      }
      case QS_VALUE_PROPERTY:
      case QS_VALUE_PROPERTY_VALUE: {
         qs_value_t *pv = (val->value_id == QS_VALUE_PROPERTY)
            ? val
            : qs_value_evaluate (exe, val->val_p);
         qs_property_t *p = qs_value_property (exe, pv);
         if (p == NULL) {
            p_error (val->node, "cannot get property for '%s'.\n",
               val->val_s);
            rval = QSV_NOT_PROPERTY;
         }
         else
            rval = qs_property_value (p);
         break;
      }

      /* unhandled: */
      default:
         p_error (val->node, "cannot process value of type '%s' (%d).\n",
            qs_value_type (val), val->value_id);
         return QSV_CANNOT_EXECUTE;
   }

   /* run all actions. */
   qs_action_t *a;
   for (a = val->action_list; a != NULL; a = a->next)
      rval = qs_action_run (exe, rval, a);

   /* return the evaluation. */
   return rval;
}

int qs_value_length (qs_value_t *v)
{
   /* return a value based on type. */
   switch (v->value_id) {
      case QS_VALUE_STRING:
         return strlen (v->val_s);
      case QS_VALUE_LIST:
         return ((qs_list_t *) v->val_p)->value_count;
      default:
         return -1;
   }
}

int qs_value_restring (qs_value_t *v, char *str)
{
   if (v->val_s == str)
      return 0;
   if (v->val_s)
      free (v->val_s);
   if (str)
      v->val_s = strdup (str);
   else
      v->val_s = NULL;
   return 1;
}

int qs_value_can_modify (qs_execute_t *exe, qs_value_t *val)
   { return (qs_value_lvalue_real (exe, val, 0)) ? 1 : 0; }
qs_value_t *qs_value_lvalue (qs_execute_t *exe, qs_value_t *val)
   { return qs_value_lvalue_real (exe, val, 1); }

qs_value_t *qs_value_lvalue_real (qs_execute_t *exe, qs_value_t *val,
   int push)
{
   /* read-only execution states vorbid this. */
   if (exe && exe->flags & QS_EXECUTE_READ_ONLY)
      return NULL;

   /* immutable values will always fail.  literals, for example,
    * don't have this flag. */
   if (!(val->flags & QS_VALUE_MUTABLE))
      return NULL;

   /* the 'char' type makes sure its reference is modifiable. */
   if (val->value_id == QS_VALUE_CHAR)
      if (val->val_p && !qs_value_can_modify (exe, val->val_p))
         return NULL;

   /* properties have additional checks. */
   if (val->link_id == QS_LINK_PROPERTY) {
      qs_property_t *p = val->link;
      if (exe != NULL && p->object != exe->object)
         return NULL;

      /* for properties, push a modification. */
      if (push) {
         qs_property_push (p, exe->rlink);
         return qs_value_lvalue_real (exe, p->value, 0);
      }
   }

   /* all tests passed! we can modify this. */
   return val;
}

char *qs_value_char_pointer (qs_value_t *val)
{
   if (val->value_id != QS_VALUE_CHAR)
      return NULL;
   if (val->val_p == NULL)
      return val->val_s;
   char *str = ((qs_value_t *) val->val_p)->val_s;
   if (val->data) {
      int index = *((int *) val->data);
      if (index < 0 || index >= strlen (str))
         return NULL;
      return str + index;
   }
   return str;
}

int qs_value_as_char (qs_value_t *rval, char *out)
{
   switch (rval->value_id) {
      case QS_VALUE_CHAR:
         *out = rval->val_s[0];
         return 1;

      case QS_VALUE_STRING:
         if (strlen (rval->val_s) != 1)
            return 0;
         *out = rval->val_s[0];
         return 1;

      case QS_VALUE_INT:
      case QS_VALUE_FLOAT:
         if (rval->val_i < 1 || rval->val_i > 255)
            return 0;
         *out = (char) rval->val_i;
         return 1;

      default:
         return 0;
   }
}

int qs_value_update_from_string (qs_value_t *val)
{
   val->val_i = atoi (val->val_s);
   val->val_f = atof (val->val_s);
   return 1;
}

qs_object_t *qs_value_object (qs_execute_t *exe, qs_value_t *val)
{
   qs_object_t *obj;

   /* only allow this for objects. */
   if (val->value_id != QS_VALUE_OBJECT)
      return NULL;

   /* first, look for an object by id.  this will be ultra-quick. */
   if ((obj = qs_object_get_by_id (exe->scheme, val->val_i)) != NULL) {
      /* update the string if necessary. */
      if (val->data == NULL || strcmp (val->data, obj->name) != 0) {
         free (val->data);
         val->data = strdup (obj->name);
      }
      return obj;
   }

   /* is there an object by name? */
   if ((obj = qs_object_get (exe->scheme, val->data)) == NULL)
      return NULL;

   /* we found an object! update our id reference and return it. */
   val->val_i = obj->id;
   return obj;
}

int qs_value_contains (qs_value_t *haystack, qs_value_t *needle)
{
   /* if the value IS variable, then yeah, it's definitely contained. */
   if (haystack == needle)
      return 1;

   /* is 'val' an indexed character reference to 'var'? */
   if (haystack->value_id  == QS_VALUE_STRING
       && needle->value_id == QS_VALUE_CHAR
       && haystack->val_s == needle->val_s)
      return 1;

   /* is this a list? if so, look in the list's self-allocated data. */
   if (haystack->value_id == QS_VALUE_LIST) {
      qs_list_t *list = haystack->data;
      if (list->values_data) {
         int i;
         for (i = 0; i < list->value_count; i++)
            if (list->values_data[i] && qs_value_contains (
               list->values_data[i], needle))
                  return 1;
      }
   }

   /* all checks passed - it's not in there. */
   return 0;
}

int qs_value_init (qs_value_t *val, qs_value_type_e type, ...)
{
   va_list va;
   int rval = 1;

   /* reset variable to our type. */
   qs_value_cleanup_base (val);
   val->value_id = type;

   /* assign parameters based on type. */
   va_start (va, type);
   switch (type) {
      /* simple real primitives. */
      case QS_VALUE_UNDEFINED: {
         char *str = va_arg (va, char *);
         qs_value_restring (val, str ? str : "<undefined>");
         break;
      }
      case QS_VALUE_INT: {
         val->val_i = va_arg (va, int);
         val->val_f = val->val_i;
         char buf[256];
         snprintf (buf, sizeof (buf), "%d", val->val_i);
         qs_value_restring (val, buf);
         break;
      }
      case QS_VALUE_FLOAT: {
         val->val_f = (float) va_arg (va, double);
         val->val_i = (int) val->val_f;
         char buf[256];
         snprintf (buf, sizeof (buf), "%g", val->val_f);
         qs_value_restring (val, buf);
         break;
      }
      case QS_VALUE_STRING:
         qs_value_restring (val, va_arg (va, char *));
         qs_value_update_from_string (val);
         break;

      /* complex real primitives: */
      case QS_VALUE_CHAR: {
         char buf[2];
         buf[0] = (char) va_arg (va, int);
         buf[1] = '\0';
         qs_value_restring (val, buf);
         val->val_i = (int)   buf[0];
         val->val_f = (float) buf[0];

         /* is this part of a string? */
         val->val_p = va_arg (va, char *);
         if (val->val_p) {
            /* it is.  is there an index? */
            int *data = va_arg (va, int *);
            if (data) {
               int *data_p = malloc (sizeof (int));
               *data_p = *data;
               val->data = data_p;
            }
         }
         break;
      }
      case QS_VALUE_LIST:
         qs_value_restring (val, "<list>");
         val->val_p = qs_list_new (val->scheme, va_arg (va, int));
         val->data = val->val_p;
         break;
      case QS_VALUE_OBJECT:
         qs_value_restring (val, "<object>");
         val->data = strdup (va_arg (va, char *));
         val->val_i = va_arg (va, qs_id_t);
         val->val_f = val->val_i;
         break;

      /* abstract primtiives: */
      /* TODO: block? */
      case QS_VALUE_VARIABLE: {
         val->val_i = va_arg (va, int);
         val->val_f = val->val_i;
         char *str = va_arg (va, char *);
         if (val->val_i == QS_SCOPE_AUTO) {
            if (str[0] == '$') {
               qs_value_restring (val, str + 1);
               val->val_i = QS_SCOPE_RLINK;
            }
            else {
               qs_value_restring (val, str);
               val->val_i = QS_SCOPE_BLOCK;
            }
         }
         else
            qs_value_restring (val, str);
         break;
      }
      case QS_VALUE_PROPERTY:
         qs_value_restring (val, va_arg (va, char *));
         break;

      /* unhandled value type. */
      default:
         /* can't initialize this value.  complain. */
         p_error (val->node, "cannot initialize variable of type '%s'.\n",
                  qs_value_type (val));
         rval = 0;
         break;
   }
   va_end (va);

   /* return 1 on success, 0 on error. */
   return rval;
}

qs_list_t *qs_value_list (qs_value_t *value)
{
   if (value->value_id != QS_VALUE_LIST)
      return NULL;
   return value->val_p;
}

int qs_value_list_internalize (qs_value_t *value)
{
   /* not allowed for non-lists. */
   if (value->value_id != QS_VALUE_LIST) {
      p_error (value->node, "attempted to internalize list contents of "
         "'%s' of type '%s'.\n", value->val_s, qs_value_type (value));
      return 0;
   }

   /* if there's other internalized data that doesn't match, free it. */
   if (value->data && value->val_p != value->data) {
      qs_list_free (value->data);
      value->data = NULL;
   }
   /* if there's no data, copy the list and use it ourselves. */
   if (value->data == NULL) {
      value->data = qs_list_copy (value->val_p);
      value->val_p = value->data;
   }
   /* otherwise, make sure all values in the list belong to itself. */
   if (value->val_p == value->data)
      qs_list_internalize (value->val_p);

   /* make sure our list points to our own data and return success. */
   return 1;
}
