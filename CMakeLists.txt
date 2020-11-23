cmake_minimum_required(VERSION "2.8")

set(VALID_BUILD_TYPES "Release" "Debug" "Static-Debug" "Static-Release")

project(opensea-common)
				
file (GLOB SOURCES src/common.c
				src/common_platform.c
				include/common.h 
				include/common_platform.h
				)

if (MSVC)
	file(GLOB EXTRA_SRC src/common_windows.c 
						include/common_windows.h)
elseif (UNIX)
	file(GLOB EXTRA_SRC src/common_nix.c 
						include/common_nix.h)
endif()

add_subdirectory(Make)

