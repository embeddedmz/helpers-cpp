cmake_minimum_required(VERSION 2.6)

project(TestHELPERS)
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})
set(CMAKE_CXX_STANDARD 11) # c++11
add_definitions(-DLINUX)

# Code coverage setup
IF(CMAKE_BUILD_TYPE MATCHES Coverage)
   INCLUDE(CodeCoverage.cmake)
   SET(CMAKE_CXX_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")
   SET(CMAKE_C_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage")
ENDIF(CMAKE_BUILD_TYPE MATCHES Coverage)

# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Locate Boost
set(Boost_USE_STATIC_LIBS ON) 
find_package(Boost COMPONENTS filesystem system REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

include_directories(./)
include_directories(./simpleini)
include_directories(../HELPERS)
include_directories(../../../libzippp/src)
include_directories(../../../libzippp/lib/libzip-1.1.3/lib)
include_directories(../../../libzippp/lib/zlib-1.2.8/)

link_directories(../../../libzippp/)
link_directories(../../../libzippp/lib/libzip-1.1.3/lib/.libs/)
link_directories(../../../libzippp/lib/zlib-1.2.8/)

IF(CMAKE_BUILD_TYPE MATCHES Coverage)

file(GLOB_RECURSE helpers_source_files ../HELPERS/*)

#Output Setup
add_executable(test_helpers Tests.cpp test_utils.cpp ${helpers_source_files})

#Link setup
target_link_libraries(test_helpers ${Boost_LIBRARIES} ${GTEST_LIBRARIES} pthread libzippp.a libzip.a libz.a)

SETUP_TARGET_FOR_COVERAGE(
           coverage_helpers
           test_helpers
           coverage
           "/home/amzoughi/Test/helpers_github.ini"
           )

ELSE(CMAKE_BUILD_TYPE MATCHES Coverage)

link_directories(../lib/${CMAKE_BUILD_TYPE})

#Output Setup
add_executable(test_helpers Tests.cpp test_utils.cpp)

#Link setup
target_link_libraries(test_helpers helpers ${Boost_LIBRARIES} ${GTEST_LIBRARIES} pthread libzippp.a libzip.a libz.a)

ENDIF(CMAKE_BUILD_TYPE MATCHES Coverage)
