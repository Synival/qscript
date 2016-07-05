#define _XOPEN_SOURCE 500
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ftw.h>

#include "qscript/qscript.h"

/* evil, evil global variables. */
qs_scheme_t *g_scheme = NULL;

int init_script_load (const char *fpath, const struct stat *sb, int tflag,
   struct FTW *ftwbuf)
{
   /* don't process directories. */
   if (tflag & FTW_D)
      return FTW_CONTINUE;

   /* don't process files that aren't .qs */
   const char *name = fpath + ftwbuf->base;
   const char *dot = strrchr (name, '.');
   if (dot == NULL || strcmp (dot, ".qs") != 0)
      return FTW_CONTINUE;

   /* process the fail.  bail if it didn't work. */
   char *mname = strdup (name);
   p_node_t *n = qs_parse_file (g_scheme, mname);
   free (mname);
   if (n == NULL)
      return FTW_STOP;

   /* we did it! keep going. */
   return FTW_CONTINUE;
}

int init_scripts (void)
{
   /* only initialize once. */
   static int init = 0;
   if (init)
      return -1;
   init = 1;

   /* create our scheme. */
   g_scheme = qs_scheme_new ();

   /* load our files. */
   int r = nftw ("assets", init_script_load, 20, FTW_CHDIR | FTW_ACTIONRETVAL);
   if (r == FTW_STOP)
      return -1;

   /* update our scheme objects and return the number of resources. */
   qs_scheme_update (g_scheme);
   return g_scheme->resource_count;
}

int main (void)
{
   /* initialize our scripts. */
   if (init_scripts () < 0)
      return 1;

   /* instantiate an object of type 'main'. */
   qs_object_new (qs_resource_get (g_scheme, "main"));
   qs_scheme_update (g_scheme);

   /* show us all our objects and all their properties. */
   qs_object_t *obj;
   qs_property_t *p;
   qs_value_t *v;
   char *type;
   for (obj = g_scheme->object_list_front; obj != NULL; obj = obj->next) {
      printf ("%s:\n", obj->name);
      for (p = obj->property_list_front; p != NULL; p = p->next) {
         v = qs_property_value (p);
         type = qs_value_type (v);
         printf ("   %-5s (%s)%*s = %-23s\n", p->name, type,
            6 - (int) strlen (type), "", v->val_s);
      }
      if (obj->property_list_front == NULL)
         printf ("   (no properties)\n");
      printf ("\n");
   }

   /* clean up memory. */
   qs_scheme_free (g_scheme);

   /* return success. */
   return 0;
}
