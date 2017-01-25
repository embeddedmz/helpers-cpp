# Helpers for C++
[![MIT license](https://img.shields.io/badge/license-MIT-blue.svg)](http://opensource.org/licenses/MIT)


## About
A set of utility functions/classes to simplify some operations related to file system, handling zip archives etc...
The helpers wraps libraries such as Boost, Libzippp (wrapper for the C library Libzip which is also a wrapper for
ZLib). They are meant to be a portable and easy-to-use APIs.

Compilation has been tested with:
- GCC 5.4.0 (GNU/Linux Ubuntu 16.04 LTS)
- Microsoft Visual Studio 2015 (Windows 10)

Underlying libraries:
- [Boost](http://www.boost.org/)
- [libzippp](https://github.com/ctabin/libzippp)

## File system handling

To create a directory(ies) :
```cpp
// under Windows
Directory::CreateFolder("C:\\LOCALREP_TMP\\X");

// under Linux
Directory::CreateFolder("/home/amzoughi/LOCALREP_TMP/X");

/* LOCALREP_TMP must exist, otherwise the static method will return false */

// under Windows
Directory::CreateDirectories("C:\\LOCALREP_TMP\\A\\B\\C");

// under Linux
Directory::CreateDirectories("/home/amzoughi/LOCALREP_TMP/A/B/C");

/* C directory will be created even if B and A don't exist */
```

To delete a folder :

```cpp
bool bSuccess = false;
size_t usCount = Directory::EraseFolder("/home/amzoughi/delete_me", bSuccess);

/* usCount is the count of deleted items, if the erased folder was empty, it will be equal to one */

if (bSuccess)
{
   /* delete_me/ is deleted */
}
```

To check the existence of a file or a directory :

```cpp
// checks if LOCALREP_TMP exists and is a directory
bool bRes = Directory::IsDirectory("/home/amzoughi/LOCALREP_TMP");

// checks if a.txt exists and is a file
bool bRes = Directory::IsFile("/home/amzoughi/LOCALREP_TMP/a.txt");
```

To list a directory and get different maps that can be useful in certain situations (for example, the sorted folder
maps can be useful to create folders elsewhere from the bottom. I already used that in a file synchronizer program.

```cpp
/* need to instanciate an object to hold the maps holding the elements of a directory */
Directory MyDirectory;

// to list the folders of the directory "LOCALREP_TMP"
std::string strResult = MyDirectory.ListFolders("/home/amzoughi/LOCALREP_TMP/");

// to list recursively the folders the same directory, set the second parameter to true
std::string strResult = MyDirectory.ListFolders("/home/amzoughi/LOCALREP_TMP/", true);

/* Hash maps or ordered maps (according to the increasing number of '/' or '\\') of paths */
const auto& MapOne = MyDirectory.GetMapFoldersAbsRel(); // e.g. key : /home/amzoughi/LOCALREP_TMP/A/B -> value : A/B
const auto& MapTwo = MyDirectory.GetMapFoldersRelAbs(); // e.g. key : A/B -> value : /home/amzoughi/LOCALREP_TMP/A/B
/* same as above but sorted according sep. occurences */
const auto& MapThree = MyDirectory.GetMapSortedFoldersAbsRel();
const auto& MapFour = MyDirectory.GetMapSortedFoldersRelAbs();

// to list the files of the home directory "amzoughi"
std::string strResult = MyDirectory.ListFiles("/home/amzoughi/");
// to list recursively the files the same directory, set the second parameter to true
std::string strResult = MyDirectory.ListFiles("/home/amzoughi/", true);
   
const auto& MapOne = MyDirectory.GetMapFilesAbsRel(); // e.g. key : /home/amzoughi/foobar.txt -> value : foobar.txt
const auto& MapTwo = MyDirectory.GetMapFilesRelAbs(); // e.g. key : foobar.txt -> value : /home/amzoughi/foobar.txt
/* same as above but sorted according sep. occurences */
const auto& MapThree = MyDirectory.GetMapSortedFilesAbsRel();
const auto& MapFour = MyDirectory.GetMapSortedFilesRelAbs();
```

To remove the file name from a local URL (path) :

```cpp
std::string strResultFileName = Directory::RemoveFileName("/home/amzoughi/test.txt");

/* strResultFileName contains "/home/amzoughi/" with the sep. at the end */
```

To rename a file (You'll need to specify the complete new path) :

```cpp
if (Directory::Rename("/home/amzoughi/test.txt", "/home/amzoughi/renamed_test.txt"))
{
   /* test.txt is renamed to renamed_test.txt */
}
```

To delete a file :

```cpp
if (Directory::EraseFile("/home/amzoughi/test.txt"))
{
   /* test.txt is deleted */
}
```

To request a file's last modification time (mtime) :

```cpp
std::time_t tRes = Directory::GetLastWriteTime("test.txt");
```

To request a file's size :

```cpp
bool bRes = false;
size_t usCount = Directory::FileSize("test.txt", bRes);

if (bRes)
{
   /* test.txt is a file and usCount contains its size if it exists 
    * otherwise, zero. */
}
```

To delete files that are older than x days (last modification time). This feature is useful to help cleanup a folder :

```cpp
/* delete files older than 10 day (from now)
 * the third parameter can be set to true to recursively clean up the folder (false by default) */
size_t uCleanedUpCount = Directory::CleanUpFiles("/home/amzoughi/Downloads/", 10, true);

/* uCleanedUpCount will contain the count of erased elements */
```

## Zip handling

If a function returns bool, always test against the returned value to check if the operation
was successful.

To extract all the files or a single file from a ZIP archive :

```cpp
size_t uUnzippedFilesCount = 0;
Zip::ExtractAllFilesFromZip("/home/test/", "test.zip", uUnzippedFilesCount);
```

Please note, that there is an optional 4th paramter of ExtractAllFilesFromZip and can be used to set a progress meter
function (can be used in a GUI progress bar to notify the user of how many files have been extracted, but also 5th argument
to set a callback for error message printing. This last in also available for the methods listed below.

```cpp
/* extract the archived file 'Pictures/cats.jpg' to /home/test/cats.jpg */
Zip::ExtractSingleFileFromZip("/home/test/", "/home/test/test.zip", "Pictures/cats.jpg");
```

To add an empty directory or an existing file to a ZIP archive :

```cpp
/* An empty directory 'NewDir' will be added to test.zip*/
Zip::AddDirectoryEntryToZip("test.zip", "NewDir");

/* 'file.txt' will be added to 'test.zip' under the name 'zipped_file.txt' */
Zip::AddFileToZip("/home/file.txt", "test.zip", "zipped_file.txt");
```

To remove a directory or a file from a ZIP archive :

```cpp
long lCount = Zip::RemoveEntryFromZip("test.zip", "DirTest");
/* lCount should be equal to the count of removed elements from DirTest 
 * including itself */

/* note the usage of '/' at the end, it is recommended to append '/'
 * at the end of a directory entry */
lCount = Zip::RemoveEntryFromZip("test.zip", "DirTest2/");

lCount = Zip::RemoveEntryFromZip("test.zip", "Dummy_File.txt");
/* lCount should be equal to one */
```

To extract text from an archived file ("Doc/text.txt") :

```cpp
bool bRes = false;
const std::string strResult = Zip::ExtractTextFromZip("/home/test.zip", "Doc/text.txt", bRes);

if(bRes)
{
   /* strResult will contain the text of the archived file */
}
```

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
