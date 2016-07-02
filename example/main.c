#define _XOPEN_SOURCE 500
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ftw.h>

#include "qscript/qscript.h"

/* evil, evil global variables. */
qs_scheme_t *g_scheme = NULL;

/* function declarations. */
QS_FUNC (qsf_prompt);

/* our custom functinos. */
qs_func_t g_maze_funcs[] = {
   { "prompt", "", 0, qsf_prompt, 0 }
};

QS_FUNC (qsf_prompt)
{
   char buf[256], *res;
   int i, len;

   /* get some input. */
   printf ("> ");
   fflush (stdout);

   /* get our string.  if we hit Ctrl+D, break. */
   res = fgets (buf, sizeof (buf), stdin);
   if (!res) {
      printf ("\n");
      return QSV_UNDEFINED;
   }

   /* get rid of line feeds and invalid characters and such. */
   len = strlen (buf);
   for (i = 0; i < len; i++) {
      if (buf[i] == '\n' || buf[i] == '\r')
         buf[i] = '\0';
      else if (buf[i] < ' ' || buf[i] > '~')
         buf[i] = '?';
   }

   /* return it. */
   return QS_RETS (buf);
}

int clear_screen (void)
{
   printf ("\x1b[2J\x1b[0H");
   return 1;
}

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
   qs_scheme_funcs (g_scheme, g_maze_funcs);

   /* load our files. */
   int r = nftw ("assets", init_script_load, 20, FTW_CHDIR | FTW_ACTIONRETVAL);
   if (r == FTW_STOP)
      return -1;

   /* update our scheme objects and return the number of resources. */
   qs_scheme_update (g_scheme);
   return g_scheme->resource_count;
}

int wait_for_enter (void)
{
   getchar ();
   fflush (stdin);
   return 1;
}

int main (void)
{
   /* initialize our scripts. */
   if (init_scripts () < 0)
      return 1;

   /* we're running! */
   clear_screen ();

   /* instantiate an object of type 'main'. */
   qs_object_new (qs_resource_get (g_scheme, "main"));

   /* exit nicely. */
   printf ("\n(Game done. Press enter to end.)");
   fflush (stdout);
   wait_for_enter ();
   clear_screen ();

   /* clean up memory. */
   qs_scheme_free (g_scheme);

   /* return success. */
   return 0;
}
