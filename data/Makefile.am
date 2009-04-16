SUBDIRS = icons

@INTLTOOL_DESKTOP_RULE@

# Desktop entries
DESKTOP_IN_FILES= viewnior.desktop.in.in

DESKTOP_FILES= $(DESKTOP_IN_FILES:.desktop.in.in=.desktop)

desktopdir = $(datadir)/applications

desktop_DATA = $(DESKTOP_FILES)


EXTRA_DIST = $(DESKTOP_IN_FILES)

CLEANFILES = viewnior.desktop.in viewnior.desktop
