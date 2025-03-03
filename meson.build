project('vu', 'c', version: '1.0', default_options: ['warning_level=2'])

sdl_dep = dependency('sdl2')

srcs = [
  'src/main.c',
  'src/ui.c'
]

executable('vu',
  srcs,
  dependencies: sdl_dep,
  link_args: '-lm',
  install: true,
  install_dir: '/usr/local/bin'
)
