/* language.c
 * ----------
 * processing of the qscript language into resources. */

#include <stdio.h>
#include <string.h>

#include "actions.h"
#include "link.h"
#include "lists.h"
#include "objects.h"
#include "resources.h"
#include "schemes.h"
#include "values.h"

#include "language.h"

#define QSCRIPT_SIMPLE_STRING_PATTERN \
   "[a-zA-Z0-9_!@#$%^&\\*+\\-\\.=\\/<>|?]+"
#define QSCRIPT_VARIABLE_PATTERN \
   "[a-zA-Z0-9_]+"

/* list of all symbols for qscripts. */
static p_symbol_t static_qs_symbols[] = {
   {QSCRIPT_ROOT,       "qscript",  "/^/ (<resource> | <comment>)* /$/"},
   {QSCRIPT_COMMENT,    "comment",  "'#' /[^\\n]*/"},
   {QSCRIPT_RESOURCE,   "resource", "(<rtype> <rname>) <outer_block>",
      qs_func_resource, qs_func_resource_f},
   {QSCRIPT_RTYPE,      "rtype",    "/[@]*/", NULL},
   {QSCRIPT_RNAME,      "rname",    "<qstring>", NULL},
   {QSCRIPT_BLOCK,      "block",    "((<value> ';')* | \"\")",
      qs_func_list, qs_func_list_f},
   {QSCRIPT_LIST,       "list",     "((<value> (',' <value>)*) | \"\")",
      qs_func_list, qs_func_list_f},
   {QSCRIPT_OUTER_LIST, "outer_list","'[' <list> ']'",
      qs_func_outer_list, qs_func_outer_list_f},
   {QSCRIPT_VALUE,      "value",
      "<comment>* <unwrap> (<number> | <variable> | <property> | "
      "<outer_list> | <outer_block> | <char> | <object> | <qstring>) "
      "<action>*",
      qs_func_value, qs_func_value_f},
   {QSCRIPT_OUTER_BLOCK,"outer_block", "'{' <block> '}'",
      qs_func_outer_list, qs_func_outer_list_f},
   {QSCRIPT_STRING,     "qstring", "(<complexstr>+ | <simplestr>)",
      qs_func_qstring},
   {QSCRIPT_CHAR,       "char",    "/'.'/"},
   {QSCRIPT_SIMPLE_STRING, "simplestr",
      "/" QSCRIPT_SIMPLE_STRING_PATTERN "/"},
   {QSCRIPT_COMPLEX_STRING, "complexstr", "(/\"[^\"]*\"/ | /'[^']*'/)",
      qs_func_complex_string},
   {QSCRIPT_NUMBER,     "number",   "(<float> | <int>)"},
   {QSCRIPT_FLOAT,      "float",    "/[-+]?[0-9]+\\.[0-9]+/"},
   {QSCRIPT_INT,        "int",      "/[-+]?[0-9]+/"},
   {QSCRIPT_VARIABLE,   "variable",
      "/[\\$]+" QSCRIPT_VARIABLE_PATTERN "/",
      qs_func_copy_contents},
   {QSCRIPT_OBJECT,     "object",
      "/[@]+" QSCRIPT_VARIABLE_PATTERN "/",
      qs_func_copy_contents},
   {QSCRIPT_PROPERTY,   "property",
      "/%" QSCRIPT_VARIABLE_PATTERN "/",
      qs_func_copy_contents},
   {QSCRIPT_ACTION,     "action",   "(<call> | <index>)",
      qs_func_action, qs_func_action_f},
   {QSCRIPT_CALL,       "call",     "'(' <list> ')'"},
   {QSCRIPT_UNWRAP,     "unwrap",   "('~' | \"\")"},
   {QSCRIPT_INDEX,      "index",    "'[' <value> ']'"},
   {-1}
};

/* functions for processing various symbols. */
P_FUNC (qs_func_complex_string)
{
   /* should have one child. */
   p_node_t *n = node->first_child;
   size_t len = strlen (n->contents) - 2;

   /* get rid of our quote characters. */
   memmove (n->contents, n->contents + 1, len);
   n->contents[len] = '\0';
}

P_FUNC (qs_func_qstring)
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

P_FUNC (qs_func_value)
{
   qs_value_t *v, *v_next;
   p_node_t *n;
   char buf[256];

   /* link an empty value. */
   v = malloc (sizeof (qs_value_t));
   memset (v, 0, sizeof (qs_value_t));
   v->node = node;
   node->data = v;

   v_next = v;
   for (n = node->first_child; n != NULL; n = n->next) {
      switch (n->type_id) {
         case QSCRIPT_UNWRAP:
            if (n->first_child->contents[0] == '~')
               v->flags |= QS_VALUE_UNWRAP;
            break;
         case QSCRIPT_NUMBER:
            v->val_i = atoi (n->first_child->first_child->contents);
            v->val_f = atof (n->first_child->first_child->contents);
            switch (n->first_child->type_id) {
               case QSCRIPT_INT:
                  v->type_id = QSCRIPT_INT;
                  snprintf (buf, sizeof (buf), "%d", v->val_i);
                  v->val_s = strdup (buf);
                  break;
               case QSCRIPT_FLOAT:
                  v->type_id = QSCRIPT_FLOAT;
                  snprintf (buf, sizeof (buf), "%g", v->val_f);
                  v->val_s = strdup (buf);
                  break;
            }
            break;
         case QSCRIPT_OBJECT:
            v->type_id = QSCRIPT_OBJECT;
            v->val_s = strdup (n->contents);
            break;
         case QSCRIPT_VARIABLE:
            v->type_id = QSCRIPT_VARIABLE;
            /* (contents[0] is guaranteed to be '$', so this is safe.) */
            if (n->contents[1] == '$') {
               v->val_s = strdup (n->contents + 2);
               v->val_i = QS_SCOPE_OBJECT;
            }
            else {
               v->val_s = strdup (n->contents + 1);
               v->val_i = QS_SCOPE_BLOCK;
            }
            v->val_f = 0.00f;
            break;
         case QSCRIPT_PROPERTY:
            v->type_id = QSCRIPT_PROPERTY;
            v->val_s = strdup (n->contents);
            break;
         case QSCRIPT_CHAR:
            v->type_id = QSCRIPT_CHAR;
            v->val_s = strdup ("0");
            v->val_s[0] = n->first_child->contents[1];
            v->val_i = v->val_s[0];
            v->val_f = v->val_s[0];
            break;
         case QSCRIPT_STRING:
            v->type_id = QSCRIPT_STRING;
            v->val_s = strdup (n->contents);
            v->val_i = atoi (n->contents);
            v->val_f = atoi (n->contents);
            break;
         case QSCRIPT_OUTER_LIST:
            v->type_id = QSCRIPT_LIST;
            v->val_s = strdup ("<list>");
            v->val_p = n->data;
            break;
         case QSCRIPT_OUTER_BLOCK:
            v->type_id = QSCRIPT_BLOCK;
            v->val_s = strdup ("<block>");
            v->val_p = n->data;
            break;
         case QSCRIPT_ACTION:
            v_next->child = n->data;
            v_next->child->parent = v_next;
            v_next = v_next->child;
            break;
      }
   }
}
P_FUNC (qs_func_value_f)
   { if (node->data) qs_value_free (node->data); }

P_FUNC (qs_func_outer_list)
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
      qs_list_t *l = malloc (sizeof (qs_list_t));
      memset (l, 0, sizeof (qs_list_t));
      l->values = malloc (sizeof (qs_value_t *));
      l->node = node;
      node->data = l;

      /* assign the type. */
      switch (node->type_id) {
         case QSCRIPT_OUTER_LIST:  l->type_id = QSCRIPT_LIST;  break;
         case QSCRIPT_OUTER_BLOCK: l->type_id = QSCRIPT_BLOCK; break;
      }
   }
}
P_FUNC (qs_func_outer_list_f)
   { if (node->data) qs_list_free (node->data); }

P_FUNC (qs_func_list)
{
   p_node_t *n;
   int index = 0;

   /* create an empty list. */
   qs_list_t *l = malloc (sizeof (qs_list_t));
   memset (l, 0, sizeof (qs_list_t));
   l->type_id = node->type_id;
   l->node = node;
   node->data = l;

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
P_FUNC (qs_func_list_f)
   { if (node->data) qs_list_free (node->data); }

P_FUNC (qs_func_action)
{
   /* create a value for this node. */
   qs_value_t *v = malloc (sizeof (qs_value_t));;
   memset (v, 0, sizeof (qs_value_t));
   v->type_id = QSCRIPT_ACTION;
   v->node = node;
   node->data = v;

   /* make this value of type 'action'. */
   qs_value_restring (v, "<action>");
   qs_action_t *a = malloc (sizeof (qs_action_t));
   memset (a, 0, sizeof (qs_action_t));
   a->value = v;
   v->data  = a;
   v->val_p = a;

   /* what kind of action is it? */
   p_node_t *n;
   for (n = node->first_child; n != NULL; n = n->next) {
      switch (n->type_id) {
         case QSCRIPT_CALL:
            a->type_id = QS_ACTION_CALL;
            a->data_p = n->first_child->next->data;
            break;
         case QSCRIPT_INDEX:
            a->type_id = QS_ACTION_INDEX;
            a->data_p = n->first_child->next->data;
            break;
      }
   }
}
P_FUNC (qs_func_action_f)
   { if (node->data) qs_value_free (node->data); }

P_FUNC (qs_func_resource)
{
   p_node_t *n;
   qs_list_t *list = NULL;
   char *name = NULL, *ch;
   int flags = 0;

   /* get our flags, name, and code block. */
   for (n = node->first_child; n != NULL; n = n->next) {
      switch (n->type_id) {
         /* process flags. */
         case QSCRIPT_RTYPE:
            for (ch = n->first_child->contents; *ch != '\0'; ch++) {
               switch (*ch) {
                  case '@': flags |= QS_RSRC_AUTO_INSTANTIATE; break;
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
P_FUNC (qs_func_resource_f)
   { if (node->data) qs_resource_free (node->data); }

P_FUNC (qs_func_copy_contents)
{
   if (node->contents)
      free (node->contents);
   node->contents = strdup (node->first_child->contents);
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
   if (!p_language_new (static_qs_symbols))
      language_built = -1;
   else
      language_built = 1;
   return language_built;
}

p_node_t *qs_parse_file (qs_scheme_t *scheme, char *filename)
{
   /* attempt to open the file. */
   FILE *file;
   if ((file = fopen (filename, "r")) == NULL)
      return NULL;

   /* get file length. */
   fseek (file, 0, SEEK_END);
   size_t len = ftell (file);
   fseek (file, 0, SEEK_SET);

   /* get file contents. */
   char *contents = malloc (sizeof (char) * (len + 1));
   size_t len_read = fread (contents, sizeof (char), len, file);
   contents[len] = '\0';
   fclose (file);

   /* did we read the right number of bytes? */ 
   if (len_read != len) {
      free (contents);
      return NULL;
   }

   /* parse contents, free allocated memory, and return our nodes. */
   p_node_t *nodes = qs_parse_content (scheme, filename, contents);
   free (contents);
   return nodes;
}

p_node_t *qs_parse_content (qs_scheme_t *scheme, char *file, char *content)
{

   /* initialize our language if we haven't already. */
   if (qs_language_init () == -1)
      return NULL;

   /* parse the results.  return an error if it didn't work. */
   p_node_t *p = p_parse_content (file, content, static_qs_symbols,
                                  static_qs_symbols, scheme);
   if (p == NULL)
      return NULL;

   /* add to our list of root-level nodes. */
   scheme->node_count++;
   scheme->node_list = realloc (scheme->node_list, sizeof (p_node_t *) *
                                scheme->node_count);
   scheme->node_list[scheme->node_count - 1] = p;
   return p;
}
