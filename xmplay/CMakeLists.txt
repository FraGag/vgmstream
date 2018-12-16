cmake_minimum_required(VERSION 2.6)

set(HEADERS
  xmpfunc.h
  xmpin.h
)
set(SOURCES xmp_vgmstream.c)

add_library(xmp-vgmstream MODULE ${HEADERS} ${SOURCES} xmpin.def)

target_link_libraries(xmp-vgmstream vgmstream version)
