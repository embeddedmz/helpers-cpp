/**
 * @file Helpers.cpp
 * @brief implementation of the helpers functions/methods
 * @author Mohamed Amine Mzoughi <mohamed-amine.mzoughi@laposte.net>
 */

#include "Helpers.h"

// ZIP

const bool Zip::ExtractAllFilesFromZip(const std::string& strDirectory, const std::string& strZipFile,
   size_t& uCount, ProgressCallback ProgressStrategy, ErrorCallback ErrorStrategy)
{
   bool bRes = false;
   uCount = 0;

   std::string strOutputDirectory(strDirectory);
   if (!Directory::IsDirectory(strDirectory) || !Directory::IsFile(strZipFile))
      return false;

   if (strOutputDirectory.at(strOutputDirectory.size() - 1) != '/'
      && strOutputDirectory.at(strOutputDirectory.size() - 1) != '\\')
   {
      if (strOutputDirectory.find_first_of('/') != std::string::npos)
         strOutputDirectory.append("/");
      else
         strOutputDirectory.append("\\");
   }

   ZipArchive zf(strZipFile);
   if (!zf.open(ZipArchive::READ_ONLY))
      return false; // Zip file couldn't be opened !

   std::vector<ZipEntry> entries = zf.getEntries();
   std::vector<ZipEntry>::iterator it;
   ZipEntry entry;

   // Determine the size (uncompressed) of all the zip entries to send it to the progress callback
   size_t uTotSize = 0;
   size_t uWrittenBytes = 0;
   for (it = entries.begin(); it != entries.end(); ++it)
   {
      entry = *it;
      if (entry.isFile())
         uTotSize += entry.getSize();
   }

   for (it = entries.begin(); it != entries.end(); ++it)
   {
      entry = *it;
      std::string strEntryName = entry.getName();
      size_t uSize = entry.getSize();
      size_t uCRC = entry.getCRC();

      // in rare cases, a directory might be coded incorrectly in a zip file : no '/' is appended at the
      // end of its name, that's why I check uCRC and uSize...
      if (entry.isDirectory() || uSize == 0 && uCRC == 0)
      {
         if (!Directory::CreateDirectories(strOutputDirectory + strEntryName))
            return false;
         ++uCount;
      }
      else if (entry.isFile()) // // Extract Zip entry to a file.
      {
         // to avoid copying a huge zip entry to main memory and causing a memory allocation failure
         // a buffer will be used instead !
         std::ofstream ofUnzippedFile(strOutputDirectory + strEntryName, std::ofstream::binary);
         if (ofUnzippedFile)
         {
            if (entry.readContent(ofUnzippedFile, ZipArchive::CURRENT, MAX_FILE_BUFFER) == 0)
            {
               uWrittenBytes += uSize;
               ProgressStrategy(uTotSize, uWrittenBytes);
               ++uCount;
            }
            else
            {
               ErrorStrategy("[ERROR] Encountered an error while writing : " + strOutputDirectory + strEntryName);
               //EraseFile(strOutputDirectory + strEntryName);
               continue;
            }
         }
         else
            ErrorStrategy("[ERROR] Encountered an error while creating : " + strOutputDirectory + strEntryName);

         ofUnzippedFile.close();
      }
   }
   bRes = (zf.getNbEntries() == uCount);
   zf.close();

   return bRes;
}

const bool Zip::ExtractSingleFileFromZip(const std::string& strOutDirectory, const std::string& strZipFile,
   const std::string& strZipEntry, ErrorCallback ErrorStrategy)
{
   std::string strOutputDirectory(strOutDirectory);
   if (!Directory::IsDirectory(strOutDirectory) || !Directory::IsFile(strZipFile))
      return false;

   if (strOutputDirectory.at(strOutputDirectory.size() - 1) != '/'
      && strOutputDirectory.at(strOutputDirectory.size() - 1) != '\\')
   {
      if (strOutputDirectory.find_first_of('/') != std::string::npos)
         strOutputDirectory.append("/");
      else
         strOutputDirectory.append("\\");
   }

   std::string strZipEntryParsed(strZipEntry);
   size_t uSlashPos = 0;
   if ((uSlashPos = strZipEntry.find_last_of('/')) != std::string::npos)
      strZipEntryParsed = strZipEntry.substr(uSlashPos + 1);

   bool bRes = false;
   ZipArchive zf(strZipFile);
   zf.open(ZipArchive::READ_ONLY);
   ZipEntry entry = zf.getEntry(strZipEntry);

   if (entry.isFile()) // // Extract Zip entry to a file.
   {
      // to avoid copying a huge zip entry to main memory and causing a memory allocation failure
      // a buffer will be used instead !
      std::ofstream ofUnzippedFile(strOutDirectory + strZipEntryParsed, std::ofstream::binary);
      if (ofUnzippedFile)
      {
         if (entry.readContent(ofUnzippedFile, ZipArchive::CURRENT, MAX_FILE_BUFFER) == 0)
         {
            bRes = true;
            ofUnzippedFile.close();
         }
         else
         {
            ErrorStrategy("[ERROR] Encountered an error while writing : " + strOutDirectory + strZipEntryParsed);
            //EraseFile(strOutDirectory + strZipEntryParsed);
         }
      }
   }
   zf.close();

   return bRes;
}

std::string Zip::ExtractTextFromZip(const std::string& strZipFile, const std::string& strZipEntry,
   bool& bSuccess, ErrorCallback ErrorStrategy)
{
   bSuccess = false;

   if (!Directory::IsFile(strZipFile))
      return "";

   ZipArchive zf(strZipFile);
   zf.open(ZipArchive::READ_ONLY);
   bSuccess = (zf.hasEntry(strZipEntry)) ? true : false;
   ZipEntry entry = zf.getEntry(strZipEntry);
   std::string strRes = entry.readAsText();
   zf.close();

   return strRes;
}

const bool Zip::AddDirectoryEntryToZip(const std::string& strZipFile, const std::string& strZipEntry)
{
   bool bRes = false;

   if (!Directory::IsFile(strZipFile) || strZipEntry.empty())
      return bRes;

   ZipArchive zf(strZipFile);
   zf.open(ZipArchive::WRITE);
   bRes = zf.addEntry(strZipEntry + ((strZipEntry[strZipEntry.length() - 1] == '/') ? "" : "/"));
   zf.close();

   return bRes;
}

const bool Zip::AddFileToZip(const std::string& strFile, const std::string& strZipFile,
   const std::string& strZipEntry, ErrorCallback ErrorStrategy)
{
   if (!Directory::IsFile(strZipFile) || !Directory::IsFile(strFile) || strZipEntry.empty())
      return false;

   ZipArchive zf(strZipFile);
   zf.open(ZipArchive::WRITE);

   bool bRes = true;
   if (!zf.addFile(strZipEntry, strFile))
   {
      ErrorStrategy("[ERROR] Unable to read file : " + strFile + " in Zip::AddFileToZip !");
      bRes = false;
   }
   zf.close();

   return bRes;
}

// e.g "myDir/subDir/" or "myFile.txt"
// if returned long is negative => error
const long Zip::RemoveEntryFromZip(const std::string& strZipFile, const std::string& strZipEntry)
{
   if (strZipEntry.empty() || !Directory::IsFile(strZipFile))
      return 0;

   long uDelCount = 0;

   ZipArchive zf(strZipFile);
   zf.open(ZipArchive::WRITE);

   std::string strZipEntryParsed(strZipEntry);
   // Maybe the user forgot to append a slash at the end of a directory zip entry...
   if (!zf.hasEntry(strZipEntry) && strZipEntryParsed[strZipEntryParsed.length() - 1] != '/')
      strZipEntryParsed += '/';

   uDelCount = zf.deleteEntry(strZipEntryParsed);
   zf.close();

   return uDelCount;
}

/**
 * @brief creates a folder
 *
 * parent folders must exist, otherwise an exception will be thrown
 * use Directory::CreateDirectories if some path directories 
 * don't exist
 *
 * @param strPath path where to create the new directory
 *
 * @return success of the operation
 */
const bool Directory::CreateFolder(const std::string& strPath)
{
   try
   {
      // Bug that can show up in Boost (version prior to 1.60.0) : https://svn.boost.org/trac/boost/ticket/7258
      // create_directories creates the directory but returns false, if the path contains a slash at the end of the string
      std::string strPathParsed(strPath);
      #ifndef LINUX
      if (strPathParsed[strPathParsed.length() - 1] == '\\' || strPathParsed[strPathParsed.length() - 1] == '/')
      #else
      if (strPathParsed[strPathParsed.length() - 1] == '/')
      #endif
         strPathParsed.erase(strPathParsed.length() - 1);

      fs::path PathDir(strPathParsed);
      if (fs::exists(PathDir) || fs::create_directory(PathDir))
         return true;
   }
   catch (const fs::filesystem_error& ex)
   {
      std::cout << ex.what() << std::endl;
      //throw; // double throw : not necessary
   }
   return false;
}

/**
* @brief creates a full path
*
* missing directories will be automatically created
*
* parent folders must exist, otherwise an exception will be thrown
*
* @param path to create the new directory
*
* @return success of the operation
*/
const bool Directory::CreateDirectories(const std::string& strPath)
{
   try
   {
      // Bug that can show up in Boost (version prior to 1.60.0) : https://svn.boost.org/trac/boost/ticket/7258
      // create_directories creates the directory but returns false, if the path contains a slash at the end of the string
      std::string strPathParsed(strPath);
      #ifndef LINUX
      if (strPathParsed[strPathParsed.length() - 1] == '\\' || strPathParsed[strPathParsed.length() - 1] == '/')
      #else
      if (strPathParsed[strPathParsed.length() - 1] == '/')
      #endif
         strPathParsed.erase(strPathParsed.length() - 1);

      fs::path PathDir(strPathParsed);
      if (fs::exists(PathDir) || fs::create_directories(PathDir))
         return true;
   }
   catch (const fs::filesystem_error& ex)
   {
      std::cout << ex.what() << std::endl;
   }  
   return false;
}

/**
 * @brief checks if the provided path is a directory
 *
 * @param file system path
 *
 * @return boolean
 */
const bool Directory::IsDirectory(const std::string& strPath)
{
   try
   {
      fs::path PathDir(strPath);
      if (fs::exists(PathDir) && fs::is_directory(PathDir))
         return true;
   }
   catch (const fs::filesystem_error& ex)
   {
      std::cout << ex.what() << std::endl;
   }
   return false;
}

/**
* @brief checks if the provided path is a file
*
* @param file system path
*
* @return boolean
*/
const bool Directory::IsFile(const std::string& strPath)
{
   try
   {
      fs::path PathFile(strPath);
      if (fs::exists(PathFile) && fs::is_regular_file(PathFile))
         return true;
   }
   catch (const fs::filesystem_error& ex)
   {
      std::cout << ex.what() << std::endl;
   }
   return false;
}

std::string Directory::RemoveFileName(const std::string& strPath)
{
   fs::path PathFile(strPath);
   std::string strDir(PathFile.make_preferred().remove_filename().string());
   if (strDir.find_first_of('/') != std::string::npos)
      strDir.append("/");
   else
      strDir.append("\\");
   return strDir;
}

// If exists(symlink_status(p,ec)), it is removed as if by POSIX remove().
// Postcondition: !exists(symlink_status(p)).
// Returns:  false if p did not exist in the first place, otherwise true.
const bool Directory::EraseFile(const std::string& strFilePath)
{
   try
   {
      return fs::remove(strFilePath);
   }
   catch (const fs::filesystem_error& ex)
   {
      std::cout << ex.what() << std::endl;
   }
   return false;
}

// Effects:  Recursively deletes the contents of p if it exists, then deletes file p itself, as if by POSIX
// Postcondition: !exists(p)
// Returns: The number of files removed.
const size_t Directory::EraseFolder(const std::string& strFolderPath, bool& bSuccess)
{
   uintmax_t uDeletedItems = 0;
   bSuccess = true;
   try
   {
      uDeletedItems = fs::remove_all(strFolderPath);
   }
   catch (const fs::filesystem_error& ex)
   {
      bSuccess = false;
      std::cout << ex.what() << std::endl;
   }
   return uDeletedItems;
}

// Effects: Renames old_p to new_p, as if by POSIX rename().
const bool Directory::Rename(const std::string& strFilePath, const std::string& strNewName)
{
   try
   {
      fs::rename(strFilePath, strNewName);
      return true;
   }
   catch (const fs::filesystem_error& ex)
   {
      std::cout << ex.what() << std::endl;
   }
   return false;
}

const size_t Directory::FileSize(const std::string& strFile, bool& bSuccess)
{
   size_t usCount = 0;
   bSuccess = false;
   try
   {
      if (IsFile(strFile))
      {
         usCount = boost::filesystem::file_size(strFile);
         bSuccess = true;
      }
   }
   catch (const fs::filesystem_error& ex)
   {
      std::cout << ex.what() << std::endl;
   }
   return usCount;
}

// Returns: The time of last data modification of p, determined as if by the value of the POSIX stat structure member st_mtime
// obtained as if by POSIX stat().
std::time_t Directory::GetLastWriteTime(const std::string& strFilePath)
{
   return fs::last_write_time(strFilePath);
}

const size_t Directory::CleanUpFiles(const std::string& strDirectory, const size_t& usKeepDays, const bool& bRecursive)
{
   if (!IsDirectory(strDirectory))
      return 0;

   size_t usCount = 0;
   Directory DirList;
   DirList.ListFiles(strDirectory, bRecursive);

   for (std::unordered_map<std::string, std::string>::const_iterator itFile = DirList.m_mapFilesAbsRel.cbegin();
   itFile != DirList.m_mapFilesAbsRel.cend();
   ++itFile)
   {
      std::time_t tLastWriteTime = GetLastWriteTime(itFile->first);
      if (tLastWriteTime < static_cast<std::time_t>(std::time(nullptr) - usKeepDays * 86400))
      {
         if (!EraseFile(itFile->first))
            std::cerr << "[ERROR][Directory::CleanUpFiles] File '" << itFile->first << "' could not be deleted." << std::endl;
         else
            ++usCount;
      }
   }
   return usCount;
}

/**
 * @brief lists all the folders of a directory
 *
 * @param path of the directory
 *
 * @return string containing folders listing
 */
std::string Directory::ListFolders(const std::string& strLocation, bool bRecursive, PathType ePathType)
{
   std::string strRelativePath;
   std::string strAbsolutePath;
   std::string strList;
   fs::path PathDir(strLocation);
   std::string strLoc = PathDir.make_preferred().string();

   m_mapFoldersRelAbs.clear();
   m_mapFoldersAbsRel.clear();
   m_mapSortedFoldersAbsRel.clear();
   m_mapSortedFoldersRelAbs.clear();

   if (strLoc.empty() || !IsDirectory(strLocation))
      return "";
   
   if (bRecursive)
   {
      for (fs::recursive_directory_iterator itDir(PathDir), itEnd; itDir != itEnd; ++itDir)
      {
         if (fs::is_directory(*itDir))
         {
            strAbsolutePath = itDir->path().string();
            strRelativePath = itDir->path().string();
            //strRelativePath.erase(0, strLoc.length() - 1); // -1 to spare the '/' or the '\'
            strRelativePath.erase(0, strLoc.length());

            #ifdef STD_RELATIVE_PATH
            if (strRelativePath.find_first_of('\\') != std::string::npos) // only for paths under a Windows based system
            { // to transform the path to a standardized one in order to compare it with the distant paths (NetExplorer platform)
               boost::replace_all(strRelativePath, "\\", "/");
            }
            #endif

            m_mapFoldersAbsRel.insert(std::pair<std::string, std::string>(strAbsolutePath, strRelativePath));
            m_mapSortedFoldersAbsRel.insert(std::pair<std::string, std::string>(strAbsolutePath, strRelativePath));
            m_mapFoldersRelAbs.insert(std::pair<std::string, std::string>(strRelativePath, strAbsolutePath));
            m_mapSortedFoldersRelAbs.insert(std::pair<std::string, std::string>(strRelativePath, strAbsolutePath));

            switch (ePathType)
            {
               case ABSOLUTE_PATH:
               default:
                  strList.append(strAbsolutePath);
                  strList.append("\n");
                  break;
               case RELATIVE_PATH:
                  strList.append(strRelativePath);
                  strList.append("\n");
                  break;
            }
         }
      }
   }
   else
   {
      for (fs::directory_iterator itDir(PathDir), itEnd; itDir != itEnd; ++itDir)
      {
         if (fs::is_directory(*itDir))
         {
            strAbsolutePath = itDir->path().string();
            strRelativePath = itDir->path().string();
            //strRelativePath.erase(0, strLoc.length() - 1); // -1 to spare the '/' or the '\'
            strRelativePath.erase(0, strLoc.length());


            #ifdef STD_RELATIVE_PATH
            // transform the path to a standardized one in order to compare it with the distant paths for example.
            if (strRelativePath.find_first_of('\\') != std::string::npos) // only for paths under a Windows based system
            {
               boost::replace_all(strRelativePath, "\\", "/");
            }
            #endif

            m_mapFoldersAbsRel.insert(std::pair<std::string, std::string>(strAbsolutePath, strRelativePath));
            m_mapSortedFoldersAbsRel.insert(std::pair<std::string, std::string>(strAbsolutePath, strRelativePath));
            m_mapFoldersRelAbs.insert(std::pair<std::string, std::string>(strRelativePath, strAbsolutePath));
            m_mapSortedFoldersRelAbs.insert(std::pair<std::string, std::string>(strRelativePath, strAbsolutePath));

            switch (ePathType)
            {
               case ABSOLUTE_PATH:
               default:
                  strList.append(strAbsolutePath);
                  strList.append("\n");
                  break;
               case RELATIVE_PATH:
                  strList.append(strRelativePath);
                  strList.append("\n");
                  break;
            }
         }
      }
   }
	return strList;
}

/**
 * @brief lists all the files of a directory
 *
 * @param path of the directory
 *
 * @return string containing files listing
 */
std::string Directory::ListFiles(const std::string& strLocation, bool bRecursive, PathType ePathType)
{
   std::string strRelativePath;
   std::string strAbsolutePath;
   std::string strList;
   fs::path PathDir(strLocation);
   std::string strLoc = PathDir.make_preferred().string();

   m_mapFilesRelAbs.clear();
   m_mapFilesAbsRel.clear();
   m_mapSortedFilesAbsRel.clear();
   m_mapSortedFilesRelAbs.clear();

   if (strLoc.empty() || !IsDirectory(strLocation))
      return "";

   if (bRecursive)
   {
      for (fs::recursive_directory_iterator itDir(PathDir), itEnd; itDir != itEnd; ++itDir)
      {
         if (fs::is_regular_file(*itDir))
         {
            strAbsolutePath = itDir->path().string();
            strRelativePath = itDir->path().string();
            
            if (strLoc[strLoc.length() - 1] == '/' // It occured only once on Ubuntu, but let's stay careful...
               || strLoc[strLoc.length() - 1] == '\\') // Never occur under Windows 10 with Boost 1.60.0
               strRelativePath.erase(0, strLoc.length()); // +1 to remove the slash preceding the file name
            else
               strRelativePath.erase(0, strLoc.length() + 1); // +1 to remove the slash preceding the file name

            #ifdef STD_RELATIVE_PATH
            if (strRelativePath.find_first_of('\\') != std::string::npos) // only for paths under a Windows based system
            { // to transform the path to a standardized one in order to compare it with the distant paths (NetExplorer platform)
               boost::replace_all(strRelativePath, "\\", "/");
            }
            #endif

            m_mapFilesAbsRel.insert(std::pair<std::string, std::string>(strAbsolutePath, strRelativePath));
            m_mapSortedFilesAbsRel.insert(std::pair<std::string, std::string>(strAbsolutePath, strRelativePath));
            m_mapFilesRelAbs.insert(std::pair<std::string, std::string>(strRelativePath, strAbsolutePath));
            m_mapSortedFilesRelAbs.insert(std::pair<std::string, std::string>(strRelativePath, strAbsolutePath));

            switch (ePathType)
            {
               case ABSOLUTE_PATH:
               default:
                  strList.append(strAbsolutePath);
                  strList.append("\n");
                  break;
               case RELATIVE_PATH:
                  strList.append(strRelativePath);
                  strList.append("\n");
                  break;
            }
         }
      }
   }
   else
   {
      for (fs::directory_iterator itDir(PathDir), itEnd; itDir != itEnd; ++itDir)
      {
         if (fs::is_regular_file(*itDir))
         {
            strAbsolutePath = itDir->path().string();
            strRelativePath = itDir->path().string();
            if (strLoc[strLoc.length() - 1] == '/' // Ubuntu case
               || strLoc[strLoc.length() - 1] == '\\') // Didn't occur under Windows 10 with Boost 1.60.0 but let's be careful
               strRelativePath.erase(0, strLoc.length()); // +1 to remove the slash preceding the file name
            else
               strRelativePath.erase(0, strLoc.length() + 1); // +1 to remove the slash preceding the file name

            #ifdef STD_RELATIVE_PATH
            if (strRelativePath.find_first_of('\\') != std::string::npos) // only for paths under a Windows based system
            { // to transform the path to a standardized one in order to compare it with the distant paths (NetExplorer platform)
               boost::replace_all(strRelativePath, "\\", "/");
            }
            #endif

            m_mapFilesAbsRel.insert(std::pair<std::string, std::string>(strAbsolutePath, strRelativePath));
            m_mapSortedFilesAbsRel.insert(std::pair<std::string, std::string>(strAbsolutePath, strRelativePath));
            m_mapFilesRelAbs.insert(std::pair<std::string, std::string>(strRelativePath, strAbsolutePath));
            m_mapSortedFilesRelAbs.insert(std::pair<std::string, std::string>(strRelativePath, strAbsolutePath));

            switch (ePathType)
            {
               case ABSOLUTE_PATH:
               default:
                  strList.append(strAbsolutePath);
                  strList.append("\n");
                  break;
               case RELATIVE_PATH:
                  strList.append(strRelativePath);
                  strList.append("\n");
                  break;
            }
         }
      }
   }
   return strList;
}
