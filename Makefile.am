SUBDIRS = gtksourcecompletion test pixmaps docs po 

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = gtksourcecompletion-2.0.pc


DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc


EXTRA_DIST= \
	intltool-extract.in	\
	intltool-merge.in	\
	intltool-update.in	\
	gtksourcecompletion-2.0.pc.in

DISTCLEANFILES = \
	intltool-extract \
	intltool-merge \
	intltool-update

#  if you write a self-test script named `chk', uncomment the
#  following and add `chk' to the EXTRA_DIST list
#TESTS=chk

#  build and install the .info pages
#info_TEXINFOS = gtksourcecompletion.texinfo
#gtksourcecompletion_TEXINFOS = gpl.texinfo

#  install the man pages
#man_MANS=gtksourcecompletion.1
