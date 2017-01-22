#include "test_utils.h"

// Test configuration constants (to be loaded from an INI file)
std::string TEST_FOLDER;
std::string TEST_FOLDER_1;
std::string TEST_FOLDER_2;
std::string TEST_FOLDER_3;
std::string TEST_FOLDER_4;
std::string TEST_FOLDER_5;
std::string TEST_FOLDER_6;
std::string TEST_FOLDER_7;
std::string TEST_FOLDER_8;
std::string TEST_FILE;
std::string TEST_FILE_A;
std::string TEST_FILE_B;
std::string TEST_FILE_DEL;
std::string TEST_ZIPFILE;
std::string TEST_ZIPOUTPUT;
std::string TEST_ZIPENTRY;
long        TEST_ZIPFILECOUNT;
std::string TEST_ZIPTEXT;
std::string TEST_FOLDER_DEL;
std::string TEST_CLEANUP;
std::string TEST_ZIPFILE_REMOVE;

bool GlobalTestInit(const std::string& strConfFile)
{
   // Reading test env. from INI file....
   CSimpleIniA ini;
   ini.LoadFile(strConfFile.c_str());

   TEST_FOLDER = ini.GetValue("test-folders", "test_folder", "");
   std::cout << "test_folder = " << TEST_FOLDER << std::endl;
   TEST_FOLDER_1 = ini.GetValue("test-folders", "test_folder_1", "");
   std::cout << "test_folder_1 = " << TEST_FOLDER_1 << std::endl;
   TEST_FOLDER_2 = ini.GetValue("test-folders", "test_folder_2", "");
   std::cout << "test_folder_2 = " << TEST_FOLDER_2 << std::endl;
   TEST_FOLDER_3 = ini.GetValue("test-folders", "test_folder_3", "");
   std::cout << "test_folder_3 = " << TEST_FOLDER_3 << std::endl;
   TEST_FOLDER_4 = ini.GetValue("test-folders", "test_folder_4", "");
   std::cout << "test_folder_4 = " << TEST_FOLDER_4 << std::endl;
   TEST_FOLDER_5 = ini.GetValue("test-folders", "test_folder_5", "");
   std::cout << "test_folder_5 = " << TEST_FOLDER_5 << std::endl;
   TEST_FOLDER_6 = ini.GetValue("test-folders", "test_folder_6", "");
   std::cout << "test_folder_6 = " << TEST_FOLDER_6 << std::endl;
   TEST_FOLDER_7 = ini.GetValue("test-folders", "test_folder_7", "");
   std::cout << "test_folder_7 = " << TEST_FOLDER_7 << std::endl;
   TEST_FOLDER_8 = ini.GetValue("test-folders", "test_folder_8", "");
   std::cout << "test_folder_8 = " << TEST_FOLDER_8 << std::endl;
   TEST_FILE = ini.GetValue("test-files", "test_file", "");
   std::cout << "test_file = " << TEST_FILE << std::endl;
   TEST_FILE_A = ini.GetValue("test-files", "test_file_a", "");
   std::cout << "test_file_a = " << TEST_FILE_A << std::endl;
   TEST_FILE_B = ini.GetValue("test-files", "test_file_b", "");
   std::cout << "test_file_b = " << TEST_FILE_B << std::endl;
   TEST_FILE_DEL = ini.GetValue("test-del", "test_del_file", "");
   std::cout << "test_del_file = " << TEST_FILE_DEL << std::endl;
   TEST_FOLDER_DEL = ini.GetValue("test-del", "test_del_folder", "");
   std::cout << "test_del_folder = " << TEST_FOLDER_DEL << std::endl;
   TEST_ZIPFILE = ini.GetValue("test-zip", "test_zip_file", "");
   std::cout << "test_zip_file = " << TEST_ZIPFILE << std::endl;
   TEST_ZIPFILECOUNT = atoi(ini.GetValue("test-zip", "test_zip_file_count", ""));
   std::cout << "test_zip_file_count = " << TEST_ZIPFILECOUNT << std::endl;
   TEST_ZIPENTRY = ini.GetValue("test-zip", "test_zip_entry", "");
   std::cout << "test_zip_entry = " << TEST_ZIPENTRY << std::endl;
   TEST_ZIPOUTPUT = ini.GetValue("test-zip", "test_zip_output", "");
   std::cout << "test_zip_output = " << TEST_ZIPOUTPUT << std::endl;
   TEST_ZIPFILE_REMOVE = ini.GetValue("test-zip", "test_remove_zip_file", "");
   std::cout << "test_remove_zip_file = " << TEST_ZIPFILE_REMOVE << std::endl;
   TEST_ZIPTEXT = ini.GetValue("test-zip", "test_zip_text", "");
   std::cout << "test_zip_text = " << TEST_ZIPTEXT << std::endl;
   TEST_CLEANUP = ini.GetValue("test-cleanup", "test_cleanup_folder", "");
   std::cout << "test_cleanup_folder = " << TEST_CLEANUP << std::endl;

   // some checkings to avoid stupid things like erasing your home directory...
   if (TEST_FOLDER.compare("/") == 0 || TEST_FOLDER.length() < 4)
      std::cout << "[ERROR] Check \"test-folders\" parameter !" << std::endl;

#ifdef LINUX
   if ( TEST_FOLDER[0] == '~'
      || (TEST_FOLDER.compare(0, 5, "/home") == 0
      && std::count(TEST_FOLDER.cbegin(), TEST_FOLDER.cend(), '/') == 2
      || std::count(TEST_FOLDER.cbegin(), TEST_FOLDER.cend(), '/') == 3 && TEST_FOLDER[TEST_FOLDER.length() - 1] == '/'))
   {
      std::cout << "[ERROR] Check \"test-folders\" parameter !" << std::endl;
      return false;
   }
#endif

   if (TEST_FOLDER.empty()
      || TEST_FOLDER_1.empty()
      || TEST_FOLDER_2.empty()
      || TEST_FOLDER_3.empty()
      || TEST_FOLDER_4.empty()
      || TEST_FOLDER_5.empty()
      || TEST_FOLDER_6.empty()
      || TEST_FOLDER_7.empty()
      || TEST_FOLDER_8.empty()
      || TEST_FILE.empty()
      || TEST_FILE_A.empty()
      || TEST_FILE_B.empty()
      || TEST_FILE_DEL.empty()
      || TEST_ZIPFILE.empty()
      || TEST_ZIPENTRY.empty()
      || TEST_ZIPOUTPUT.empty()
      || TEST_ZIPFILE_REMOVE.empty()
      || TEST_ZIPTEXT.empty()
      || TEST_FOLDER_DEL.empty()
      || TEST_CLEANUP.empty()
      || TEST_ZIPFILECOUNT <= 0)
   {
      std::cout << "[CRITIC] Missing test parameters ! Aborting tests.";
      return false;
   }

   return true;
}

void GlobalTestCleanUp(void)
{

   return;
}

void TestZipErrorLogger(const std::string& strMsg)
{
   std::cout << strMsg << std::endl;
}

int TestZipProgressCallback(double dFileSize, double dBytesCount)
{
   if (dFileSize <= 0.0)
      return 0;

   // how wide you want the progress meter to be
   const int iTotalDots = 20;
   double dFractionProcessed = dBytesCount / dFileSize;
   // part of the progressmeter that's already "full"
   int iDots = round(dFractionProcessed * iTotalDots);

   // create the "meter"
   int iDot = 0;
   std::cout << static_cast<unsigned>(dFractionProcessed * 100) << "% [";

   // part  that's full already
   for (; iDot < iDots; iDot++)
      std::cout << "=";

   // remaining part (spaces)
   for (; iDot < iTotalDots; iDot++)
      std::cout << " ";

   // and back to line begin - do not forget the fflush to avoid output buffering problems!
   std::cout << "]           \r" << std::flush;

   return dFractionProcessed;
}

std::string GetTimeStamp()
{
   // Notice the use of a stringstream, yet another useful stream medium!
   // to increase performances we can use sprintf or miloyip itoa fast imp.
   std::ostringstream stream;
   std::time_t tRawTime;
   std::tm* tmTimeInfo;

   time(&tRawTime);
   tmTimeInfo = localtime(&tRawTime);

   stream << (tmTimeInfo->tm_year) + 1900 << std::setfill('0') << std::setw(2) << tmTimeInfo->tm_mon
      << tmTimeInfo->tm_mday << '_' << tmTimeInfo->tm_hour
      << tmTimeInfo->tm_min << tmTimeInfo->tm_sec;
   // The str() function of output stringstreams return a std::string.
   return stream.str();
}

bool GetFileTime(const char* const & pszFilePath, time_t& tLastModificationTime)
{
   FILE* pFile = fopen(pszFilePath, "rb");
   if (pFile != nullptr)
   {
      struct stat file_info;
#ifndef LINUX
      if (fstat(_fileno(pFile), &file_info) == 0)
#else
      if (fstat(fileno(pFile), &file_info) == 0)
#endif
      {
         tLastModificationTime = file_info.st_mtime;
         return true;
      }
   }
   return false;
}
