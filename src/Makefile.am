bin_PROGRAMS = viewnior

BUILT_SOURCES =			\
	uni-marshal.c		\
	uni-marshal.h

uni_headers =           \
	mouse_handler.h     \
	uni-cache.h         \
	uni-image-view.h	\
	uni-anim-view.h		\
	uni-scroll-win.h	\
	uni-dragger.h		\
	uni-nav.h           \
	vnr-message-area.h	\
	vnr-file.h          \
	uni-zoom.h          \
	uni-utils.h         \
	vnr-tools.h

viewnior_SOURCES =		\
	main.c              \
	vnr-window.c		\
	vnr-window.h		\
	uni-cache.c         \
	uni-anim-view.c		\
	uni-nav.c           \
	uni-scroll-win.c	\
	uni-dragger.c		\
	uni-image-view.c	\
	vnr-message-area.c	\
	mouse_handler.c		\
	vnr-file.c          \
	uni-utils.c         \
	vnr-tools.c         \
	$(BUILT_SOURCES)    \
	$(uni_headers)

viewnior_CFLAGS = 		\
	$(VNR_CFLAGS) 		\
	$(VNR_WARNINGS)		\
	-DPACKAGE_LOCALE_DIR=\""$(datadir)/locale"\"

viewnior_LDADD = $(VNR_LIBS)

uni-marshal.h: uni-marshal.list $(GLIB_GENMARSHAL)
	$(GLIB_GENMARSHAL) $< --header --prefix=uni_marshal > $@
uni-marshal.c: uni-marshal.list $(GLIB_GENMARSHAL)
	echo "#include \"uni-marshal.h\"" > $@ && \
	$(GLIB_GENMARSHAL) $< --body --prefix=uni_marshal >> $@

CLEANFILES = $(BUILT_SOURCES)
EXTRA_DIST = uni-marshal.list
