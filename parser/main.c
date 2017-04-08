#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "qscript/qs_config.h"
#include "qscript/qscript.h"

int main (int argc, char **argv)
{
   char *filename;
   FILE *file;
   int file_close;
   if (argc >= 2) {
      filename = argv[1];
      if ((file = fopen (filename, "r")) == NULL) {
         char buf[256];
         snprintf (buf, sizeof (buf), "Couldn't open '%s' for reading",
            filename);
         perror (buf);
         exit (errno);
      }
      file_close = 1;
   }
   else {
      filename   = "stdin";
      file       = stdin;
      file_close = 0;
   }

   /* create a scheme for our scripts. */
   qs_scheme_t *scheme;
   scheme = qs_scheme_new ();

   /* get all of our content. */
   char *content = strdup ("");
   char buf[256];
   size_t len = 0, total = 1;

   while (fgets (buf, sizeof (buf), file)) {
      len = strlen (buf);
      total += len;

      char *old = content;
      if ((content = realloc (content, total)) == NULL) {
         perror ("Couldn't realloc() content");
         free (old);
         exit (errno);
      }
      strcat (content, buf);
   }
   if (file_close)
      fclose (file);

   /* read from our file. */
   errno = 0;
   int res = (qs_parse_content (scheme, filename, content) ? 1 : 0);
   free (content);
   if (!res) {
      if (errno != 0) {
         perror ("Unable to parse content");
         exit (errno);
      }
      else
         exit (1);
   }

   /* finish linking compiled scripts. */
   qs_scheme_update (scheme);

   /* instantiate main. */
   qs_resource_t *rsrc = qs_resource_get (scheme, ("main"));
   if (rsrc)
      qs_object_instantiate (rsrc, "main");
   qs_scheme_update (scheme);

   /* thorough, (hopefully) bug-free clean-up. */
   qs_scheme_free (scheme);

   /* return success. */
   return 0;
}

