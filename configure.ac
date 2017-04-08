# Define version here, before initialization.
m4_define(qs_version_major, 0)
m4_define(qs_version_minor, 1)
m4_define(qs_version_micro, 0)
m4_define(qs_version, [qs_version_major.qs_version_minor.qs_version_micro])

# Initialize.
AC_PREREQ([2.62])
AC_INIT(qscript, [qs_version])

# AC Configuration.
AC_CONFIG_SRCDIR([src/.dummy])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIRS([m4])

# Configure libtool.
AC_PROG_LIBTOOL

# Configure automake.
AM_INIT_AUTOMAKE([subdir-objects])

# Checks for programs.
AC_PROG_CC
AC_PROG_CC_C99
AC_PROG_RANLIB

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h stdio.h signal.h math.h stdarg.h])
AC_CHECK_HEADER_STDBOOL

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset strchr strdup strcasecmp])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
