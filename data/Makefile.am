SUBDIRS = icons

@INTLTOOL_DESKTOP_RULE@

# Desktop entries
DESKTOP_IN_FILES= viewnior.desktop.in.in

DESKTOP_FILES= $(DESKTOP_IN_FILES:.desktop.in.in=.desktop)

desktopdir = $(datadir)/applications

desktop_DATA = $(DESKTOP_FILES)

uidir = $(pkgdatadir)
UI_FILES =          \
    viewnior-ui.xml

ui_DATA = $(UI_FILES)


EXTRA_DIST =            \
    $(DESKTOP_IN_FILES) \
    $(UI_FILES)

CLEANFILES = viewnior.desktop.in viewnior.desktop
