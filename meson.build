project('L-System', 'cpp',
	default_options : ['cpp_std=c++20',
                     'warning_level=3',
                     'buildtype=debug',
                     'optimization=g'])

srcs = [
  'main.cpp', 'grammer.cpp', 'callbacks.cpp', 'shaders.cpp', 'graphics.cpp',
]

# get dependencies
depends = []
cpp = meson.get_compiler('cpp')

depends += cpp.find_library('GLEW')
depends += cpp.find_library('GLU')
depends += cpp.find_library('glfw')
depends += cpp.find_library('GL')

velox = subproject('VeloxEngine')
depends += velox.get_variable('velox_dep')
inc_dir = velox.get_variable('includes')

# copy shaders
configure_file(input  : 'lsystem.fs',
               output : 'lsystem.fs',
               copy   : true)
configure_file(input  : 'lsystem.vs',
               output : 'lsystem.vs',
               copy   : true)

executable('lsystem', [srcs],
	dependencies : depends,
	include_directories : inc_dir)
