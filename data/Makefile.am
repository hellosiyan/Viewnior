SUBDIRS = icons

@INTLTOOL_DESKTOP_RULE@

# Desktop entries
DESKTOP_IN_FILES= viewnior.desktop.in.in

DESKTOP_FILES= $(DESKTOP_IN_FILES:.desktop.in.in=.desktop)

desktopdir = $(datadir)/applications

desktop_DATA = $(DESKTOP_FILES)

uidir = $(pkgdatadir)

if ENABLE_WALLPAPER
UI_FILES =          \
    viewnior-ui.xml
else
UI_FILES =          \
    viewnior-ui-nowallpaper.xml
endif



ui_DATA = $(UI_FILES)


EXTRA_DIST =            \
    $(DESKTOP_IN_FILES) \
    viewnior-ui-nowallpaper.xml \
    viewnior-ui.xml

CLEANFILES = viewnior.desktop.in viewnior.desktop
