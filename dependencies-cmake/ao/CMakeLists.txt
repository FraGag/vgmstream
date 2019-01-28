cmake_minimum_required(VERSION 3.0)

set(VERSION "1.2.2")

# libao doesn't compile as-is in Visual C++.
# When compiling with Visual C++, we'll download a patched version instead.
# We'll download the patched copy in a different directory from the unpatched version
# so that the same source tree can be used for builds using other compilers
# and those builds will be able to use the original sources, not the patched ones.
if(MSVC)
  set(TAR_SOURCE_URL "https://github.com/FraGag/libao/archive/${VERSION}-msvc.tar.gz")
  set(TAR_FILE "${CMAKE_CURRENT_SOURCE_DIR}/libao-${VERSION}-msvc.tar.gz")
  set(PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/libao-${VERSION}-msvc")
else()
  set(TAR_SOURCE_URL "https://github.com/xiph/libao/archive/${VERSION}.tar.gz")
  set(TAR_FILE "${CMAKE_CURRENT_SOURCE_DIR}/libao-${VERSION}.tar.gz")
  set(PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/libao-${VERSION}")
endif()

if(NOT IS_DIRECTORY "${PREFIX}")
  if(NOT EXISTS "${TAR_FILE}")
    message(STATUS "Downloading libao...")
    file(DOWNLOAD "${TAR_SOURCE_URL}" "${TAR_FILE}")
  endif()

  if(EXISTS "${TAR_FILE}")
    file(MAKE_DIRECTORY "${TMP_DIR}")
    execute_process(
      COMMAND "${CMAKE_COMMAND}" -E tar xzf "${TAR_FILE}"
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
  endif()
endif()

include(CheckCSourceCompiles)
include(CheckTypeSize)

check_type_size(short SIZEOF_SHORT BUILTIN_TYPES_ONLY)
check_type_size(int   SIZEOF_INT   BUILTIN_TYPES_ONLY)
check_type_size(long  SIZEOF_LONG  BUILTIN_TYPES_ONLY)

if(SIZEOF_SHORT EQUAL 2)
  set(SIZE16 short)
elseif(SIZEOF_INT EQUAL 2)
  set(SIZE16 int)
endif()

if(NOT DEFINED SIZE16)
  message(FATAL_ERROR "No 16-bit integer type found on this platform!")
endif()

if(SIZEOF_SHORT EQUAL 4)
  set(SIZE32 short)
elseif(SIZEOF_INT EQUAL 4)
  set(SIZE32 int)
elseif(SIZEOF_LONG EQUAL 4)
  set(SIZE32 long)
endif()

if(NOT DEFINED SIZE32)
  message(FATAL_ERROR "No 32-bit integer type found on this platform!")
endif()

configure_file(
  "${PREFIX}/include/ao/os_types.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/include/ao/os_types.h"
  @ONLY
)

set(CMAKE_REQUIRED_LIBRARIES winmm)
check_c_source_compiles([[
#include <windows.h>
#include <mmsystem.h>
int main(int na, char ** a) {
    return waveOutOpen(0,0,0,0,0,0);
}
]]
  HAVE_WMM
)

set(HEADERS
  "${PREFIX}/include/ao/ao.h"
  "${PREFIX}/include/ao/ao_private.h"
  "${PREFIX}/include/ao/plugin.h"
  "${CMAKE_CURRENT_BINARY_DIR}/include/ao/os_types.h"
)

set(SOURCES
  "${PREFIX}/src/ao_aixs.c"
  "${PREFIX}/src/ao_au.c"
  "${PREFIX}/src/ao_null.c"
  "${PREFIX}/src/ao_raw.c"
  "${PREFIX}/src/ao_wav.c"
  "${PREFIX}/src/audio_out.c"
  "${PREFIX}/src/config.c"
)

if(HAVE_WMM)
  set(SOURCES
    "${SOURCES}"
    "${PREFIX}/src/ao_wmm.c")
endif(HAVE_WMM)

add_library(ao "${HEADERS}" "${SOURCES}")

target_compile_definitions(ao PRIVATE AO_BUILDING_LIBAO)
if(HAVE_WMM)
  target_compile_definitions(ao PRIVATE HAVE_WMM)
endif(HAVE_WMM)
target_include_directories(ao PUBLIC
  "${PREFIX}/include"
  "${CMAKE_CURRENT_BINARY_DIR}/include/ao"
)
target_include_directories(ao PRIVATE
  "${PREFIX}/include/ao"
)

if(HAVE_WMM)
  target_link_libraries(ao PUBLIC winmm ksuser)
endif(HAVE_WMM)
