cmake_minimum_required(VERSION 2.6)

if(WIN32 AND CMAKE_SIZEOF_VOID_P EQUAL 4)
  add_subdirectory(foobar2000)
  add_subdirectory(wtl)
endif()
