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
   int file_close, i;

   /* create a scheme for our scripts. */
   qs_scheme_t *scheme;
   scheme = qs_scheme_new ();

   /* load all files specified in the filename.  if no arguments are provided,
    * read from stdin. */
   for (i = 1; i < argc || i == 1; i++) {
      if (i < argc) {
         filename = argv[i];
         if ((file = fopen (filename, "r")) == NULL) {
            char buf[256];
            snprintf (buf, sizeof (buf), "Couldn't open '%s' for reading",
               filename);
            perror (buf);
            continue;
         }
         file_close = 1;
      }
      else {
         filename   = "stdin";
         file       = stdin;
         file_close = 0;
      }
   
      /* read from our file. */
      errno = 0;
      int res = (qs_parse_fstream (scheme, filename, file) ? 1 : 0);
      if (!res && errno != 0)
         perror ("Unable to parse content");
      if (file_close)
         fclose (file);
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

