icon_sizes = [
  '16x16',
  '22x22',
  '24x24',
  '32x32',
  '48x48'
]

foreach icon_size: icon_sizes
  install_subdir(
    join_paths(icon_size, 'apps'),
    install_dir: join_paths(datadir, 'icons', 'hicolor', icon_size)
  )
endforeach

install_data(
  'scalable/apps/viewnior.svg',
  install_dir: join_paths(datadir, 'icons', 'hicolor', 'scalable', 'apps')
)
