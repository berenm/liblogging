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


add_submodule(libcorefungi git://github.com/berenm/libcorefungi.git lib/corefungi BRANCH master INCLUDE_DIRS include)
include_directories(${libcorefungi_INCLUDE_DIRS})

find_package(Boost COMPONENTS filesystem system thread REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

# -------------------------------------------------------------------------
# liblogging
include_directories(include)

file(GLOB_RECURSE liblogging_SOURCES src/*.cpp)
add_library(logging ${liblogging_SOURCES})
set_target_properties(logging PROPERTIES
  VERSION   ${liblogging_VERSION}
  SOVERSION ${liblogging_SOVERSION}
  SONAME    liblogging${liblogging_SOVERSION}
)
target_link_libraries(logging corefungi ${Boost_LIBRARIES})

# -------------------------------------------------------------------------
# installation
install(TARGETS logging
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(DIRECTORY include/logging
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# -------------------------------------------------------------------------
# tests
if(BUILD_TESTS)
  enable_testing()

  add_executable(liblogging-test-1 test/test1.cpp)
  target_link_libraries(liblogging-test-1 logging)
  add_test(liblogging-test-1 liblogging-test-1)
endif()

if(ENABLE_GCOVERALLS)
  include(GCoveralls)
  enable_gcoveralls()
endif()
