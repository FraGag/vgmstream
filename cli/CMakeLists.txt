cmake_minimum_required(VERSION 2.6)

add_executable(vgmstream_cli
  vgmstream_cli.c
)

target_link_libraries(vgmstream_cli vgmstream getopt version)
