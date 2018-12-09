cmake_minimum_required(VERSION 2.6)

# clHCA
add_library(clHCA
  ../ext_includes/clHCA.h
  clHCA.c
)
target_include_directories(clHCA PUBLIC ../ext_includes)

# Getopt
add_subdirectory(Getopt)
