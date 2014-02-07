cmake_minimum_required(VERSION 2.8)
project(routing-drawer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Werror -Wno-unused-variable -Wno-strict-aliasing -O2 -ggdb")

set(SOURCES
  src/main.cpp
  src/geometry.cpp
  src/binary_parser.cpp
  src/graph_dumper.cpp
)

set(HEADERS
  src/graph.hpp
  src/geometry.hpp
  src/binary_parser.hpp
  src/graph_dumper.hpp
)

find_package(PkgConfig)

pkg_check_modules(CAIRO REQUIRED cairo)
include_directories(${CAIRO_INCLUDE_DIRS})
link_directories(${CAIRO_LIBRARY_DIRS})

add_executable(routing-drawer ${SOURCES} ${HEADERS})
target_link_libraries(routing-drawer ${CAIRO_LIBRARIES})
