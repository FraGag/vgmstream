cmake_minimum_required(VERSION 2.6)

add_executable(vgmstream_cli
  vgmstream_cli.c
)

target_link_libraries(vgmstream_cli vgmstream getopt version)

add_executable(vgmstream123
  vgmstream123.c
)

target_link_libraries(vgmstream123 vgmstream getopt version ao)
