cmake_minimum_required(VERSION 2.6)

add_library(getopt
  getopt.h
  getopt.c
)
target_include_directories(getopt PUBLIC .)
