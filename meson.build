project('opensea-common', 'c', license: 'MPL-2.0', version: '1.18.18', default_options : ['warning_level=2'])

c = meson.get_compiler('c')

#TODO: Add -Wcast-align=strict and fix these issues to help ensure better portability
#NOTE: -Wsign-conversion can be useful while debugging, but there are numerous places this shows up
#      and it is not useful, so only add it while debugging.
warning_flags = [
#	'-Wcast-align=strict',
#	'-Wsign-conversion',
	'-Wshadow=compatible-local',
	'-Wvla',
	'-Wfloat-equal',
	'-Wnull-dereference',
	'-Wunused-const-variable',
	'-Wduplicated-cond',
	'-Wjump-misses-init',
	'-Wstringop-overflow',
	'-Wlogical-op',
	'-Wshift-overflow=2',
	'-Wdouble-promotion',
	'-Wformat-security'
]

add_project_arguments(c.get_supported_arguments(warning_flags), language : 'c')

if get_option('debug')
  add_project_arguments('-D_DEBUG', language : 'c')
endif

global_cpp_args = []

src_files = ['src/common.c', 'src/common_platform.c']

deps = []

# Opensea common defines the nvme header globally and opensea-transport, opensea-operations and openseachest itself include this, so only need to declare it here
if target_machine.system() == 'linux'
  src_files += ['src/common_nix.c']
  if c.check_header('linux/nvme_ioctl.h')
	global_cpp_args += ['-DSEA_NVME_IOCTL_H']
  elif c.check_header('linux/nvme.h')
	global_cpp_args += ['-DSEA_NVME_H']
  elif c.check_header('uapi/nvme.h')
	global_cpp_args += ['-DSEA_UAPI_NVME_H']
  else
	global_cpp_args += ['-DDISABLE_NVME_PASSTHROUGH']
  endif
elif target_machine.system() == 'freebsd'
  src_files += ['src/common_nix.c']
  if not c.check_header('dev/nvme/nvme.h')
	  global_cpp_args += ['-DDISABLE_NVME_PASSTHROUGH']
  endif
elif target_machine.system() == 'sunos'
  src_files += ['src/common_nix.c']
  global_cpp_args += ['-DDISABLE_NVME_PASSTHROUGH']
elif target_machine.system() == 'windows'
  src_files += ['src/common_windows.c']
  if c.get_define('__MINGW32__') != ''
	  global_cpp_args += ['-DDISABLE_NVME_PASSTHROUGH', '-DSTRSAFE_NO_DEPRECATE']
	  win32_version = c.find_library('version')
	  advapi32 = c.find_library('advapi32')
	  deps += [win32_version, advapi32]
  endif
  global_cpp_args += ['-D_CRT_NONSTDC_NO_DEPRECATE', '-D_CRT_SECURE_NO_WARNINGS']
  global_cpp_args += ['-DENABLE_CSMI']
endif # TODO UEFI and vmware

m_dep = c.find_library('m', required : false)

incdir = include_directories('include')

opensea_common_lib = static_library('opensea-common', src_files, c_args : global_cpp_args, dependencies : [m_dep, deps], include_directories : incdir)
opensea_common_dep = declare_dependency(link_with : opensea_common_lib, compile_args : global_cpp_args, include_directories : incdir)
