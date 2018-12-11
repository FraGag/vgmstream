cmake_minimum_required(VERSION 2.6)

set(HEADERS
  foo_filetypes.h
  foo_prefs.h
  foo_vgmstream.h
)

set(SOURCES
  foo_prefs.cpp
  foo_streamfile.cpp
  foo_vgmstream.cpp
)

if(CMAKE_RC_COMPILER)
  set(HEADERS ${HEADERS} resource.h)
  set(SOURCES ${SOURCES} foo_input_vgmstream.rc)
endif(CMAKE_RC_COMPILER)

add_library(foo_input_vgmstream MODULE ${HEADERS} ${SOURCES})
target_link_libraries(foo_input_vgmstream
  vgmstream
  version
  foobar2000 # for ATLHelpers
  foobar2000_SDK
  foobar2000_shared
)
