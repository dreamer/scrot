## Process this file with automake to produce Makefile.in

SUBDIRS = giblib

docs_DATA = README AUTHORS ChangeLog TODO
docsdir = $(prefix)/doc/giblib

EXTRA_DIST= $(docs_DATA) giblib.pc.in

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = giblib.pc

bin_SCRIPTS = giblib-config
