cmake_minimum_required(VERSION 2.6)

execute_process(
  COMMAND git describe --always
  RESULT_VARIABLE GIT_RESULT
  OUTPUT_VARIABLE GIT_OUTPUT
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(GIT_RESULT EQUAL 0)
  string(REPLACE ":" "_" VERSION "${GIT_OUTPUT}")
  configure_file(
    "${CURRENT_SOURCE_DIR}/version.h.in"
    "${CURRENT_BINARY_DIR}/version.h"
    @ONLY
  )
endif()
