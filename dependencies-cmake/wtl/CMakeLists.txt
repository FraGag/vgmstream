cmake_minimum_required(VERSION 2.6)

set(VERSION "9.1.1")
set(ZIP_FILE "${CMAKE_CURRENT_SOURCE_DIR}/wtl-${VERSION}.zip")
set(TMP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/wtl_tmp")
set(PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/wtl-${VERSION}")

if(NOT IS_DIRECTORY "${PREFIX}")
  if(NOT EXISTS "${ZIP_FILE}")
    message(STATUS "Downloading Windows Template Library...")
    file(DOWNLOAD "https://www.nuget.org/api/v2/package/wtl/${VERSION}" "${ZIP_FILE}")
  endif()

  if(EXISTS "${ZIP_FILE}")
    file(MAKE_DIRECTORY "${TMP_DIR}")
    execute_process(
      COMMAND "${CMAKE_COMMAND}" -E tar xf "${ZIP_FILE}"
      WORKING_DIRECTORY "${TMP_DIR}"
    )
    file(RENAME "${TMP_DIR}/lib/native" "${PREFIX}")
    file(REMOVE_RECURSE "${TMP_DIR}")
  endif()
endif()

add_library(wtl INTERFACE)
target_include_directories(wtl INTERFACE
  "${PREFIX}/include"
)
