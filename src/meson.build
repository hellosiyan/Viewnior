src_inc = include_directories('.')

viewnior_sources = [
    'main.c',
    'vnr-window.c',
    'vnr-window.h',
    'uni-cache.c',
    'uni-anim-view.c',
    'uni-nav.c',
    'uni-scroll-win.c',
    'uni-dragger.c',
    'uni-image-view.c',
    'vnr-message-area.c',
    'vnr-properties-dialog.c',
    'vnr-file.c',
    'uni-utils.c',
    'vnr-prefs.c',
    'vnr-crop.c',
    'vnr-tools.c',
    'uni-exiv2.cpp',
]

marshal = 'uni-marshal'

viewnior_sources += gnome.genmarshal(
  marshal,
  sources: marshal + '.list',
  prefix: 'uni_marshal',
  internal: true
)

nautilus = executable(
  'viewnior',
  viewnior_sources,
  include_directories: viewnior_include_dirs,
  dependencies: viewnior_deps,
  install: true
)
