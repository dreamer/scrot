dnl Process this file with autoconf to produce a configure script.

AC_INIT
AC_CONFIG_SRCDIR([giblib/gib_list.c])
AM_CONFIG_HEADER(giblib/giblib_config.h)
AM_INIT_AUTOMAKE(giblib, 1.2.4, no-define)

AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AM_PROG_LIBTOOL
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AM_SANITY_CHECK

AM_MAINTAINER_MODE
AM_WITH_DMALLOC

AC_PATH_GENERIC(imlib2, 1.0.0, [
  AC_SUBST(IMLIB_LIBS)
  AC_SUBST(IMLIB_CFLAGS) ],
  AC_MSG_ERROR(Cannot find imlib2: Is imlib2-config in the path?) )

dnl the above doesn't work for some reason :/
IMLIB_LIBS=`imlib2-config --libs`
IMLIB_CFLAGS=`imlib2-config --cflags`
AC_SUBST(IMLIB_LIBS)
AC_SUBST(IMLIB_CFLAGS)

LIBS="$LIBS -lm"
CFLAGS="$CFLAGS -Wall"


AC_CONFIG_FILES([
Makefile
giblib-config
giblib.pc
giblib/Makefile
])
AC_CONFIG_COMMANDS([default],[[
chmod +x giblib-config
]],[[]])
AC_OUTPUT

echo "
$PACKAGE $VERSION
Configuration:
--------------

Imlib2 libs:	$IMLIB_LIBS
Imlib2 cflags:  $IMLIB_CFLAGS
CFLAGS:			$CFLAGS
"
