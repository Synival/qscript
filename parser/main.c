#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "qscript/qs_config.h"
#include "qscript/qscript.h"

int main (int argc, char **argv)
{
   /* create a scheme for our scripts. */
   qs_scheme_t *scheme;
   scheme = qs_scheme_new ();

   /* load all files specified in the filename. */
   int file_count = 0, i;
   for (i = 1; i < argc; i++) {
      file_count++;
      qs_file_new (scheme, argv[i]);
   }

   /* if no files were matches, read from stdin. */
   if (file_count == 0)
      qs_file_new_fstream (scheme, "stdin", stdin);

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

