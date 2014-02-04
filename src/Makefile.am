bin_PROGRAMS = viewnior

QUIET_GEN = $(Q:@=@echo '  GEN   '$@;)

BUILT_SOURCES =         \
    uni-marshal.c       \
    uni-marshal.h

uni_headers =           \
    uni-cache.h         \
    uni-image-view.h    \
    uni-anim-view.h     \
    uni-scroll-win.h    \
    uni-dragger.h       \
    uni-nav.h           \
    vnr-message-area.h  \
    vnr-properties-dialog.h  \
    vnr-file.h          \
    uni-zoom.h          \
    uni-utils.h         \
    vnr-prefs.h         \
    vnr-crop.h          \
    vnr-tools.h         \
    uni-exiv2.hpp

viewnior_SOURCES =      \
    main.c              \
    vnr-window.c        \
    vnr-window.h        \
    uni-cache.c         \
    uni-anim-view.c     \
    uni-nav.c           \
    uni-scroll-win.c    \
    uni-dragger.c       \
    uni-image-view.c    \
    vnr-message-area.c  \
    vnr-properties-dialog.c  \
    vnr-file.c          \
    uni-utils.c         \
    vnr-prefs.c         \
    vnr-crop.c          \
    vnr-tools.c         \
    uni-exiv2.cpp       \
    $(BUILT_SOURCES)    \
    $(uni_headers)

viewnior_CFLAGS =       \
    $(VNR_CFLAGS)       \
    $(VNR_WARNINGS)     \
    -DPACKAGE_DATA_DIR=\""$(datadir)"\" \
    -DPACKAGE_LOCALE_DIR=\""$(datadir)/locale"\"

viewnior_CXXFLAGS = $(viewnior_CFLAGS)

viewnior_LDADD = $(LDADD) $(VNR_LIBS) -lm

uni-marshal.h: uni-marshal.list $(GLIB_GENMARSHAL)
	$(QUIET_GEN)$(GLIB_GENMARSHAL) $< --header --prefix=uni_marshal > $@
uni-marshal.c: uni-marshal.list $(GLIB_GENMARSHAL)
	$(QUIET_GEN)echo "#include \"uni-marshal.h\"" > $@ && \
	$(GLIB_GENMARSHAL) $< --body --prefix=uni_marshal >> $@

CLEANFILES = $(BUILT_SOURCES)
EXTRA_DIST = uni-marshal.list
