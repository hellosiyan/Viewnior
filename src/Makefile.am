bin_PROGRAMS = viewnior

QUIET_GEN = $(Q:@=@echo '  GEN   '$@;)

BUILT_SOURCES = 

uni_headers = 

viewnior_SOURCES =      \
    main.c              \
    $(BUILT_SOURCES)    \
    $(uni_headers)

viewnior_CFLAGS =       \
    $(VNR_CFLAGS)       \
    $(VNR_WARNINGS)     \
    -DPACKAGE_DATA_DIR=\""$(datadir)"\" \
    -DPACKAGE_LOCALE_DIR=\""$(datadir)/locale"\"

viewnior_LDADD = $(VNR_LIBS)

CLEANFILES = $(BUILT_SOURCES)
EXTRA_DIST = uni-marshal.list
