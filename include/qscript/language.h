/* language.h
 * ----------
 * processing of the qscript language into resources. */

#ifndef __QS_LANGUAGE_H
#define __QS_LANGUAGE_H

#include <stdio.h>

#include "defs.h"
#include "parser.h"

P_FUNC (qs_language_resource);
P_FUNC (qs_language_resource_f);
P_FUNC (qs_language_list);
P_FUNC (qs_language_list_f);
P_FUNC (qs_language_outer_list);
P_FUNC (qs_language_outer_list_f);
P_FUNC (qs_language_value);
P_FUNC (qs_language_value_f);
P_FUNC (qs_language_qstring);
P_FUNC (qs_language_complex_string);
P_FUNC (qs_language_copy_contents);
P_FUNC (qs_language_action);
P_FUNC (qs_language_action_f);

/* various kinds of processing functions. */
int qs_language_init (void);

/* evil global variables. */
extern p_symbol_t global_qs_symbols[];

#endif
