#define _XOPEN_SOURCE 500
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "qscript/qscript.h"

int main (void)
{
   /* create our scheme, load its resources, and update changes. */
   qs_scheme_t *scheme = qs_scheme_new ();
   qs_file_new_directory (scheme, ".", 1);
   qs_scheme_update (scheme);

   /* instantiate our player and add rlinks to its resource chain. */
   qs_object_t *obj = qs_object_new (scheme, "player");
   qs_rlink_inject (obj, qs_resource_get (scheme, "player_base"),    0);
   qs_rlink_inject (obj, qs_resource_get (scheme, "class_wizard"),   10);
   qs_rlink_inject (obj, qs_resource_get (scheme, "purple_robe_on"), 20);
   qs_rlink_inject (obj, qs_resource_get (scheme, "magic_wand_on"),  20);
   qs_rlink_inject (obj, qs_resource_get (scheme, "buff_invisible"), 30);
   qs_rlink_inject (obj, qs_resource_get (scheme, "too_hot"),        40);

   /* update changes again so 'obj' is run. */
   qs_scheme_update (scheme);

   /* show us all our objects and all their properties. */
   qs_object_t *o;
   qs_property_t *p;
   qs_value_t *v;
   char *type, json_buf[65536];
   for (o = scheme->object_list_front; o != NULL; o = o->next) {
      /* vanilla. */
      printf ("\e[37;1m%s (%d) using qs_print_value():\e[0m\n",
         o->name, o->id);
      for (p = o->property_list_front; p != NULL; p = p->next) {
         v = qs_property_value (p);
         type = qs_value_type (v);
         printf ("   %-9s (%s)%*s = ", p->name, type,
            7 - (int) strlen (type), "");
         qs_print_value (v, 0);
         printf ("\n");
      }
      if (o->property_list_front == NULL)
         printf ("   (no properties)\n");
      printf ("\n");

      /* JSON! */
      printf ("\e[37;1m%s (%d) using qs_print_object_json():\e[0m\n%s\n\n",
         o->name, o->id,
         qs_print_object_json (o, json_buf, sizeof (json_buf)));
   }

   /* clean-up time!  free all objects, resources, and the scheme itself. */
   qs_scheme_free (scheme);

   /* return success. */
   return 0;
}
