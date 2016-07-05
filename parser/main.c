#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../include/qscript.h"

int main (void)
{
   qs_scheme_t *scheme;

   /* create a scheme for our scripts. */
   scheme = qs_scheme_new ();

   /* get all of our content. */
   char *content = strdup ("");
   char buf[256];
   size_t len = 0, total = 1;

   while (fgets (buf, sizeof (buf), stdin)) {
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

   /* read from our file. */
   errno = 0;
   int res = (qs_parse_content (scheme, "stdin", content) ? 1 : 0);
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
      qs_object_instantiate (rsrc);

   /* thorough, (hopefully) bug-free clean-up. */
   qs_scheme_free (scheme);

   /* return success. */
   return 0;
}

