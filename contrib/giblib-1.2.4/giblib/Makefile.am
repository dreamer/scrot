## Process this file with automake to produce Makefile.in
MAINTAINERCLEANFILES = Makefile.in

INCLUDES = \
-I. -I$(top_srcdir) \
-DPREFIX=\""$(prefix)"\" @IMLIB_CFLAGS@

lib_LTLIBRARIES = libgiblib.la

installed_headers_DATA = \
giblib.h gib_list.h gib_hash.h gib_btree.h gib_queue.h gib_stack.h gib_utils.h gib_style.h gib_imlib.h giblib_config.h gib_debug.h

installed_headersdir = $(prefix)/include/giblib

libgiblib_la_SOURCES = \
    gib_debug.h \
    giblib.h \
    gib_list.c gib_list.h \
    gib_hash.c gib_hash.h \
    gib_btree.c gib_btree.h \
    gib_style.c gib_style.h \
    gib_imlib.c gib_imlib.h \
    gib_stack.c gib_stack.h \
    gib_utils.c gib_utils.h \
	gib_queue.c gib_queue.h

libgiblib_la_LIBADD = @IMLIB_LIBS@
libgiblib_la_DEPENDENCIES = giblib_config.h
libgiblib_la_LDFLAGS = -version-info 1:6:0

