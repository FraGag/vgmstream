cmake_minimum_required(VERSION 2.6)

include(CheckLanguage)
include(CheckSymbolExists)

project(vgmstream
  LANGUAGES C)

# C++ is required for the Audacious plugin only.
# If a C++ compiler is not available, the Audacious plugin will be disabled automatically.
check_language(CXX)
if(CMAKE_CXX_COMPILER)
  enable_language(CXX)
endif(CMAKE_CXX_COMPILER)

# RC is used for resources on Windows.
# If an RC compiler is not available, resources will not be compiled into the binaries.
check_language(RC)
if(CMAKE_RC_COMPILER)
  enable_language(RC)
endif(CMAKE_RC_COMPILER)

if(MSVC)
  add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif(MSVC)

# On some platforms, the standard C math functions are implemented in a library that isn't linked by default, libm.
check_symbol_exists("sqrt" "math.h" HAVE_SQRT)
if(HAVE_SQRT)
  # The target platform doesn't require linking to an additional library.
  # Add a dummy library named `m` so we can just link to `m` on all platforms.
  add_library(m INTERFACE)
else(HAVE_SQRT)
  set(CMAKE_REQUIRED_LIBRARIES "m")
  check_symbol_exists("sqrt" "math.h" HAVE_SQRT_WITH_M)
  if(NOT HAVE_SQRT_WITH_M)
    message(FATAL_ERROR "Couldn't find how to link to function sqrt()")
  endif(NOT HAVE_SQRT_WITH_M)
endif(HAVE_SQRT)

# The `version_h` target generates `version.h` in the build directory.
# The `version` target centralizes the addition of the build directory to the include path;
# a target that wants to use `version.h` should link to the `version` target.
add_custom_target(version_h
  DEPENDS version.h.in
  BYPRODUCTS "${CMAKE_CURRENT_BINARY_DIR}/version.h"
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMAND
    "${CMAKE_COMMAND}"
    -D "CURRENT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}"
    -D "CURRENT_BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}"
    -P version.cmake)

add_library(version INTERFACE)
add_dependencies(version version_h)
target_compile_definitions(version INTERFACE CMAKE) # until we remove the prior build systems
target_include_directories(version INTERFACE "${CMAKE_CURRENT_BINARY_DIR}")

#add_subdirectory(audacious)
add_subdirectory(cli)
add_subdirectory(dependencies-cmake)
add_subdirectory(ext_libs)
add_subdirectory(src)

# Only add these subdirectories when targetting 32-bit Windows.
# The targets they provide are plugins for media players that only exist as 32-bit Windows executables.
if(WIN32 AND CMAKE_SIZEOF_VOID_P EQUAL 4)
  add_subdirectory(fb2k)
  add_subdirectory(winamp)
  #add_subdirectory(xmplay)
endif()
