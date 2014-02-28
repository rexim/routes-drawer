cmake_minimum_required(VERSION 2.8)
project(routes-drawer)

include(CheckCXXCompilerFlag REQUIRED)

check_cxx_compiler_flag("-std=c++11" FLAGS_SUPPORT_CXX11)
if(FLAGS_SUPPORT_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wc++11-compat")
    message(STATUS "Found C++11 support.")
else(FLAGS_SUPPORT_CXX11)
    check_cxx_compiler_flag("-std=c++0x" FLAGS_SUPPORT_CXX0X)
    if(FLAGS_SUPPORT_CXX0X)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wc++0x-compat")
        message(STATUS "Found C++0x support.")
    else(FLAGS_SUPPORT_CXX0X)
        message(SEND_ERROR "No C++0x support.")
    endif(FLAGS_SUPPORT_CXX0X)
endif(FLAGS_SUPPORT_CXX11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -O2 -ggdb")

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

add_executable(routes-drawer ${SOURCES} ${HEADERS})
target_link_libraries(routes-drawer ${CAIRO_LIBRARIES})
