cmake_minimum_required(VERSION 2.8)
find_package(Git REQUIRED)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(GNUInstallDirs)
#include(DebianVersion)
include(AddSubmodule)

#parse_debian_version(LIBLOGGING_VERSION)
add_submodule(boost-log lib/boost-log
  INCLUDE_DIRS .
)

project(liblogging CXX)

find_package(Boost COMPONENTS program_options filesystem system regex thread chrono REQUIRED)
include_directories(
  ${Boost_INCLUDE_DIRS}
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
file(GLOB_RECURSE boost-log_SOURCES
  ${boost-log_SOURCE_DIR}/libs/log/src/attribute_name.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/attribute_set.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/attribute_values_view.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/code_conversion.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/core.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/record_ostream.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/severity_level.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/global_logger_storage.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/named_scope.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/process_id.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/thread_id.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/timer.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/exceptions.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/default_sink.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/text_ostream_backend.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/text_file_backend.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/syslog_backend.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/thread_specific.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/once_block.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/timestamp.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/threadsafe_queue.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/event.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/trivial.cpp
)
add_library(logging SHARED
  ${LIBLOGGING_SOURCES}
  ${boost-log_SOURCES}
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
