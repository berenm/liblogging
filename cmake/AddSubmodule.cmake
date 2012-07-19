include(CMakeParseArguments)

find_package(Git REQUIRED)

function(add_submodule SUBMODULE_NAME SUBMODULE_FOLDER)

  set(GTULU_PROGRAM_ARGS_FLAGS )
  set(GTULU_PROGRAM_ARGS_ONE_VALUE   )
  set(GTULU_PROGRAM_ARGS_MULTI_VALUE INCLUDE_DIRS)
  cmake_parse_arguments(SUBMODULE "${GTULU_PROGRAM_ARGS_FLAGS}" "${GTULU_PROGRAM_ARGS_ONE_VALUE}" "${GTULU_PROGRAM_ARGS_MULTI_VALUE}" ${ARGN} )

  if(NOT DEFINED ${SUBMODULE_NAME}_SOURCE_DIR)
    message(STATUS "updating ${SUBMODULE_NAME} submodule...")
    execute_process(COMMAND git submodule update --init ${SUBMODULE_FOLDER} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} OUTPUT_QUIET)

    if(EXISTS ${SUBMODULE_FOLDER}/CMakeLists.txt)
      add_subdirectory(${SUBMODULE_FOLDER})
    endif()

    if(NOT DEFINED ${SUBMODULE_NAME}_SOURCE_DIR)
      set(${SUBMODULE_NAME}_SOURCE_DIR ${SUBMODULE_FOLDER})
      set(${SUBMODULE_NAME}_SOURCE_DIR ${SUBMODULE_FOLDER} PARENT_SCOPE)
    endif()

    foreach(SUBMODULE_INCLUDE_DIR ${SUBMODULE_INCLUDE_DIRS})
      if(NOT IS_ABSOLUTE ${SUBMODULE_INCLUDE_DIR})
        set(SUBMODULE_INCLUDE_DIR ${${SUBMODULE_NAME}_SOURCE_DIR}/${SUBMODULE_INCLUDE_DIR})
      endif()

      message(STATUS "adding include dir ${SUBMODULE_INCLUDE_DIR}")
      include_directories(${SUBMODULE_INCLUDE_DIR})
    endforeach()
  endif()

endfunction(add_submodule)
