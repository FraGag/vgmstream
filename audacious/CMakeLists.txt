cmake_minimum_required(VERSION 2.6)

if(NOT CMAKE_CXX_COMPILER)
  message(STATUS "No C++ compiler was found. The Audacious plugin will not be available.")
  return()
endif()

if(NOT PKG_CONFIG_FOUND)
  message(STATUS "pkg-config was not found. The Audacious plugin will not be available.")
  return()
endif()

pkg_check_modules(glib2 IMPORTED_TARGET glib-2.0)
if(NOT glib2_FOUND)
  message(STATUS "The development files for glib-2.0 were not found. The Audacious plugin will not be available.")
  return()
endif()

pkg_check_modules(audacious IMPORTED_TARGET audacious)
if(NOT audacious_FOUND)
  message(STATUS "The development files for Audacious were not found. The Audacious plugin will not be available.")
  return()
endif()

if(audacious_VERSION VERSION_LESS "3.6")
  message(STATUS "The development files for Audacious are for a version of Audacious that is too old (< 3.6). The Audacious plugin will not be available.")
  return()
endif()

set(HEADERS
  plugin.h
  vfs.h
)
set(SOURCES
  plugin.cc
  vfs.cc
)

add_library(vgmstream_audacious MODULE ${HEADERS} ${SOURCES})
set_target_properties(vgmstream_audacious
  PROPERTIES LIBRARY_OUTPUT_NAME vgmstream
)
target_link_libraries(vgmstream_audacious vgmstream version PkgConfig::audacious PkgConfig::glib2)
