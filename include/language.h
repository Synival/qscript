/* language.h
 * ----------
 * processing of the qscript language into resources. */

#ifndef __QS_LANGUAGE_H
#define __QS_LANGUAGE_H

#include "defs.h"

P_FUNC (qs_func_resource);
P_FUNC (qs_func_resource_f);
P_FUNC (qs_func_list);
P_FUNC (qs_func_list_f);
P_FUNC (qs_func_outer_list);
P_FUNC (qs_func_outer_list_f);
P_FUNC (qs_func_value);
P_FUNC (qs_func_value_f);
P_FUNC (qs_func_qstring);
P_FUNC (qs_func_complex_string);
P_FUNC (qs_func_variable);
P_FUNC (qs_func_action);
P_FUNC (qs_func_action_f);

/* various kinds of processing functions. */
int qs_language_init (void);
p_node_t *qs_parse_file (qs_scheme_t *scheme, char *filename);
p_node_t *qs_parse_content (qs_scheme_t *scheme, char *file, char *content);

#endif
