resource_files = []

subdir('shaders')

bundled_resources = custom_target(
  'bundled-resources',
  input: resource_files,
  output: 'resources.c',
  command: ['python3', '@CURRENT_SOURCE_DIR@/bundle.py', '@OUTPUT0@', '@INPUT@'],
)

src += bundled_resources
