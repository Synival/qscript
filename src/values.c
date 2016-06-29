/* values.c
 * --------
 * useful functions for value types. */

#include "actions.h"
#include "execute.h"
#include "language.h"
#include "lists.h"
#include "objects.h"
#include "properties.h"
#include "rlinks.h"
#include "schemes.h"
#include "variables.h"

#include "values.h"

/* some constant return values used all the time. */
#define QSV_ERR QSCRIPT_UNDEFINED
QSV_DEFINE (QSV_ZERO,           QSCRIPT_INT, "0",                   0, 0.00f);
QSV_DEFINE (QSV_ONE,            QSCRIPT_INT, "1",                   1, 1.00f);
QSV_DEFINE (QSV_INVALID_TYPE,       QSV_ERR, "<invalid type>",      0, 0.00f);
QSV_DEFINE (QSV_NOT_VARIABLE,       QSV_ERR, "<not variable>",      0, 0.00f);
QSV_DEFINE (QSV_NOT_PROPERTY,       QSV_ERR, "<not property>",      0, 0.00f);
QSV_DEFINE (QSV_CANNOT_MODIFY,      QSV_ERR, "<cannot modify>",     0, 0.00f);
QSV_DEFINE (QSV_NOT_FUNC_CALL,      QSV_ERR, "<not function call>", 0, 0.00f);
QSV_DEFINE (QSV_INVALID_INDEX,      QSV_ERR, "<invalid index>",     0, 0.00f);
QSV_DEFINE (QSV_INVALID_SUB_FUNC,   QSV_ERR, "<invalid sub_func>",  0, 0.00f);
QSV_DEFINE (QSV_NOT_ENOUGH_ARGS,    QSV_ERR, "<not enough args>" ,  0, 0.00f);
QSV_DEFINE (QSV_INVALID_FUNC,       QSV_ERR, "<invalid func>",      0, 0.00f);
QSV_DEFINE (QSV_INVALID_OPER,       QSV_ERR, "<invalid operation>", 0, 0.00f);
QSV_DEFINE (QSV_DIVISION_BY_ZERO,   QSV_ERR, "<division by zero>",  0, 0.00f);
QSV_DEFINE (QSV_INVALID_RESOURCE,   QSV_ERR, "<invalid resource>",  0, 0.00f);
QSV_DEFINE (QSV_CANNOT_EXECUTE,     QSV_ERR, "<cannot execute>",    0, 0.00f);
QSV_DEFINE (QSV_CANNOT_UNWRAP,      QSV_ERR, "<cannot unwrap>",     0, 0.00f);
QSV_DEFINE (QSV_SUCCESS,            QSV_ERR, "<success>",           0, 0.00f);
QSV_DEFINE (QSV_NOT_IN_HEAP,        QSV_ERR, "<not in heap>",       0, 0.00f);
QSV_DEFINE (QSV_SCOPE_LITERAL,      QSV_ERR, "literal",             0, 0.00f);
QSV_DEFINE (QSV_SCOPE_OBJECT,       QSV_ERR, "object",              0, 0.00f);
QSV_DEFINE (QSV_SCOPE_BLOCK,        QSV_ERR, "block",               0, 0.00f);
QSV_DEFINE (QSV_SCOPE_PROPERTY,     QSV_ERR, "property",            0, 0.00f);
QSV_DEFINE (QSV_SCOPE_UNKNOWN,      QSV_ERR, "unknown",             0, 0.00f);
QSV_DEFINE (QSV_UNDEFINED,QSCRIPT_UNDEFINED, "<undefined>",         0, 0.00f);
QSV_DEFINE (QSV_INVALID_VALUE,      QSV_ERR, "<invalid value>",     0, 0.00f);
QSV_DEFINE (QSV_NO_OBJECT,          QSV_ERR, "<no object>",         0, 0.00f);
QSV_DEFINE (QSV_ALREADY_WOUND,      QSV_ERR, "<already wound>",     0, 0.00f);

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
      p_error (NULL, "attempted to modify immutable value.\n");
      return 0;
   }

   /* get rid of our own data. */
   qs_value_cleanup (dest);

   /* copy basic data. */
   dest->type_id = src->type_id;
   dest->val_s = strdup (src->val_s);
   dest->val_i = src->val_i;
   dest->val_f = src->val_f;

   /* how do we handle the pointer? */
   switch (dest->type_id) {
      case QSCRIPT_LIST:
         dest->data = qs_list_copy (dest->scheme, src->val_p);
         dest->val_p = dest->data;
         break;
      case QSCRIPT_INDEX:
      case QSCRIPT_CHAR:
         if (src->data) {
            int *data_i = malloc (sizeof (int));
            *data_i = *((int *) src->data);
            dest->data = data_i;
         }
         break;
      default:
         dest->val_p = src->val_p;
         break;
   }

   /* copy children. */
   if (src->child) {
      dest->child = malloc (sizeof (qs_value_t));
      memset (dest->child, 0, sizeof (qs_value_t));
      dest->child->parent = dest;
      dest->child->flags = QS_VALUE_MUTABLE;
      qs_value_copy (exe, dest->child, src->child);
   }

   /* return success. */
   return 1;
}

char *qs_value_type (qs_value_t *val)
{
   switch (val->type_id) {
      case QSCRIPT_UNDEFINED: return "undefined";
      case QSCRIPT_STRING:    return "string";
      case QSCRIPT_FLOAT:     return "float";
      case QSCRIPT_INT:       return "int";
      case QSCRIPT_VARIABLE:  return "variable";
      case QSCRIPT_LIST:      return "list";
      case QSCRIPT_BLOCK:     return "block";
      case QSCRIPT_INDEX:     return "index";
      case QSCRIPT_CHAR:      return "char";
      case QSCRIPT_ACTION:    return "action";
      case QSCRIPT_OBJECT:    return "object";
      case QSCRIPT_PROPERTY:  return "property";
      default:                return "unknown";
   }
}

qs_variable_t *qs_value_get_variable (qs_rlink_t *rlink, qs_value_t *v)
{
   if (v->link_id == QS_LINK_VARIABLE)
      return v->link;
   switch (v->type_id) {
      case QSCRIPT_VARIABLE:
         return qs_variable_get (rlink, v->val_s, v->val_i);
      case QSCRIPT_STRING: {
         char *str = v->val_s;
         int scope = -1;
         if (*str == '$') {
            str++;
            if (*str == '$') {
               str++;
               scope = QS_SCOPE_OBJECT;
            }
            else
               scope = QS_SCOPE_BLOCK;
         }
         if (scope == -1 || *str == '\0')
            return NULL;
         return qs_variable_get (rlink, str, scope);
      }
      default:
         return NULL;
   }
}

qs_property_t *qs_value_get_property (qs_rlink_t *rlink, qs_value_t *v)
{
   if (v->link_id == QS_LINK_PROPERTY)
      return v->link;
   if (v->val_s[0] != '%')
      return NULL;
   else
      return qs_property_get (rlink->object, v->val_s + 1);
}

int qs_value_cleanup (qs_value_t *value)
{
   /* free children. */
   if (value->child) {
      qs_value_free (value->child);
      value->child = NULL;
   }

   /* if there's some type-specific data, free it. */
   if (value->data) {
      switch (value->type_id) {
         case QSCRIPT_LIST:
            qs_list_free (value->data);
            break;
         case QSCRIPT_CHAR:
         case QSCRIPT_INDEX:
            free (value->data);
            break;
         case QSCRIPT_ACTION:
            qs_action_free (value->data);
            break;
         default:
            p_error (value->node, "no method for data clean-up for value "
               "of type '%s' (%s).\n", qs_value_type (value), value->val_s);
      }
      value->data = NULL;
   }
   if (value->val_s) {
      free (value->val_s);
      value->val_s = NULL;
   }
   return 1;
}

int qs_value_free (qs_value_t *value)
{
   /* if value's not on the stack and there's a node reference, it's directly
    * linked to the code.  remove reference in the code. */
   if (!value->scheme && value->node)
         value->node->data = NULL;

   /* get rid of lines to other values. */
   if (value->parent) value->parent->child = value->child;
   if (value->child)  value->child->parent = value->parent;

   /* clean up other stuff and return true. */
   qs_value_cleanup (value);
   free (value);
   return 1;
}

int qs_value_truth (qs_execute_t *exe, qs_value_t *val)
{
   if (val->val_f != 0.00f && 
      (val->type_id == QSCRIPT_INT ||
       val->type_id == QSCRIPT_FLOAT ||
       val->type_id == QSCRIPT_STRING))
      return 1;
   else if (val->type_id == QSCRIPT_CHAR) {
      switch (val->val_s[0]) {
         case 'y': case 'Y': case '1': return 1;
         default:                      return 0;
      }
   }
   else if (val->type_id == QSCRIPT_STRING) {
      if (!(val->val_s[0] == 't' || val->val_s[0] == 'T' ||
            val->val_s[0] == 'y' || val->val_s[0] == 'Y' ||
            val->val_s[0] == 'o' || val->val_s[0] == 'O'))
         return 0;
      if (strcasecmp (val->val_s, "true") == 0) return 1;
      if (strcasecmp (val->val_s, "yes")  == 0) return 1;
      if (strcasecmp (val->val_s, "on")   == 0) return 1;
      return 0;
   }
   else if (val->type_id == QSCRIPT_OBJECT) {
      if (qs_value_object (exe, val))
         return 1;
      else
         return 0;
   }
   else
      return 0;
}

qs_value_t *qs_value_evaluate_block (qs_execute_t *exe, qs_value_t *val)
{
   qs_variable_t *last_var;
   qs_list_t *list = val->val_p;
   qs_value_t *rval = QSV_UNDEFINED;
   qs_execute_t *new_exe = NULL;
   int i;

   /* remember where to pop variables after we're done executing this block. */
   /* evaluate all parameters and return the result of the final one. */
   if (list->type_id == QSCRIPT_BLOCK) {
      last_var = exe->rlink->variable_list_back;
      new_exe = qs_execute_push (QS_EXE_BLOCK, exe->rlink, exe, exe->action,
         exe->name, 0, NULL);
      exe = new_exe;
   }

   /* execute everything in our block. */
   for (i = 0; i < list->value_count && !(exe->flags & QS_EXE_BREAK); i++)
      rval = qs_value_read (exe, list->values[i]);

   /* for blocks, get all of all variables declared in this scope. */
   if (new_exe) {
      qs_variable_free_after (new_exe->rlink, last_var, &rval);
      qs_execute_pop (new_exe);
   }

   /* return our return value. */
   return rval;
}

qs_value_t *qs_value_read (qs_execute_t *exe, qs_value_t *val)
{
   qs_value_t *rval = QSV_CANNOT_EXECUTE;
   while (val) {
      switch (val->type_id) {
         case QSCRIPT_STRING:
         case QSCRIPT_INT:
         case QSCRIPT_FLOAT:
         case QSCRIPT_CHAR:
         case QSCRIPT_LIST:
         case QSCRIPT_OBJECT:
         case QSCRIPT_UNDEFINED:
            rval = val;
            break;
         case QSCRIPT_BLOCK:
            rval = qs_value_evaluate_block (exe, val);
            break;
         case QSCRIPT_ACTION:
            rval = qs_action_run (exe, rval, val->val_p);
            break;
         case QSCRIPT_VARIABLE: {
            qs_variable_t *var = qs_value_get_variable (exe->rlink, val);
            if (var == NULL) {
               p_error (val->node, "cannot get variable for '%s'.\n",
                  val->val_s);
               rval = QSV_NOT_VARIABLE;
            }
            else
               rval = qs_variable_value (var);
            break;
         }
         case QSCRIPT_PROPERTY: {
            qs_property_t *p = qs_value_get_property (exe->rlink, val);
            if (p == NULL) {
               p_error (val->node, "cannot get property for '%s'.\n",
                  val->val_s);
               rval = QSV_NOT_PROPERTY;
            }
            else
               rval = qs_property_value (p);
            break;
         }
         default:
            p_error (val->node, "cannot process value of type '%s'.\n",
               qs_value_type (val));
            return QSV_CANNOT_EXECUTE;
      }
      val = val->child;
   }
   return rval;
}

qs_variable_t *qs_value_variable (qs_execute_t *exe, qs_value_t *val)
   { return qs_value_get_variable (exe->rlink, qs_value_read (exe, val)); }

int qs_value_length (qs_value_t *v)
{
   /* return a value based on type. */
   switch (v->type_id) {
      case QSCRIPT_STRING:
         return strlen (v->val_s);
      case QSCRIPT_LIST:
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
   { return (qs_value_modify_value_real (exe, val, 0)) ? 1 : 0; }
qs_value_t *qs_value_modify_value (qs_execute_t *exe, qs_value_t *val)
   { return qs_value_modify_value_real (exe, val, 1); }

qs_value_t *qs_value_modify_value_real (qs_execute_t *exe,
   qs_value_t *val, int push)
{
   /* read-only execution states vorbid this. */
   if (exe && exe->flags & QS_EXE_READ_ONLY)
      return NULL;

   /* immutable values will always fail.  literals, for example,
    * don't have this flag. */
   if (!(val->flags & QS_VALUE_MUTABLE))
      return NULL;

   /* the 'index' and 'char' types makes sure its reference is modifiable. */
   if (val->type_id == QSCRIPT_INDEX || val->type_id == QSCRIPT_CHAR)
      if (val->val_p && !qs_value_can_modify (exe, val->val_p))
         return NULL;

   /* for properties, push a modification. */
   if (push && val->link_id == QS_LINK_PROPERTY) {
      qs_property_t *p = val->link;
      qs_property_push (p, exe->rlink);
      return qs_value_modify_value_real (exe, p->value, 0);
   }

   /* all tests passed! we can modify this. */
   return val;
}

char *qs_value_char_pointer (qs_value_t *val)
{
   if (val->type_id != QSCRIPT_CHAR)
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
   switch (rval->type_id) {
      case QSCRIPT_CHAR:
         *out = rval->val_s[0];
         return 1;

      case QSCRIPT_STRING:
         if (strlen (rval->val_s) != 1)
            return 0;
         *out = rval->val_s[0];
         return 1;

      case QSCRIPT_INT:
      case QSCRIPT_FLOAT:
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

   /* first, look for an object by id.  this will be ultra-quick. */
   if ((obj = qs_object_get_by_id (exe->scheme, val->val_i)) != NULL) {
      /* update the string if necessary. */
      if (val->val_s[0] != '@' || strcmp (val->val_s + 1, obj->name) != 0) {
         char buf[256];
         snprintf (buf, sizeof (buf), "@%s", obj->name);
         qs_value_restring (val, buf);
      }
      return obj;
   }

   /* is there an object by name? */
   if (val->val_s[0] != '@')
      return NULL;
   if ((obj = qs_object_get (exe->scheme, val->val_s + 1)) == NULL)
      return NULL;

   /* we found an object! update our id reference and return it. */
   val->val_i = obj->id;
   return obj;
}
