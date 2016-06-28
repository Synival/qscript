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

#if 0
   p_node_print (scheme->node_list[0]);
   qs_print_resource (scheme->resource_list_front);
#endif

   /* finish linking compiled scripts. */
   qs_scheme_link (scheme);

   /* thorough, (hopefully) bug-free clean-up. */
   qs_scheme_free (scheme);

   /* return success. */
   return 0;
}
