cmake_minimum_required(VERSION 3.5)
project(d16_jit)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
find_package(GLib REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c99 -O0 -ggdb3")
find_program(LUA_COMMAND luajit)

function(dasm_process filename)
    get_filename_component(base ${filename} NAME_WE)
    set(base_abs ${CMAKE_CURRENT_BINARY_DIR}/${base})
    set(output ${base_abs}.c)
    add_custom_command(
            OUTPUT ${output}
            COMMAND ${LUA_COMMAND} dynasm/dynasm.lua -o ${CMAKE_CURRENT_BINARY_DIR}/${base}.c ${filename}
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            MAIN_DEPENDENCY ${filename}
    )
    set_source_files_properties(${output} PROPERTIES GENERATED TRUE)
endfunction()

dasm_process(dyn_jit.dasc)
include_directories( ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/dynasm)
include_directories(${Glib_INCLUDE_DIR} ${GlibConfig_INCLUDE_DIR})

set(SOURCE_FILES main.c jit_macros.h ${CMAKE_CURRENT_BINARY_DIR}/dyn_jit.c dasm_driver.c dasm_driver.h cpu_intrinsics.c io.c)
add_executable(d16_jit ${SOURCE_FILES})
target_link_libraries(d16_jit ${Glib_LIBRARY})
