#include <errno.h>
#include <stdio.h>

#include "../include/qscript.h"

#define MY_FILE "test/test.qs"

int main (void)
{
   qs_scheme_t *scheme;

   /* create a scheme for our scripts. */
   scheme = qs_scheme_new ();

   /* read from our file. */
   if (!qs_parse_file (scheme, MY_FILE)) {
      printf ("Couldn't open '%s'.\n", MY_FILE);
      return errno;
   }

   /* finish linking compiled scripts. */
   qs_scheme_link (scheme);

   /* instantiate main. */
   qs_object_new (qs_resource_get (scheme, "main"));

   /* thorough, (hopefully) bug-free clean-up. */
   qs_scheme_free (scheme);

   /* return success. */
   return 0;
}
