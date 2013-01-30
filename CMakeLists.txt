cmake_minimum_required(VERSION 2.8)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(CMakeExtraBootstrap)
include(GNUInstallDirs)
include(ParseVersion)
include(AddSubmodule)
include(FindCXX11)
include(FixCCache)
include(FixStaticLink)

project(liblogging CXX)
parse_version(liblogging VERSION 0.0.1)

add_submodule(boost-log lib/boost-log INCLUDE_DIRS .)

find_package(Boost COMPONENTS filesystem system thread REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

include_directories(include)

# -------------------------------------------------------------------------
# liblogging
file(GLOB_RECURSE liblogging_SOURCES src/*.cpp)
file(GLOB_RECURSE boost-log_SOURCES
  ${boost-log_SOURCE_DIR}/libs/log/src/attribute_name.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/attribute_set.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/attribute_value_set.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/code_conversion.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/core.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/date_time_format_parser.cpp
#  ${boost-log_SOURCE_DIR}/libs/log/src/debug_output_backend.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/default_attribute_names.cpp
#  ${boost-log_SOURCE_DIR}/libs/log/src/default_filter_factory.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/default_sink.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/event.cpp
#  ${boost-log_SOURCE_DIR}/libs/log/src/event_log_backend.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/exceptions.cpp
#  ${boost-log_SOURCE_DIR}/libs/log/src/filter_parser.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/format_parser.cpp
#  ${boost-log_SOURCE_DIR}/libs/log/src/formatter_parser.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/global_logger_storage.cpp
#  ${boost-log_SOURCE_DIR}/libs/log/src/init_from_settings.cpp
#  ${boost-log_SOURCE_DIR}/libs/log/src/init_from_stream.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/light_rw_mutex.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/named_scope.cpp
#  ${boost-log_SOURCE_DIR}/libs/log/src/named_scope_format_parser.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/once_block.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/parser_utils.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/process_id.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/process_name.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/record_ostream.cpp
#  ${boost-log_SOURCE_DIR}/libs/log/src/settings_parser.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/severity_level.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/spirit_encoding.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/syslog_backend.cpp
#  ${boost-log_SOURCE_DIR}/libs/log/src/text_file_backend.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/text_ostream_backend.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/thread_id.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/thread_specific.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/threadsafe_queue.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/timer.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/timestamp.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/trivial.cpp
  ${boost-log_SOURCE_DIR}/libs/log/src/unhandled_exception_count.cpp
)
add_library(logging SHARED
  ${liblogging_SOURCES}
  ${boost-log_SOURCES}
)
set_target_properties(logging PROPERTIES
  VERSION   ${liblogging_VERSION}
  SOVERSION ${liblogging_SOVERSION}
  SONAME    liblogging${liblogging_SOVERSION}
)
target_link_libraries(logging
  ${Boost_LIBRARIES}
  pthread rt
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
if(CMAKE_BUILD_TESTS)
  enable_testing()

  add_executable(unit-test-logging
    test/test1.cpp
  )
  target_link_libraries(unit-test-logging logging)
  add_test(unit-test-logging unit-test-logging)
endif()
