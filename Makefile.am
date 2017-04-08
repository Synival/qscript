ACLOCAL_AMFLAGS=-I m4

lib_LTLIBRARIES = libqscript.la
libqscript_la_LDFLAGS = \
   -release 0.1.0 -lm
libqscript_la_CFLAGS = \
   -I$(top_srcdir)/include -Wall -std=c99 -D__TARGET_QSCRIPT__
libqscript_la_SOURCES = \
   src/actions.c \
   src/execute.c \
   src/funcs.c \
   src/ids.c \
   src/language.c \
   src/lists.c \
   src/objects.c \
   src/parser.c \
   src/print.c \
   src/properties.c \
   src/qs_config.c \
   src/resources.c \
   src/rlinks.c \
   src/schemes.c \
   src/stacks.c \
   src/standard.c \
   src/values.c \
   src/variables.c \
   mpc/mpc.c

otherincludedir = \
   $(includedir)/qscript
otherinclude_HEADERS = \
   include/qscript/qscript.h \
   include/qscript/defs.h \
   include/qscript/actions.h \
   include/qscript/execute.h \
   include/qscript/funcs.h \
   include/qscript/ids.h \
   include/qscript/language.h \
   include/qscript/link.h \
   include/qscript/lists.h \
   include/qscript/objects.h \
   include/qscript/parser_types.h \
   include/qscript/print.h \
   include/qscript/properties.h \
   include/qscript/resources.h \
   include/qscript/rlinks.h \
   include/qscript/schemes.h \
   include/qscript/stacks.h \
   include/qscript/standard.h \
   include/qscript/values.h \
   include/qscript/variables.h

bin_PROGRAMS = qscript
qscript_SOURCES = parser/main.c
qscript_CFLAGS  = -I$(top_srcdir)/include -Wall -std=c99
qscript_LDFLAGS = -L../lib -lqscript
