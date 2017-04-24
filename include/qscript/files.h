/* files.h
 * -------
 * managing scripts loaded from files or other contexts. */

#ifndef __QS_FILES_H
#define __QS_FILES_H

#include "defs.h"

/* structure for each file loaded in a scheme. */
struct _qs_file_t {
   /* file info. */
   char *name;

   /* resources loaded from this file. */
   p_node_t *node;
   int resource_count;
   qs_resource_t *resource_list_front, *resource_list_back;

   /* links. */
   qs_id_t id;
   qs_scheme_t *scheme;
   qs_file_t *prev, *next;
};

/* file management functions. */
qs_file_t *qs_file_new_content (qs_scheme_t *scheme, char *name, char *content);
qs_file_t *qs_file_new (qs_scheme_t *scheme, char *filename);
int qs_file_new_directory (qs_scheme_t *scheme, char *directory, int recurse);
qs_file_t *qs_file_new_fstream (qs_scheme_t *scheme, char *name, FILE *file);
int qs_file_free (qs_file_t *file);
qs_file_t *qs_file_get (qs_scheme_t *scheme, char *name);

#endif
