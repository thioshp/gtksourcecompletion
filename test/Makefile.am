bin_PROGRAMS = completion-simple

INCLUDES=	\
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DICON_DIR=\""$(datadir)/gtksourcecompletion/pixmaps"\" \
	-I$(top_srcdir)/gtksourcecompletion	\
	`pkg-config --cflags gtksourceview-2.0` \
	`pkg-config --cflags gio-2.0` \
	$(PACKAGE_CFLAGS)

AM_CFLAGS = \
	-Wall	

noinst_PROGRAMS = completion-simple

gio_test_SOURCES = 		\
	gio-test.c

gio_test_LDFLAGS =  \
	`pkg-config --libs gtksourceview-2.0`	\
	`pkg-config --libs gio-2.0`

gio_test_LDADD = 	\
	$(top_builddir)/gtksourcecompletion/libgtksourcecompletion-2.0.la	\
	$(PACKAGE_LIBS)

completion_simple_SOURCES = 		\
	completion-simple.c		\
	gsc-provider-test.h		\
	gsc-provider-test.c		\
	gsc-provider-words.h		\
	gsc-provider-words.c

completion_simple_LDFLAGS =  \
	`pkg-config --libs gtksourceview-2.0`	\
	`pkg-config --libs gio-2.0`

completion_simple_LDADD = 	\
	$(top_builddir)/gtksourcecompletion/libgtksourcecompletion-2.0.la	\
	$(PACKAGE_LIBS)

completion_focus_SOURCES = 		\
	gsc-documentwords-provider.c	\
	gsc-documentwords-provider.h	\
	gsc-provider-file.c		\
	gsc-provider-file.h		\
	completion-focus.c		\
	gsc-provider-test.h		\
	gsc-provider-test.c

completion_focus_LDFLAGS =  \
	`pkg-config --libs gtksourceview-2.0`	\
	`pkg-config --libs gio-2.0`

completion_focus_LDADD = 	\
	$(top_builddir)/gtksourcecompletion/libgtksourcecompletion-2.0.la	\
	$(PACKAGE_LIBS)


completion_custom_widgets_SOURCES = 	\
	gsc-provider-test.h		\
	gsc-provider-test.c		\
	completion-custom-widgets.c

completion_custom_widgets_LDFLAGS =  \
	`pkg-config --libs gtksourceview-2.0`	\
	`pkg-config --libs gio-2.0`

completion_custom_widgets_LDADD = 	\
	$(top_builddir)/gtksourcecompletion/libgtksourcecompletion-2.0.la	\
	$(PACKAGE_LIBS)

if USE_DEVHELP
completion_simple_SOURCES += gsc-provider-devhelp.h gsc-provider-devhelp.c
completion_simple_LDADD += $(DEVHELP_LIBS)
INCLUDES += $(DEVHELP_CFLAGS)
endif
