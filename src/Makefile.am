lib_LTLIBRARIES = libcdcover.la

SUBDIRS = interface skin 

libdir = `pkg-config audacious --variable=visualization_plugin_dir`

CFLAGS = -O2 -Wall @AUDACIOUS_CFLAGS@ @CFLAGS@ -I$(top_builddir) -I$(top_srcdir) -I./skin \
         @GTK_CFLAGS@ @GDK_PIXBUF_CFLAGS@

LIBS = @AUDACIOUS_LIBS@ @GDK_PIXBUF_LIBS@
libcdcover_la_LIBADD = ./interface/libinterface.la
libcdcover_la_LDFLAGS = -module -avoid-version
libcdcover_la_SOURCES = configmgm.c  configmgm.h  debug.h  findcover.c  findcover.h  globals.h  main.c  mainwindow.c  mainwindow.h 

