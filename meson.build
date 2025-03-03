project('vu', 'c', version: '1.0', default_options: ['warning_level=2'])

cc = meson.get_compiler('c')

sdl_dep = dependency('sdl2', required: true)
tif_dep = cc.find_library('tiff', required: true)
gtf_dep = cc.find_library('geotiff', required: true)
deps = [sdl_dep, tif_dep, gtf_dep]

srcs = [
  'src/main.c',
  'src/ui.c',
  'src/io.c'
]

executable('vu',
  srcs,
  dependencies: deps,
  link_args: '-lm',
  install: true,
  install_dir: '/usr/local/bin'
)
