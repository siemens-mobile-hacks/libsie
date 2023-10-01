cmake_minimum_required(VERSION 3.14.1)

get_filename_component(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/../include ABSOLUTE)
get_filename_component(LIB_DIR ${CMAKE_SOURCE_DIR}/../lib ABSOLUTE)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIB_DIR})

macro(push_library lib)
    set(LIBS "${LIBS} ${LIB_DIR}/${lib}")
endmacro()

set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)
set(CMAKE_C_COMPILER "arm-eabi-gcc")
set(CMAKE_LINKER "arm-eabi-ld")

include_directories(${INCLUDE_DIR})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msoft-float -fshort-wchar -marm -mlittle-endian -mcpu=arm926ej-s")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -nostdlib -pipe -fno-exceptions -fdata-sections -ffunction-sections")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -Wall -Werror")

push_library(libcrt.a)
push_library(libcrt_helper.so)
push_library(libc-1.0.so)
push_library(libgcc-4.6.1.so)
push_library(libft-2.4.6-1.so)

set(CMAKE_SHARED_LINKER_FLAGS "-zmax-page-size=1 -marmelf -nostdlib --defsym __ex=0")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -shared -s -Bsymbolic -Bsymbolic-function -soname=libsie.so")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -rpath-link ${LIB_DIR} ${LIBS}")
set(CMAKE_C_CREATE_SHARED_LIBRARY "<CMAKE_LINKER> <LINK_FLAGS> <OBJECTS> -o <TARGET>")

project(sieelf_libsie)
file(GLOB_RECURSE SOURCES "src/*.c")

add_library(sie SHARED ${SOURCES})

add_custom_command(TARGET sie POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/include/sie/ ${INCLUDE_DIR}/sie/)
