cmake_minimum_required(VERSION 3.28.2)
include(../sdk/config.cmake)
project(sieelf_libsie C)

file(GLOB_RECURSE SOURCES "src/*.c")

add_library(sie_nsg SHARED ${SOURCES})
target_sdk_setup(sie_nsg NSG)
target_compile_options(sie_nsg BEFORE PUBLIC -std=gnu99 -Wall -Werror)
target_link_libraries(sie_nsg crt_helper gcc c ft)
set_target_properties(sie_nsg PROPERTIES LIBRARY_OUTPUT_NAME sie)

add_library(sie_elka SHARED ${SOURCES})
target_sdk_setup(sie_elka ELKA)
target_compile_options(sie_elka BEFORE PUBLIC -std=gnu99 -Wall -Werror)
target_link_libraries(sie_elka crt_helper gcc c ft)
set_target_properties(sie_elka PROPERTIES LIBRARY_OUTPUT_NAME sie)
