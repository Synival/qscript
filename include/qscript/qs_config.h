/* qs_config.h
 * -----------
 * internal configuration files. */

#ifndef __QS_QS_CONFIG_H
#define __QS_QS_CONFIG_H

#ifdef HAVE_CONFIG_H
   #include "config.h"
#endif

/* check manually for strdup() - configure script makes mistakes. */
#if (defined __APPLE__) || \
    (defined _SVID_SOURCE) || \
    (defined _BSD_SOURCE) || \
    (_XOPEN_SOURCE >= 500) || \
    ((defined _XOPEN_SOURCE) && (defined _XOPEN_SOURCE_EXTENDED)) || \
    (_POSIX_C_SOURCE >= 200809L)
   #ifndef HAVE_STRDUP
      #define HAVE_STRDUP 1
      #define HAVE_STRCASECMP 1
   #endif
#else
   #undef HAVE_STRDUP
   #undef HAVE_STRCASECMP
   #define strdup     _qs_strdup
   #define strcasecmp _qs_strcasecmp
#endif

/* add strdup() if we have to. */
#ifndef HAVE_STRDUP
   char *_qs_strdup (const char *str);
   int _qs_strcasecmp (const char *s1, const char *s2);
#else
#endif

#endif
