SUBDIRS = icons

@INTLTOOL_DESKTOP_RULE@

# Desktop entries
DESKTOP_IN_FILES= viewnior.desktop.in.in

DESKTOP_FILES= $(DESKTOP_IN_FILES:.desktop.in.in=.desktop)

desktopdir = $(datadir)/applications

desktop_DATA = $(DESKTOP_FILES)


gladedir = $(datadir)/viewnior
glade_DATA = vnr-preferences-dialog.ui \
             vnr-crop-dialog.ui

APPDATA_FILES = viewnior.appdata.xml

PIXMAPS_FILES = \
	pixmaps/object-rotate-right.png \
	pixmaps/object-rotate-left.png \
	pixmaps/object-flip-horizontal.png \
	pixmaps/object-flip-vertical.png

pixmaps_DATA = $(PIXMAPS_FILES)
pixmapsdir = $(datadir)/viewnior/pixmaps

EXTRA_DIST = $(DESKTOP_IN_FILES) $(pixmaps_DATA) $(glade_DATA) $(APPDATA_FILES)

DISTCLEANFILES = $(DESKTOP_FILES) viewnior.desktop.in