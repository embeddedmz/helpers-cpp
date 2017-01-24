# Helpers for C++
[![MIT license](https://img.shields.io/badge/license-MIT-blue.svg)](http://opensource.org/licenses/MIT)


## About
A set of utility functions/classes to simplify some operations related to file system, handling zip archives etc...
The helpers wraps libraries such as Boost, Libzippp (wrapper for the C library Libzip which is also a wrapper for
ZLib). They are meant to be a portable and easy-to-use API.

Compilation has been tested with:
- GCC 5.4.0 (GNU/Linux Ubuntu 16.04 LTS)
- Microsoft Visual Studio 2015 (Windows 10)

Underlying libraries:
- [Boost](http://www.boost.org/)
- [libzippp](https://github.com/ctabin/libzippp)

## Zip handling

This section will be completed soon.

## File system handling

This section will be completed soon.


## Installation
You will need CMake to generate a makefile for the static library to build the tests or the code coverage 
program.

Also make sure you have Boost, Libzipp and Google Test installed.

The provided CMakeLists file have to be modified to indicate the path of Libzipp and its underlying
libraries.

You can follow this script https://gist.github.com/fideloper/f72997d2e2c9fbe66459 to install libcurl.

This tutorial will help you installing properly Google Test on Ubuntu: https://www.eriksmistad.no/getting-started-with-google-test-on-ubuntu/

The CMake script located in the tree will produce a makefile for the creation of a static library,
whereas the one under TestHELPERS will produce the unit tests program.

To create a debug static library, change directory to the one containing the first CMakeLists.txt

```Shell
cmake . -DCMAKE_BUILD_TYPE:STRING=Debug
make
```

To create a release static library, just change "Debug" by "Release".

The library will be found under lib/[BUILD_TYPE]/libhelpers.a

For the unit tests program, first build the static library and use the same build type when
building it :

```Shell
cd TestHELPERS/
cmake . -DCMAKE_BUILD_TYPE=Debug     # or Release
make
```

To run it, you must indicate the path of the INI conf file (see the section below)
```Shell
./bin/[BUILD_TYPE]/test_helpers /path_to_your_ini_file/conf.ini
```

## Run Unit Tests

[simpleini](https://github.com/brofield/simpleini) is used to gather unit tests parameters from
an INI configuration file.

A template of the INI file exists already under TestHELPERS/

You can also generate an XML file of test results by adding this argument when calling the test program

```Shell
./bin/[BUILD_TYPE]/test_helpers /path_to_your_ini_file/conf.ini --gtest_output="xml:./TestHELPERS.xml"
```

## Memory Leak Check

Visual Leak Detector has been used to check memory leaks with the Windows build (Visual Sutdio 2015)
You can download it here: https://vld.codeplex.com/

To perform a leak check with the Linux build, you can do so :

```Shell
valgrind --leak-check=full ./bin/Debug/test_helpers /path_to_ini_file/conf.ini
```

## Code Coverage

The code coverage build doesn't use the static library but compiles and uses directly the 
Helpers-C++ API in the test program.

First of all, in TestHELPERS/CMakeLists.txt, find and repalce :
```
"/home/amzoughi/Test/helpers_github.ini"
```
by the location of your ini file and launch the code coverage :

```Shell
cd TestHELPERS/
cmake . -DCMAKE_BUILD_TYPE=Coverage
make
make coverage_helpers
```

If everything is OK, the results will be found under ./TestHELPERS/coverage/index.html

Under Visual Studio, you can simply use OpenCppCoverage (https://opencppcoverage.codeplex.com/)

## Contribute
All contributions are highly appreciated. This includes updating documentation, writing code and unit tests
to increase code coverage and enhance tools.

Try to preserve the existing coding style (Hungarian notation, indentation etc...).
