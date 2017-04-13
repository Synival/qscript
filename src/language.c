/* language.c
 * ----------
 * processing of the qscript language into resources. */

//#include <dirent.h>
//#include <fcntl.h>
//#include <limits.h>
//#include <stdio.h>
//#include <string.h>
//#include <unistd.h>
//#include <sys/types.h>
//#include <sys/stat.h>

#include "qscript/actions.h"
#include "qscript/link.h"
#include "qscript/lists.h"
#include "qscript/objects.h"
#include "qscript/parser.h"
#include "qscript/resources.h"
#include "qscript/schemes.h"
#include "qscript/values.h"

#include "qscript/language.h"

#define QSCRIPT_SIMPLE_STRING_PATTERN \
   "[a-zA-Z0-9_!@#$%^&\\*+\\-=\\/<>|?]+"
#define QSCRIPT_VARIABLE_PATTERN \
   "[a-zA-Z0-9_]+"

/* list of all symbols for qscripts. */
static p_symbol_t static_qs_language_symbols[] = {
   {QSCRIPT_ROOT,       "qscript",  "/^/ (<resource> | <comment>)* /$/"},
   {QSCRIPT_COMMENT,    "comment",  "'#' /[^\\n]*/"},
   {QSCRIPT_RESOURCE,   "resource", "(<rflags> <rname>) <outer_block>",
      qs_language_resource, qs_language_resource_f},
   {QSCRIPT_RFLAGS,     "rflags",   "/[@]*/", NULL},
   {QSCRIPT_RNAME,      "rname",    "<qstring>", NULL},
   {QSCRIPT_BLOCK,      "block",    "((<comment> | (<value> ';'))* | \"\")",
      qs_language_list, qs_language_list_f},
   {QSCRIPT_LIST,       "list",     "((<value> (',' <value>)* /[,]?/) | \"\")",
      qs_language_list, qs_language_list_f},
   {QSCRIPT_OUTER_LIST, "outer_list","'[' <list> ']'",
      qs_language_outer_list, qs_language_outer_list_f},
   {QSCRIPT_VALUE,      "value",
      "<comment>* <vflags> <primitive> <action>*",
      qs_language_value, qs_language_value_f},
   {QSCRIPT_VFLAGS,     "vflags",   "/[~]*/", NULL},
   {QSCRIPT_PRIMITIVE,  "primitive",
      "(<boolean> | <property_value> | <property> | <number> | <variable> | "
       "<outer_list> | <outer_block> | <char> | <object> | <undefined> | "
       "<qstring>)"},
   {QSCRIPT_OUTER_BLOCK,"outer_block", "'{' <block> '}'",
      qs_language_outer_list, qs_language_outer_list_f},
   {QSCRIPT_STRING,     "qstring", "(<complexstr>+ | <simplestr>)",
      qs_language_qstring},
   {QSCRIPT_CHAR,       "char",    "/'.'/"},
   {QSCRIPT_SIMPLE_STRING, "simplestr",
      "/" QSCRIPT_SIMPLE_STRING_PATTERN "/"},
   {QSCRIPT_COMPLEX_STRING, "complexstr", "(/\"[^\"]*\"/ | /'[^']*'/)",
      qs_language_complex_string},
   {QSCRIPT_NUMBER,     "number",   "(<float> | <int>)"},
   {QSCRIPT_BOOLEAN,    "boolean",  "(\"true\" | \"false\")"},
   {QSCRIPT_FLOAT,      "float",    "/[-+]?[0-9]+\\.[0-9]+/"},
   {QSCRIPT_INT,        "int",      "/[-+]?[0-9]+/"},
   {QSCRIPT_VARIABLE,   "variable",
      "/\\$[$@]?" QSCRIPT_VARIABLE_PATTERN "/",
      qs_language_copy_contents},
   {QSCRIPT_OBJECT,     "object",
      "/[@]+" QSCRIPT_VARIABLE_PATTERN "/",
      qs_language_copy_contents},
   {QSCRIPT_PROPERTY,   "property", "'.' <qstring>"},
   {QSCRIPT_PROPERTY_VALUE, "property_value", "'.' '(' <value> ')'"},
   {QSCRIPT_ACTION,     "action",
      "<comment>* (<call> | <index> | <property_value> | <property>)",
      qs_language_action, qs_language_action_f},
   {QSCRIPT_CALL,       "call",     "'(' <list> ')'"},
   {QSCRIPT_INDEX,      "index",    "'[' <value> ']'"},
   {QSCRIPT_UNDEFINED,  "undefined","\"undefined\""},
   {-1}
};

/* functions for processing various symbols. */
P_FUNC (qs_language_complex_string)
{
   /* should have one child. */
   p_node_t *n = node->first_child;
   size_t len = strlen (n->contents) - 2;

   /* get rid of our quote characters. */
   memmove (n->contents, n->contents + 1, len);
   n->contents[len] = '\0';
}

P_FUNC (qs_language_qstring)
{
   p_node_t *n;
   size_t len;
   char *new_str;

   /* how long will our new string be? */
   len = 1;
   for (n = node->first_child; n != NULL; n = n->next) {
      switch (n->type_id) {
         case QSCRIPT_SIMPLE_STRING:
         case QSCRIPT_COMPLEX_STRING:
            len += strlen (n->first_child->contents);
            break;
      }
   }

   /* re-use 'contents'. */
   if (node->contents)
      free (node->contents);
   new_str = (node->contents = malloc (sizeof (char) * len));
   new_str[0] = '\0';
   len = 0;

   /* build our string. */
   for (n = node->first_child; n != NULL; n = n->next) {
      switch (n->type_id) {
         case QSCRIPT_SIMPLE_STRING:
         case QSCRIPT_COMPLEX_STRING:
            len += sprintf (new_str + len, "%s",
                            n->first_child->contents);
            break;
      }
   }
}

P_FUNC (qs_language_value)
{
   qs_value_t *v;
   qs_action_t *a = NULL;

   /* link an empty value. */
   v = calloc (1, sizeof (qs_value_t));
   v->scheme = qs_scheme_from_node (node);
   v->node = node;
   v->link_id = QS_LINK_LITERAL;
   node->data = v;

   /* process the rest of our arguments. */
   p_node_t *vn;
   for (vn = node->first_child; vn != NULL; vn = vn->next) {
      switch (vn->type_id) {
         case QSCRIPT_COMMENT:
            break;

         case QSCRIPT_VFLAGS: {
            char *ch;
            for (ch = vn->first_child->contents; *ch != '\0'; ch++) {
               switch (*ch) {
                  /* if a value begins with '~', the contents must be of type
                  * QSCRIPT_LIST.  the list contents will be inserted into a
                  * function/resource/lambda call's argument list.  this is
                  * called 'unwrapping'. */
                  case '~':
                     v->flags |= QS_VALUE_UNWRAP;
                     break;
               }
            }
            break;
         }

         /* the main value. */
         case QSCRIPT_PRIMITIVE: {
            p_node_t *n = vn->first_child;
            switch (n->type_id) {
               /* primitive types. */
               case QSCRIPT_UNDEFINED:
                  qs_value_free (v);
                  v = QSV_UNDEFINED;
                  node->data = v;
                  break;
               case QSCRIPT_NUMBER:
                  switch (n->first_child->type_id) {
                     case QSCRIPT_INT:
                        qs_value_init (v, QS_VALUE_INT,
                           atoi (n->first_child->first_child->contents));
                        break;
                     case QSCRIPT_FLOAT:
                        qs_value_init (v, QS_VALUE_FLOAT,
                           atof (n->first_child->first_child->contents));
                        break;
                  }
                  break;
               case QSCRIPT_OBJECT:
                  qs_value_init (v, QS_VALUE_OBJECT, n->contents + 1,
                                 (qs_id_t) 0);
                  break;
               case QSCRIPT_VARIABLE:
                  qs_value_init (v, QS_VALUE_VARIABLE, QS_SCOPE_AUTO,
                                 n->contents + 1);
                  break;
               case QSCRIPT_PROPERTY:
                  qs_value_init (v, QS_VALUE_PROPERTY,
                                 n->first_child->next->contents);
                  break;
               case QSCRIPT_PROPERTY_VALUE:
                  v->value_id = QS_VALUE_PROPERTY_VALUE;
                  v->val_s = strdup ("<property_value>");
                  v->val_p = n->first_child->next->next->data;
                  break;
               case QSCRIPT_CHAR:
                  qs_value_init (v, QS_VALUE_CHAR, (int)
                     n->first_child->contents[1], NULL, NULL);
                  break;
               case QSCRIPT_STRING:
                  qs_value_init (v, QS_VALUE_STRING, n->contents);
                  break;

               /* these are internal and handled deeper in the parse tree. */
               case QSCRIPT_OUTER_LIST:
                  v->value_id = QS_VALUE_LIST;
                  v->val_s = strdup ("<list>");
                  v->val_p = n->data;
                  break;
               case QSCRIPT_OUTER_BLOCK:
                  v->value_id = QS_VALUE_BLOCK;
                  v->val_s = strdup ("<block>");
                  v->val_p = n->data;
                  break;
               case QSCRIPT_BOOLEAN:
                  qs_value_init (v, QS_VALUE_BOOLEAN,
                     strcmp (n->first_child->contents, "true") == 0);
                  break;
               default:
                  p_error (vn, "invalid primitive type '%d'.\n", n->type_id);
            }
            break;
         }

         /* any actions to be performed on the value. */
         case QSCRIPT_ACTION:
            if (a) {
               a->next = vn->data;
               a->next->prev = a;
               a = a->next;
            }
            else {
               v->flags |= QS_VALUE_FREE_ACTIONS;
               v->action_list = vn->data;
               a = v->action_list;
            }
            a->parent_value = v;
            break;

         default:
            p_error (node, "invalid value type '%d'.\n", vn->type_id);
      }
   }
}
P_FUNC (qs_language_value_f)
{
   if (node->data && node->data != QSV_UNDEFINED)
      qs_value_free (node->data);
}

P_FUNC (qs_language_outer_list)
{
   /* steal our child node's data. */
   p_node_t *n = node->first_child->next;
   if (n->data) {
      qs_list_t *l = n->data;
      l->node = node;
      node->data = l;
      n->data = NULL;
   }
   /* empty lists will have no data.  create it here. */
   /* FIXME: this is a hack around undesirable behavior in MPC.
    *        perhaps there's a cleaner way? */
   else {
      /* allocate an empty list and link it to this node. */
      qs_list_t *l = calloc (1, sizeof (qs_list_t));
      l->scheme = qs_scheme_from_node (node);
      l->values = malloc (sizeof (qs_value_t *));
      l->node = node;
      node->data = l;

      /* assign the type. */
      switch (node->type_id) {
         case QSCRIPT_OUTER_LIST:  l->list_id = QS_LIST_LIST;  break;
         case QSCRIPT_OUTER_BLOCK: l->list_id = QS_LIST_BLOCK; break;
      }
   }
}
P_FUNC (qs_language_outer_list_f)
   { if (node->data) qs_list_free (node->data); }

P_FUNC (qs_language_list)
{
   p_node_t *n;
   int index = 0;

   /* create an empty list. */
   qs_list_t *l = calloc (1, sizeof (qs_list_t));
   l->scheme = qs_scheme_from_node (node);
   l->node = node;
   node->data = l;

   /* what kind of list? */
   switch (node->type_id) {
      case QSCRIPT_LIST:  l->list_id = QS_LIST_LIST;  break;
      case QSCRIPT_BLOCK: l->list_id = QS_LIST_BLOCK; break;
      default:            l->list_id = QS_LIST_UNDEFINED;
   }

   /* allocate our parameters. */
   l->value_count = 0;
   for (n = node->first_child; n != NULL; n = n->next)
      if (n->type_id == QSCRIPT_VALUE && n->data)
         ++(l->value_count);
   l->values = malloc (sizeof (qs_value_t *) *
      ((l->value_count > 0) ? l->value_count : 1));

   /* assign our function name, parameters, and determine formatting. */
   for (n = node->first_child; n != NULL; n = n->next)
      if (n->type_id == QSCRIPT_VALUE && n->data) {
         l->values[index] = n->data;
         l->values[index]->list = l;
         index++;
      }
}
P_FUNC (qs_language_list_f)
   { if (node->data) qs_list_free (node->data); }

P_FUNC (qs_language_action)
{
   /* allocate a simple action type. */
   qs_action_t *a = calloc (1, sizeof (qs_action_t));
   a->node = node;
   node->data = a;

   /* what kind of action is it? */
   p_node_t *n;
   for (n = node->first_child; n != NULL; n = n->next) {
      switch (n->type_id) {
         case QSCRIPT_CALL:
            a->action_id = QS_ACTION_CALL;
            a->data_p = n->first_child->next->data;
            break;
         case QSCRIPT_INDEX:
            a->action_id = QS_ACTION_INDEX;
            a->data_p = n->first_child->next->data;
            break;
         case QSCRIPT_PROPERTY:
            a->action_id = QS_ACTION_PROPERTY;
            a->data_p = n->first_child->next->contents;
            break;
         case QSCRIPT_PROPERTY_VALUE:
            a->action_id = QS_ACTION_PROPERTY_VALUE;
            a->data_p = n->first_child->next->next->data;
            break;
      }
   }
}
P_FUNC (qs_language_action_f)
   { if (node->data) qs_action_free (node->data); }

P_FUNC (qs_language_resource)
{
   p_node_t *n;
   qs_list_t *list = NULL;
   qs_flags_t flags = 0;
   char *name = NULL, *ch;

   /* get our flags, name, and code block. */
   for (n = node->first_child; n != NULL; n = n->next) {
      switch (n->type_id) {
         /* process flags. */
         case QSCRIPT_RFLAGS:
            for (ch = n->first_child->contents; *ch != '\0'; ch++) {
               switch (*ch) {
                  case '@': flags |= QS_RESOURCE_GLOBAL; break;
               }
            }
            break;

         /* process name. */
         case QSCRIPT_RNAME:
            name = n->first_child->contents;
            break;

         /* statement block. */
         case QSCRIPT_OUTER_BLOCK:
            list = n->data;
            break;
      }
   }

   /* create our new resource. */
   qs_resource_t *r;
   if ((r = qs_resource_new (node->parse_data, name, list, flags)) != NULL) {
      node->data = r;
      r->node = node;
   }
}
P_FUNC (qs_language_resource_f)
   { if (node->data) qs_resource_free (node->data); }

P_FUNC (qs_language_copy_contents)
{
   if (node->contents)
      free (node->contents);
   node->contents = node->first_child->contents;
   node->first_child->contents = NULL;
}

int qs_language_init (void)
{
   /* create our language if we haven't already.
    * if it fails, return -1 from now on. */
   static int language_built = 0;
   if (language_built == -1)
      return -1;
   else if (language_built == 1)
      return 0;
   if (!p_language_new (qs_language_symbols ()))
      language_built = -1;
   else
      language_built = 1;
   return language_built;
}

p_symbol_t *qs_language_symbols (void)
{
   return static_qs_language_symbols;
}
