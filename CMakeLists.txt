cmake_minimum_required(VERSION 2.6)

project(gpio-test)
set(CMAKE_CXX_FLAGS "-g -Wall")
include_directories(gpio/)
file(
     GLOB_RECURSE

     source_files

     gpio/*

     sample/*
)

add_executable(
     gpio-test
     ${source_files}
)

add_library( oa20_gpio gpio/oa20_gpio.c)

INSTALL(TARGETS gpio-test oa20_gpio
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
INSTALL( FILES gpio/oa20_gpio.h DESTINATION include)
