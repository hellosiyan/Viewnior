project('viewnior', 'c', 'cpp',
  version: '1.8',
  license: 'GPL3+',
  meson_version: '>= 0.43.0'
)

add_global_arguments('-Wno-deprecated-declarations', language: 'c')

# Directory variables
datadir = get_option('datadir')
#

# Compiler variables
cc = meson.get_compiler('c')
#

# Imports
gnome = import('gnome')
i18n = import('i18n')
#

# Dependencies
glib_ver = '>= 2.32'

viewnior_deps = [
  dependency('gtk+-2.0', version: '>= 2.20'),
  dependency('glib-2.0', version: glib_ver),
  dependency('gio-2.0', version: glib_ver),
  dependency('shared-mime-info', version: '>= 0.20'),
  dependency('gdk-pixbuf-2.0', version: '>= 0.21'),
  dependency('exiv2', version: '>= 0.21'),
]
#

# Configuration
conf = configuration_data()

conf.set_quoted('VERSION', meson.project_version())
conf.set_quoted('PACKAGE', 'viewnior')
conf.set_quoted('PACKAGE_VERSION', meson.project_version())
conf.set_quoted('PACKAGE_STRING', 'viewnior ' + meson.project_version())
conf.set_quoted('GETTEXT_PACKAGE', 'viewnior')
conf.set_quoted('PACKAGE_DATA_DIR', join_paths(get_option('prefix'), get_option('datadir')))
conf.set_quoted('PACKAGE_LOCALE_DIR', join_paths(get_option('prefix'), get_option('datadir'), 'locale'))

configure_file(output: 'config.h', configuration: conf)
#

viewnior_include_dirs = include_directories('.')

subdir('po')
subdir('data')
subdir('man')
subdir('src')

meson.add_install_script('meson_post_install.py')
