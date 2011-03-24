noinst_LTLIBRARIES = libinterface.la

CFLAGS = -O2 -Wall @AUDACIOUS_CFLAGS@ @CFLAGS@ -I$(top_builddir) -I$(top_srcdir) -I../ \
         @GTK_CFLAGS@ @GDK_PIXBUF_CFLAGS@

noinst_HEADERS = interface.h callbacks.h cdcover.glade

libinterface_la_SOURCES = callbacks.c  callbacks.h  callbacks_export.h  interface.c  interface.h  support.c  support.h

