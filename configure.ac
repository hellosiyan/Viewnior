#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([viewnior], [1.6], [contact@siyanpanayotov.com])
AM_INIT_AUTOMAKE
AM_SILENT_RULES([yes])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])


# Checks for programs.
AC_PROG_LIBTOOL
AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AM_PROG_CC_C_O
AC_HEADER_STDC
AC_PROG_CXX

VNR_WARNINGS=-Wall
AC_SUBST(VNR_WARNINGS)

# ******
# Translation
# ******
IT_PROG_INTLTOOL([0.35.0])

GETTEXT_PACKAGE=AC_PACKAGE_NAME
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [Gettext package.])
AM_GLIB_GNU_GETTEXT

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_PATH_PROG(GLIB_GENMARSHAL, glib-genmarshal)

# Checks for library functions.

# ******
# Libs
# ******


GTK_REQUIRED=2.20
GLIB_REQUIRED=2.32
SHARED_MIME_INFO_REQUIRED=0.20
GDKPIXBUF_REQUIRED=2.4.0
EXIV2_REQUIRED=0.21


VNR_MODULES="gtk+-2.0 >= $GTK_REQUIRED \
             glib-2.0 >= $GLIB_REQUIRED \
             gio-2.0 >= $GLIB_REQUIRED \
             shared-mime-info >= $SHARED_MIME_INFO_REQUIRED \
             gdk-pixbuf-2.0 >= $GDKPIXBUF_REQUIRED \
             exiv2 >= $EXIV2_REQUIRED"

# ******
# Wallpaper support
# ******
AC_MSG_CHECKING([whether Wallpaper change functionality is enabled])

AC_ARG_ENABLE([wallpaper],
    AS_HELP_STRING([--disable-wallpaper],[Disable Wallpaper change functionality]),
                   ,[enable_wallpaper=yes])

AM_CONDITIONAL(ENABLE_WALLPAPER, test "x$enable_wallpaper" = xyes)

if test x$enable_wallpaper = xyes ; then
    AC_DEFINE(HAVE_WALLPAPER, 1, [Define to 1 if wallpaper change functionality is included])
fi

AC_MSG_RESULT([$ENABLE_WALLPAPERlpaper])

# ****************
# CFLAGS/LIBS init
# ****************

PKG_CHECK_MODULES(VNR, $VNR_MODULES)
AC_SUBST(VNR_CFLAGS)
AC_SUBST(VNR_LIBS)

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 data/Makefile
                 data/viewnior.desktop.in
                 data/icons/Makefile
                 data/icons/16x16/Makefile
                 data/icons/16x16/apps/Makefile
                 data/icons/22x22/Makefile
                 data/icons/22x22/apps/Makefile
                 data/icons/24x24/Makefile
                 data/icons/24x24/apps/Makefile
                 data/icons/32x32/Makefile
                 data/icons/32x32/apps/Makefile
                 data/icons/48x48/Makefile
                 data/icons/48x48/apps/Makefile
                 data/icons/scalable/Makefile
                 data/icons/scalable/apps/Makefile
                 po/Makefile.in
                 man/Makefile])
AC_OUTPUT

echo "
    Viewnior ........... : Version $VERSION
    Location ........... : $prefix/bin
    CFLAGS ............. : $CFLAGS
    Wallpaper support .. : $enable_wallpaper
"

echo "
    To compile and install:
    1. Become root (using su)
    2. Type 'make install'
"
