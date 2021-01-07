cmake_minimum_required(VERSION "3.12")

list(APPEND CMAKE_CONFIGURATION_TYPES "Debug" "Release" "Static-Debug" "Static-Release")


#if (${CMAKE_GENERATOR_PLATFORM} STREQUAL "ARM")
if (CMAKE_GENERATOR_PLATFORM MATCHES "ARM")
	set(CMAKE_C_FLAGS_DEBUG "/Zi /JMC /Od /Od /EHsc /RTC1 /MDd")
	set(CMAKE_C_FLAGS_STATIC-DEBUG "/Zi /JMC /Od /Od /EHsc /RTC1 /MTd")
#elseif (${CMAKE_GENERATOR_PLATFORM} STREQUAL "ARM64")
elseif (CMAKE_GENERATOR_PLATFORM MATCHES "ARM64")
	set(CMAKE_C_FLAGS_DEBUG "/Zi /JMC /Od /Od /EHsc /RTC1 /MDd")
	set(CMAKE_C_FLAGS_STATIC-DEBUG "/Zi /JMC /Od /Od /EHsc /RTC1 /MTd")
else()
	set(CMAKE_C_FLAGS_DEBUG "/ZI /JMC /Od /Od /EHsc /RTC1 /MDd")
	set(CMAKE_C_FLAGS_STATIC-DEBUG "/ZI /JMC /Od /Od /EHsc /RTC1 /MTd")
endif()

set(CMAKE_C_FLAGS_RELEASE "/LTCG /GL /Gy /Oi /EHsc /O2 /Zi ")
set(CMAKE_C_FLAGS_STATIC-RELEASE "/LTCG /GL  /Gy /Zi /Oi /EHsc /MT")


#set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

project(opensea-common)
				
file (GLOB SOURCES src/common.c
				src/common_platform.c
				include/common.h 
				include/common_platform.h
				)

add_definitions(-D_UNICODE)

set(CMAKE_SYSTEM_VERSION "${10.0.16299.0}")
set(CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION "${10.0.16299.0}")
set(CMAKE_SUPPRESS_REGENERATION true)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if (MSVC)
	file(GLOB EXTRA_SRC src/common_windows.c 
						include/common_windows.h)
elseif (UNIX)
	file(GLOB EXTRA_SRC src/common_nix.c 
						include/common_nix.h)
endif()

add_subdirectory(Make)

