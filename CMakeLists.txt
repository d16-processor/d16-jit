cmake_minimum_required(VERSION 3.5)
project(d16-jit)

OPTION(NO_JIT "Disable JIT compiler" OFF)
OPTION(NO_MIDI "Disable RTMidi" OFF)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
find_package(GLib REQUIRED)
find_package(RtMIDI REQUIRED)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -O0 -ggdb3")
if(NO_JIT)
  SET(SOURCE_FILES nojit.c)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DNO_JIT -g")
else(NO_JIT)

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

  set(SOURCE_FILES ${CMAKE_CURRENT_BINARY_DIR}/dyn_jit.c dasm_driver.c)
endif(NO_JIT)
include_directories( ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/dynasm)
include_directories(${Glib_INCLUDE_DIR} ${GlibConfig_INCLUDE_DIR})
if(NO_MIDI)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DNO_MIDI")
else(NO_MIDI)
  include_directories(${RTMIDI_INCLUDE_DIR})
  SET(SOURCE_FILES ${SOURCE_FILES} sound.c)
endif(NO_MIDI)
set(SOURCE_FILES ${SOURCE_FILES} main.c jit_macros.h dasm_driver.h cpu_intrinsics.c io.c timer.c)
add_executable(d16-jit ${SOURCE_FILES})
if(!NO_MIDI)
  target_link_libraries(d16-jit -lrtmidi)
endif(!NO_MIDI)
target_link_libraries(d16-jit ${Glib_LIBRARY} pthread)
install(TARGETS d16-jit DESTINATION bin)
