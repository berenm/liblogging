cmake_minimum_required(VERSION 2.8)
find_package(Git REQUIRED)
message(STATUS "updating liblogging submodules...")
execute_process(COMMAND git submodule update --init WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} OUTPUT_QUIET)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(GNUInstallDirs)

project(liblogging CXX)

find_package(Boost COMPONENTS program_options filesystem system regex thread chrono REQUIRED)
include_directories(
  ${Boost_INCLUDE_DIRS}
  lib/boost-log
)
link_directories(${Boost_LIBRARY_DIRS})

# -------------------------------------------------------------------------
# project version
set(LIBLOGGING_VERSION 0.0.1)

include_directories(include)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# -------------------------------------------------------------------------
# liblogging
file(GLOB_RECURSE LIBLOGGING_SOURCES src/*.cpp)
file(GLOB_RECURSE BOOSTLOG_SOURCES
  lib/boost-log/libs/log/src/attribute_name.cpp
  lib/boost-log/libs/log/src/attribute_set.cpp
  lib/boost-log/libs/log/src/attribute_values_view.cpp
  lib/boost-log/libs/log/src/code_conversion.cpp
  lib/boost-log/libs/log/src/core.cpp
  lib/boost-log/libs/log/src/record_ostream.cpp
  lib/boost-log/libs/log/src/severity_level.cpp
  lib/boost-log/libs/log/src/global_logger_storage.cpp
  lib/boost-log/libs/log/src/named_scope.cpp
  lib/boost-log/libs/log/src/process_id.cpp
  lib/boost-log/libs/log/src/thread_id.cpp
  lib/boost-log/libs/log/src/timer.cpp
  lib/boost-log/libs/log/src/exceptions.cpp
  lib/boost-log/libs/log/src/default_sink.cpp
  lib/boost-log/libs/log/src/text_ostream_backend.cpp
  lib/boost-log/libs/log/src/text_file_backend.cpp
  lib/boost-log/libs/log/src/syslog_backend.cpp
  lib/boost-log/libs/log/src/thread_specific.cpp
  lib/boost-log/libs/log/src/once_block.cpp
  lib/boost-log/libs/log/src/timestamp.cpp
  lib/boost-log/libs/log/src/threadsafe_queue.cpp
  lib/boost-log/libs/log/src/event.cpp
  lib/boost-log/libs/log/src/trivial.cpp
)
add_library(logging SHARED
  ${LIBLOGGING_SOURCES}
  ${BOOSTLOG_SOURCES}
)
set_target_properties(logging PROPERTIES
  VERSION   "${LIBLOGGING_VERSION}"
  SOVERSION "${LIBLOGGING_SOVERSION}"
  SONAME    "liblogging${LIBLOGGING_SOVERSION}"
)
target_link_libraries(logging
  ${Boost_LIBRARIES}
)

# -------------------------------------------------------------------------
# installation
install(TARGETS logging
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(DIRECTORY include/logging
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# -------------------------------------------------------------------------
# tests
enable_testing()

add_executable(unit-test-logging
  test/test1.cpp
)
target_link_libraries(unit-test-logging logging)
add_test(unit-test-logging unit-test-logging)
