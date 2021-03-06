dnl Process this file with autoconf to produce a configure script.

AC_INIT(gtksourcecompletion,0.9.0)
dnl Every other copy of the package version number gets its value from here
AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)

dnl create a config.h file (Automake will add -DHAVE_CONFIG_H)
AM_CONFIG_HEADER(config.h)

AC_SUBST(VERSION)

ISODATE=`date +%Y-%m-%d`
AC_SUBST(ISODATE)

AC_CANONICAL_HOST

# Dependencies
GTK_REQUIRED_VERSION=2.12.0

AC_SUBST(GTK_REQUIRED_VERSION)

dnl pkg-config configuration
PKG_CHECK_MODULES(DEP, gtk+-2.0 >= $GTK_REQUIRED_VERSION)
AC_SUBST(DEP_CFLAGS)
AC_SUBST(DEP_LIBS)

dnl Checks for programs.
AC_PROG_INSTALL
AC_PROG_CC
AM_PROG_LIBTOOL

dnl Checks for libraries.

############i18n support###############
AC_PROG_INTLTOOL([0.23])

GETTEXT_PACKAGE=gtksourcecompletion-2.0
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["$GETTEXT_PACKAGE"],["Descripcion"])

ALL_LINGUAS="es fr it"
AM_GLIB_GNU_GETTEXT

gtksourcecompletionlocaledir='${prefix}/${DATADIRNAME}/locale'
AC_SUBST(gtksourcecompletionlocaledir)
########################################


dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(unistd.h sys/param.h sys/time.h time.h sys/mkdev.h sys/sysmacros.h string.h memory.h fcntl.h dirent.h sys/ndir.h ndir.h alloca.h locale.h )

jm_CHECK_TYPE_STRUCT_UTIMBUF
AC_HEADER_MAJOR
AC_FUNC_ALLOCA
AC_STRUCT_TM
AC_STRUCT_ST_BLOCKS
AC_FUNC_CLOSEDIR_VOID
AC_CHECK_FUNCS(mkfifo)
AC_CHECK_FUNC(mknod)
# check for gtk-doc
GTK_DOC_CHECK(1.8)
dnl Checks for typedefs, structures, and compiler characteristics.

dnl Checks for library functions.
AC_PATH_PROG(GLIB_GENMARSHAL, glib-genmarshal)

have_devhelp=no
AC_ARG_WITH(devhelp,
            AC_HELP_STRING([--with-devhelp],
                           [Build with devhelp test case]),,
            with_devhelp=auto)
if test "x$with_devhelp" != "xno"
then
        PKG_CHECK_MODULES(DEVHELP, [
                libdevhelp-1.0 >= 0.22
                webkit-1.0
        ], have_devhelp=yes, have_devhelp=no)

        #Disable devhelp by the moment
        #if test "x$have_devhelp" = "xyes"
        if test "x$have_devhelp" = "xyes"
        then
                AC_DEFINE([HAVE_DEVHELP], 1, [Have devhelp])
        fi
fi

AM_CONDITIONAL(USE_DEVHELP, test "x$have_devhelp" = "xyes")
#Disable devhelp by the moment
#AM_CONDITIONAL(USE_DEVHELP, test "x$have_devhelpno" = "xyes")

AC_OUTPUT([
Makefile
gtksourcecompletion-2.0.pc
gtksourcecompletion.spec
gtksourcecompletion/Makefile
test/Makefile
pixmaps/Makefile
docs/Makefile
docs/reference/Makefile
po/Makefile.in
])
