/**
 * @file Helpers.h
 * @brief Helpers (File system, ZIP files...) in C++
 * Wrappers for Boost file system, libzippp,...
 *
 * @author Mohamed Amine Mzoughi <mohamed-amine.mzoughi@laposte.net>
 * @date 2016-08-09
 */
 
#ifndef INCLUDE_LOCALREP_H_
#define INCLUDE_LOCALREP_H_

#include <boost/filesystem.hpp>
/* to count '/' occurrences in a path (used to sort the folders/files listing) */
#include <boost/range/algorithm/count.hpp>
#include <cstdio>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

#include "libzippp.h"

// the same size I use in my MD5 SHA1 Calculator....
constexpr size_t MAX_FILE_BUFFER = (32 * 20 * 820);

using namespace libzippp;
namespace fs = boost::filesystem; 

// ZIP operations
namespace Zip
{
   typedef std::function<void(double, double)> ProgressCallback;
   typedef std::function<void(const std::string&)> ErrorCallback;

   // using auto will cause link errors...
   #define DefaultProgressCallback [](double dA, double dB) { dA; dB; }
   #define DefaultErrorCallback [](const std::string& strErrorMsg) { strErrorMsg; }

   const bool ExtractAllFilesFromZip(const std::string& strOutputDirectory,
                                     const std::string& strZipFile,
                                     size_t& uCount,
                                     ProgressCallback ProgressStrategy = DefaultProgressCallback,
                                     ErrorCallback ErrorStrategy = DefaultErrorCallback);
   
   const bool ExtractSingleFileFromZip(const std::string& strOutDirectory,
                                       const std::string& strZipFile,
                                       const std::string& strZipEntry,
                                       ErrorCallback ErrorStrategy = DefaultErrorCallback);

   std::string ExtractTextFromZip(const std::string& strZipFile,
                                  const std::string& strZipEntry,
                                  bool& bSuccess,
                                  ErrorCallback ErrorStrategy = DefaultErrorCallback);
   
   const bool AddFileToZip(const std::string& strFile,
                           const std::string& strZipFile,
                           const std::string& strZipEntry,
                           ErrorCallback ErrorStrategy = DefaultErrorCallback);

   const long RemoveEntryFromZip(const std::string& strZipFile, const std::string& strZipEntry);
   
   const bool AddDirectoryEntryToZip(const std::string& strZipFile, const std::string& strZipEntry);
}

class Directory
{
public:
   // functor to sort the vector of paths according to '/' occurences
   struct SlashOccurrencesComparison
   {
      // TODO : slashes - back-slashes management has to be optimized...
      bool operator()(const std::string& strA, const std::string& strB)
      {
         if ((strA.find_first_of('\\') != std::string::npos) || (strA.find_first_of('/') != std::string::npos))
         {
            if ((strB.find_first_of('\\') != std::string::npos) || (strB.find_first_of('/') != std::string::npos))
            {
               if ((boost::count(strA, '\\') == boost::count(strB, '\\') && boost::count(strA, '\\') != 0)
                  || (boost::count(strA, '/') == boost::count(strB, '/') && boost::count(strA, '/') != 0))
                  return strA < strB;
               else if (strA.find_first_of('\\') != std::string::npos)
                  return (boost::count(strA, '\\') < boost::count(strB, '\\'));
               else
                  return (boost::count(strA, '/') < boost::count(strB, '/'));
            }
            else
               return false;
         }
         else if ((strB.find_first_of('\\') != std::string::npos) || (strB.find_first_of('/') != std::string::npos))
            return true;

         if (strA.length() == strB.length())
            return strA < strB;

         return (strA.length() < strB.length());
      }
   };

   typedef std::unordered_map<std::string, std::string> HashMap;
   typedef std::map<std::string, std::string, SlashOccurrencesComparison> SortedMap;

   enum PathType
   {
      ABSOLUTE_PATH,
      RELATIVE_PATH
   };

   /* class methods */
   static const bool CreateFolder(const std::string& strPath);
   static const bool CreateDirectories(const std::string& strPath);
   static const bool IsDirectory(const std::string& strPath);
   static const bool IsFile(const std::string& strPath);
   
   /* e.g. C:\LOCALREP_TMP\regular_file.txt => C:\LOCALREP_TMP\ */
   static std::string RemoveFileName(const std::string& strPath);
   
   static const bool Rename(const std::string& strFilePath, const std::string& strNewName);
   static const bool EraseFile(const std::string& strFilePath);
   static const size_t EraseFolder(const std::string& strFolderPath, bool& bSuccess);
   static std::time_t GetLastWriteTime(const std::string& strFilePath);
   static const size_t CleanUpFiles(const std::string& strDirectory,
                                    const size_t& usKeepDays,
                                    const bool& bRecursive = false);
   static const size_t FileSize(const std::string& strFile, bool& bSuccess);

   /* object methods */
   std::string ListFolders(const std::string& strLocation,
      bool bRecursive = false,
      PathType ePathType = ABSOLUTE_PATH);

   std::string ListFiles(const std::string& strPath,
      bool bRecursive = false,
      PathType ePathType = ABSOLUTE_PATH);

   inline const size_t GetFilesCount() const { return m_mapFilesAbsRel.size(); }
   inline const size_t GetFoldersCount() const { return m_mapFoldersAbsRel.size(); }

   const HashMap& GetMapFoldersRelAbs() const { return m_mapFoldersRelAbs; }
   
   const HashMap& GetMapFoldersAbsRel() const { return m_mapFoldersAbsRel; }
   
   const SortedMap& GetMapSortedFoldersAbsRel() const { return m_mapSortedFoldersAbsRel; }
   
   const SortedMap& GetMapSortedFoldersRelAbs() const { return m_mapSortedFoldersRelAbs; }
   
   const HashMap& GetMapFilesRelAbs() const { return m_mapFilesRelAbs; }
   
   const HashMap& GetMapFilesAbsRel() const { return m_mapFilesAbsRel; }
   
   const SortedMap& GetMapSortedFilesAbsRel() const { return m_mapSortedFilesAbsRel; }
   
   const SortedMap& GetMapSortedFilesRelAbs() const { return m_mapSortedFilesRelAbs; }

protected:
   // If "C:\XXXX" was listed :
   // Only folders paths (relative -> absolute paths and vice versa)
   HashMap m_mapFoldersRelAbs; // Unordered e.g. A -> C:\XXXX\A
   HashMap m_mapFoldersAbsRel; // Unordered e.g. C:\XXXX\A -> A
   SortedMap m_mapSortedFoldersRelAbs; // Ordered e.g. A -> C:\XXXX\A
   SortedMap m_mapSortedFoldersAbsRel; // Ordered e.g. C:\XXXX\A -> A
   /* recursively e.g. A\B\C <-> C:\XXXX\A\B\C and vice versa */
   
   // Only files paths (relative -> absolute paths and vice versa)
   HashMap m_mapFilesRelAbs; // Unordered e.g. A\foobar.txt -> C:\XXXX\A\foobar.txt
   HashMap m_mapFilesAbsRel; // Unordered e.g. C:\XXXX\A\foobar.txt -> A\foobar.txt
   SortedMap m_mapSortedFilesRelAbs; // Ordered e.g. A\foobar.txt -> C:\XXXX\A\foobar.txt
   SortedMap m_mapSortedFilesAbsRel; // Ordered e.g. C:\XXXX\A\foobar.txt -> A\foobar.txt
   /* not recursively e.g. data.txt <-> C:\XXXX\data.txt and vice versa */

};

#endif // INCLUDE_LOCALREP_H_
