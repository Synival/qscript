/* hooks.h
 * -------
 * functions called on certain conditions. */

#ifndef __QS_HOOKS_H
#define __QS_HOOKS_H

#include "defs.h"

/* hooks attached to schemes. */
struct _qs_hook_t {
   /* hook data. */
   qs_hook_signal_t signal;
   qs_hook_func *func;

   /* link info. */
   qs_id_t id;
   qs_scheme_t *scheme;
   qs_hook_t *prev, *next;
};

/* functions. */
qs_hook_t *qs_hook_new (qs_scheme_t *scheme, qs_hook_signal_t signal,
   qs_hook_func *func);
int qs_hook_free (qs_hook_t *hook);
int qs_hook_call (qs_scheme_t *scheme, qs_hook_signal_t signal, void *data,
   size_t data_size);

#endif
