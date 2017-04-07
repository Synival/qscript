/* standard.h
 * ----------
 * standard functions included in all language schemes. */

#ifndef __QS_STANDARD_H
#define __QS_STANDARD_H

#include "defs.h"

#include "funcs.h"

/* global variables. */
extern qs_func_t qs_func_list_standard[];

/* standard functions. */
QS_FUNC (qsf_print);
QS_FUNC (qsf_math);
QS_FUNC (qsf_if);
QS_FUNC (qsf_compare);
QS_FUNC (qsf_return);
QS_FUNC (qsf_let);
QS_FUNC (qsf_type);
QS_FUNC (qsf_scope);
QS_FUNC (qsf_index);
QS_FUNC (qsf_while);
QS_FUNC (qsf_for);
QS_FUNC (qsf_lambda);
QS_FUNC (qsf_arg);
QS_FUNC (qsf_args);
QS_FUNC (qsf_variable);
QS_FUNC (qsf_property);
QS_FUNC (qsf_truth);
QS_FUNC (qsf_vars);
QS_FUNC (qsf_run);
QS_FUNC (qsf_boolean);
QS_FUNC (qsf_print_r);
QS_FUNC (qsf_incr);
QS_FUNC (qsf_inv);
QS_FUNC (qsf_print_r);
QS_FUNC (qsf_eval);
QS_FUNC (qsf_switch);
QS_FUNC (qsf_length);
QS_FUNC (qsf_print_v);
QS_FUNC (qsf_this);
QS_FUNC (qsf_cast);
QS_FUNC (qsf_id);
QS_FUNC (qsf_func_exists);
QS_FUNC (qsf_tokenize);
QS_FUNC (qsf_multi);
QS_FUNC (qsf_arg_list);
QS_FUNC (qsf_for_each);
QS_FUNC (qsf_new);

#endif
