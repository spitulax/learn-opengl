project(
  'learn-opengl',
  'c', 'cpp',
  version : run_command('cat', meson.project_source_root() / 'VERSION', check: true).stdout().strip(),
  license: 'MIT',
  default_options : [
    'c_std=c23',
    'cpp_std=c++23',
    'warning_level=3',
  ],
)

cc = meson.get_compiler('c')
cxx = meson.get_compiler('cpp')

if get_option('buildtype').startswith('debug')
  add_project_arguments(['-Og', '-DDEBUG'], language: ['c', 'cpp'])
endif

add_project_arguments('-DPROG_VERSION="' + meson.project_version() + '"', language: ['c', 'cpp'])
add_project_arguments(['-Wconversion', '-Wsign-conversion', '-Wpedantic'], language: ['c', 'cpp'])
add_project_arguments(['-Wshadow', '-Wnon-virtual-dtor'], language: 'cpp')

subdir('src')

include = [
  include_directories('src'),
]

dependencies = [
  dependency('glfw3'),
  dependency('glm'),
  cc.find_library('m'),
]

subdir('resources')
subdir('deps')

executable(
  'learn-opengl',
  src,
  include_directories : include,
  dependencies : dependencies,
  link_with : [glad],
  install : true,
)
