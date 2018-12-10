cmake_minimum_required(VERSION 2.6)

set(HEADERS)
set(SOURCES in_vgmstream.c)

if(CMAKE_RC_COMPILER)
  set(HEADERS ${HEADERS} resource.h)
  set(SOURCES ${SOURCES} resource.rc)
endif(CMAKE_RC_COMPILER)

add_library(in_vgmstream MODULE ${HEADERS} ${SOURCES})

target_compile_definitions(in_vgmstream PRIVATE VGM_WINAMP_UNICODE)
target_link_libraries(in_vgmstream vgmstream version)
