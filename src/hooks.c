/* hooks.c
 * -------
 * functions called on certain conditions. */

#include <stdlib.h>

#include "qscript/link.h"
#include "qscript/schemes.h"

#include "qscript/hooks.h"

qs_hook_t *qs_hook_new (qs_scheme_t *scheme, qs_hook_signal_t signal,
   qs_hook_func *func)
{
   qs_hook_t *new;

   /* allocate our new hook. */
   new = calloc (1, sizeof (qs_hook_t));
   new->scheme = scheme;
   new->signal = signal;
   new->func   = func;

   /* link to hook list and assign id. */
   QS_LINK_BACK (scheme, new, hook);

   /* return our new hook. */
   return new;
}

int qs_hook_free (qs_hook_t *hook)
{ 
   /* unlink hook from our scheme and free it. */
   QS_UNLINK (hook->scheme, hook, hook);
   free (hook);

   /* return success. */
   return 1;
}

int qs_hook_call (qs_scheme_t *scheme, qs_hook_signal_t signal, void *data,
   size_t data_size)
{
   qs_hook_t *h;
   int count = 0;
   for (h = scheme->hook_list_front; h != NULL; h = h->next)
      if (h->signal & signal)
         if (h->func (scheme, h, signal, data, data_size))
            count++;
   return count;
}
