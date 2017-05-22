/* files.c
 * -------
 * managing scripts loaded from files or other contexts. */

#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "qscript/language.h"
#include "qscript/link.h"
#include "qscript/parser.h"
#include "qscript/resources.h"
#include "qscript/schemes.h"

#include "qscript/files.h"

qs_file_t *qs_file_new_content (qs_scheme_t *scheme, char *name, char *content)
{
   /* initialize our language if we haven't already. */
   if (qs_language_init () == -1)
      return NULL;

   /* if there are files with this name already, deallocate them. */
   qs_file_t *old;
   while ((old = qs_file_get (scheme, name)) != NULL)
      qs_file_free (old);

   qs_file_t *new = calloc (1, sizeof (qs_file_t));
   new->scheme = scheme;
   new->name = strdup (name);

   /* parse the results.  bail out here if it didn't work. */
   p_symbol_t *symbols = qs_language_symbols ();
   new->node = p_parse_content (name, content, symbols, symbols, new);
   if (new->node == NULL) {
      free (new->name);
      free (new);
      return NULL;
   }

   /* link to our scheme and return our new file. */
   QS_LINK_BACK (scheme, new, file);
   return new;
}

qs_file_t *qs_file_new (qs_scheme_t *scheme, char *filename)
{
   /* attempt to open the file. */
   int fd = -1;
   FILE *file = NULL;

#define PF_BAIL_IF(cond, ...) \
   do { \
      errno = 0; \
      if (cond) { \
         p_error (NULL, __VA_ARGS__); \
         errno = 0; \
         if (file) \
            fclose (file); \
         else if (fd < 0) { \
            close (fd); \
         } \
         return NULL; \
      } \
   } while (0)

   PF_BAIL_IF ((fd = open (filename, O_RDONLY)) == -1,
      "Couldn't open file '%s' for reading: %s\n", filename, strerror (errno));
   struct stat sbuf;
   fstat (fd, &sbuf);
   PF_BAIL_IF (!S_ISREG (sbuf.st_mode),
      "File '%s' is not a regular file.\n", filename);
   PF_BAIL_IF ((file = fdopen (fd, "r")) == NULL,
      "Couldn't convert fd -> file for '%s': %s\n", filename, strerror (errno));

   /* get file length. */
   PF_BAIL_IF (fseek (file, 0, SEEK_END) != 0,
      "Couldn't seek to end of file '%s': %s\n", filename, strerror (errno));
   long len = ftell (file);
   PF_BAIL_IF (len == -1l || len == LONG_MAX,
      "Couldn't get content length for '%s': %s\n", filename, strerror (errno));
   fseek (file, 0, SEEK_SET);

   /* get file content. */
   char *content = malloc (sizeof (char) * (len + 1));
   size_t len_read = fread (content, sizeof (char), len, file);
   PF_BAIL_IF (ferror (file) != 0,
      "Error reading from '%s'.\n", filename);
   content[len] = '\0';
   fclose (file);

   /* did we read the right number of bytes? */ 
   if (len_read != len) {
      free (content);
      return NULL;
   }

   /* parse content, free allocated memory, and return our nodes. */
   qs_file_t *rval = qs_file_new_content (scheme, filename, content);
   free (content);
   return rval;
}

int qs_file_new_directory (qs_scheme_t *scheme, char *directory, int recurse)
{
   int count = 0;
   DIR *d;
   struct dirent *dir;

   /* attempt to open the directory. */
   if ((d = opendir (directory)) == NULL) {
      p_error (NULL, "Couldn't open directory '%s': %s\n", directory,
         strerror (errno));
      return -1;
   }

   /* look at children. */
   struct stat stat_buf;
   char fbuf[1024];
   while ((dir = readdir (d)) != NULL) {
      if (dir->d_name[0] == '.')
         continue;
      snprintf (fbuf, sizeof (fbuf), "%s/%s", directory, dir->d_name);
      if (stat (fbuf, &stat_buf) != 0)
         continue;
      if (S_ISDIR (stat_buf.st_mode)) {
         if (recurse)
            count += qs_file_new_directory (scheme, fbuf, recurse);
         continue;
      }
      if (S_ISREG (stat_buf.st_mode)) {
         char *ext = strrchr (dir->d_name, '.');
         if (ext != NULL && strcasecmp (ext, ".qs") == 0) {
            if (qs_file_new (scheme, fbuf))
               count++;
         }
         continue;
      }
   }

   /* return the number of directories processed. */
   closedir (d);
   return count;
}

qs_file_t *qs_file_new_fstream (qs_scheme_t *scheme, char *name, FILE *file)
{
   /* get all of our content. */
   char *content = strdup ("");
   size_t len = 0, total = 1;
   char buf[256];
   while (fgets (buf, sizeof (buf), file)) {
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

   /* load the content. */
   qs_file_t *rval = qs_file_new_content (scheme, name, content);
   free (content);
   return rval;
}

int qs_file_free (qs_file_t *file)
{
   /* free associated resources. */
   while (file->resource_list_front)
      qs_resource_free (file->resource_list_front);

   /* clear nodes allocated. */
   if (file->node)
      p_node_free (file->node);

   /* deallocate personal data. */
   if (file->name)
      free (file->name);

   /* unlink from scheme. */
   QS_UNLINK (file->scheme, file, file);

   /* free structure itself and return success. */
   free (file);
   return 1;
}

qs_file_t *qs_file_get (qs_scheme_t *scheme, char *name)
{
   qs_file_t *f;
   for (f = scheme->file_list_front; f != NULL; f = f->next)
      if (strcmp (f->name, name) == 0)
         return f;
   return NULL;
}
