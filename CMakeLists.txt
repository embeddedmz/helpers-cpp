cmake_minimum_required(VERSION 2.6)

# Project configuration
project(helpers)
set(LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})
set(CMAKE_CXX_STANDARD 11)

add_definitions(-DLINUX)

# Locate Boost
find_package(Boost COMPONENTS filesystem system REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(HELPERS)
include_directories(../../libzippp/src)
include_directories(../../libzippp/lib/libzip-1.1.3/lib)
include_directories(../../libzippp/lib/zlib-1.2.8/)

file(GLOB_RECURSE helpers_source_files HELPERS/*)

add_library(helpers STATIC ${helpers_source_files})
