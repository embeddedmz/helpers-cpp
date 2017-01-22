#include "test_utils.h"

// Test parameters
extern std::string TEST_FOLDER;
extern std::string TEST_FOLDER_1;
extern std::string TEST_FOLDER_2;
extern std::string TEST_FOLDER_3;
extern std::string TEST_FOLDER_4;
extern std::string TEST_FOLDER_5;
extern std::string TEST_FOLDER_6;
extern std::string TEST_FOLDER_7;
extern std::string TEST_FOLDER_8;
extern std::string TEST_FILE;
extern std::string TEST_FILE_A;
extern std::string TEST_FILE_B;
extern std::string TEST_FILE_DEL;
extern std::string TEST_ZIPFILE;
extern std::string TEST_ZIPOUTPUT;
extern std::string TEST_ZIPENTRY;
extern long TEST_ZIPFILECOUNT;
extern std::string TEST_ZIPTEXT;
extern std::string TEST_FOLDER_DEL;
extern std::string TEST_CLEANUP;
extern std::string TEST_ZIPFILE_REMOVE;

namespace
{
class HelpersTest : public ::testing::Test
{
protected:

   HelpersTest()
   {
   }

   virtual ~HelpersTest()
   {
   }

   virtual void SetUp()
   {

   }

   virtual void TearDown()
   {
   }
   
   Directory m_oDirectory;
};

   // Unit tests

TEST_F(HelpersTest, CreateFolder)
{
   EXPECT_TRUE(Directory::CreateFolder(TEST_FOLDER_1));
   EXPECT_TRUE(Directory::CreateFolder(TEST_FOLDER));

   // Check if path's separator is not system related
   EXPECT_TRUE(Directory::CreateFolder(TEST_FOLDER_2));
   EXPECT_TRUE(Directory::CreateFolder(TEST_FOLDER_3));
   EXPECT_TRUE(Directory::CreateFolder(TEST_FOLDER_4));
   EXPECT_TRUE(Directory::CreateFolder(TEST_FOLDER_5));
   EXPECT_TRUE(Directory::CreateFolder(TEST_FOLDER_6));
   EXPECT_TRUE(Directory::CreateDirectories(TEST_FOLDER_7));

   // check for failure
   EXPECT_FALSE(Directory::CreateFolder(TEST_FOLDER_8));
}

TEST_F(HelpersTest, DirectoryCheck)
{
   EXPECT_TRUE(Directory::IsDirectory(TEST_FOLDER_2));

   // check for failure
   EXPECT_FALSE(Directory::IsDirectory(TEST_FOLDER_3 + "H"));
   EXPECT_FALSE(Directory::IsDirectory(TEST_FOLDER_3 + TEST_FILE));
}

TEST_F(HelpersTest, FileCheck)
{
   EXPECT_TRUE(Directory::IsFile(TEST_FOLDER_3 + TEST_FILE));

   // check for failure
   EXPECT_FALSE(Directory::IsFile(TEST_FOLDER_3 + "D"));
   EXPECT_FALSE(Directory::IsFile(TEST_FOLDER_3 + "inexistant.dat"));
}

TEST_F(HelpersTest, DirectoryList)
{
   std::string strResult = m_oDirectory.ListFolders(TEST_FOLDER_3);
   EXPECT_FALSE(strResult.empty());
   EXPECT_EQ(m_oDirectory.GetMapFoldersAbsRel().size(),
             m_oDirectory.GetMapSortedFoldersAbsRel().size());
   EXPECT_EQ(m_oDirectory.GetMapFoldersRelAbs().size(),
      m_oDirectory.GetMapSortedFoldersRelAbs().size());
   EXPECT_EQ(m_oDirectory.GetMapFoldersAbsRel().size(),
      m_oDirectory.GetMapFoldersRelAbs().size());

   EXPECT_TRUE( ::AreMapsEqual(m_oDirectory.GetMapSortedFoldersAbsRel(),
                            m_oDirectory.GetMapFoldersAbsRel() ));
   EXPECT_TRUE(::AreMapsEqual(m_oDirectory.GetMapSortedFoldersRelAbs(),
      m_oDirectory.GetMapFoldersRelAbs()));

}
TEST_F(HelpersTest, DirectoryListRecursively)
{
   std::string strResult = m_oDirectory.ListFolders(TEST_FOLDER, true);
   EXPECT_FALSE(strResult.empty());
   EXPECT_EQ(m_oDirectory.GetMapFoldersAbsRel().size(),
      m_oDirectory.GetMapSortedFoldersAbsRel().size());
   EXPECT_EQ(m_oDirectory.GetMapFoldersRelAbs().size(),
      m_oDirectory.GetMapSortedFoldersRelAbs().size());
   EXPECT_EQ(m_oDirectory.GetMapFoldersAbsRel().size(),
      m_oDirectory.GetMapFoldersRelAbs().size());

   EXPECT_TRUE(::AreMapsEqual(m_oDirectory.GetMapSortedFoldersAbsRel(),
      m_oDirectory.GetMapFoldersAbsRel()));
   EXPECT_TRUE(::AreMapsEqual(m_oDirectory.GetMapSortedFoldersRelAbs(),
      m_oDirectory.GetMapFoldersRelAbs()));
}

TEST_F(HelpersTest, FilesList)
{
   std::string strResult = m_oDirectory.ListFiles(TEST_FOLDER_1);
   EXPECT_FALSE(strResult.empty());
   EXPECT_EQ(m_oDirectory.GetMapFilesAbsRel().size(),
      m_oDirectory.GetMapSortedFilesAbsRel().size());
   EXPECT_EQ(m_oDirectory.GetMapFilesRelAbs().size(),
      m_oDirectory.GetMapSortedFilesRelAbs().size());
   EXPECT_EQ(m_oDirectory.GetMapFilesAbsRel().size(),
      m_oDirectory.GetMapFilesRelAbs().size());

   EXPECT_TRUE(::AreMapsEqual(m_oDirectory.GetMapSortedFilesAbsRel(),
      m_oDirectory.GetMapFilesAbsRel()));
   EXPECT_TRUE(::AreMapsEqual(m_oDirectory.GetMapSortedFilesRelAbs(),
      m_oDirectory.GetMapFilesRelAbs()));
}

TEST_F(HelpersTest, FilesListRecursively)
{
   std::string strResult = m_oDirectory.ListFiles(TEST_FOLDER_2, true);
   EXPECT_FALSE(strResult.empty());
   EXPECT_EQ(m_oDirectory.GetMapFilesAbsRel().size(),
      m_oDirectory.GetMapSortedFilesAbsRel().size());
   EXPECT_EQ(m_oDirectory.GetMapFilesRelAbs().size(),
      m_oDirectory.GetMapSortedFilesRelAbs().size());
   EXPECT_EQ(m_oDirectory.GetMapFilesAbsRel().size(),
      m_oDirectory.GetMapFilesRelAbs().size());

   EXPECT_TRUE(::AreMapsEqual(m_oDirectory.GetMapSortedFilesAbsRel(),
      m_oDirectory.GetMapFilesAbsRel()));
   EXPECT_TRUE(::AreMapsEqual(m_oDirectory.GetMapSortedFilesRelAbs(),
      m_oDirectory.GetMapFilesRelAbs()));
}

TEST_F(HelpersTest, ExtractFileDir)
{
   std::string strFilePath = TEST_FOLDER + TEST_FILE;
   std::string strResultFileName = Directory::RemoveFileName(strFilePath);
   EXPECT_STREQ(TEST_FOLDER.c_str(), strResultFileName.c_str());
}

TEST_F(HelpersTest, RenameFile)
{
   if (Directory::IsFile(TEST_FOLDER + TEST_FILE_A))
   {
      ASSERT_TRUE(Directory::Rename(TEST_FOLDER + TEST_FILE_A, TEST_FOLDER + TEST_FILE_B));
      EXPECT_TRUE(Directory::IsFile(TEST_FOLDER + TEST_FILE_B));
      EXPECT_FALSE(Directory::IsFile(TEST_FOLDER + TEST_FILE_A));
   }
   else if (Directory::IsFile(TEST_FOLDER + TEST_FILE_B))
   {
      ASSERT_TRUE(Directory::Rename(TEST_FOLDER + TEST_FILE_B, TEST_FOLDER + TEST_FILE_A));
      EXPECT_TRUE(Directory::IsFile(TEST_FOLDER + TEST_FILE_A));
      EXPECT_FALSE(Directory::IsFile(TEST_FOLDER + TEST_FILE_B));
   }
   else
      ASSERT_TRUE(false); // External test environment problem
}

// check for failure
TEST_F(HelpersTest, RenameInexistantFile)
{
   ASSERT_FALSE(Directory::Rename(TEST_FOLDER + "inexistant_file.txt", TEST_FOLDER + "new_name.txt"));
}

TEST_F(HelpersTest, DeleteExistingFile)
{
   // Create a dummy file before performing this test
   if (!Directory::IsFile(TEST_FOLDER + TEST_FILE_DEL))
   {
      std::ofstream ofsDummy(TEST_FOLDER + TEST_FILE_DEL);
      ASSERT_TRUE(static_cast<bool>(ofsDummy));
      ofsDummy << "Dummy file for test purposes...." << std::endl;
      ASSERT_TRUE(static_cast<bool>(ofsDummy));
   }
   ASSERT_TRUE(Directory::IsFile(TEST_FOLDER + TEST_FILE_DEL));

   EXPECT_TRUE(Directory::EraseFile(TEST_FOLDER + TEST_FILE_DEL));
   EXPECT_FALSE(Directory::IsFile(TEST_FOLDER + TEST_FILE_DEL));
}

// check for failure
TEST_F(HelpersTest, DeleteNonExistentFile)
{
   EXPECT_FALSE(Directory::EraseFile(TEST_FOLDER + "inexistant_file.txt"));
}

TEST_F(HelpersTest, DeleteDirs)
{
   // Create a dummy foler before performing this test
   if (!Directory::IsDirectory(TEST_FOLDER + TEST_FOLDER_DEL))
   {
      ASSERT_TRUE(Directory::CreateFolder(TEST_FOLDER + TEST_FOLDER_DEL));
   }
   ASSERT_TRUE(Directory::IsDirectory(TEST_FOLDER + TEST_FOLDER_DEL));

   // Copy some files into the dummy folder and check if they are correctly copied

   // Delete the dummy folder

   bool bSuccess = false;
   size_t usCount = Directory::EraseFolder(TEST_FOLDER + TEST_FOLDER_DEL, bSuccess);
   EXPECT_TRUE(bSuccess);
   EXPECT_EQ(1, usCount); // TODO : to be fixed according to the number of files present + the folder itself
}

// check for failure
TEST_F(HelpersTest, DeleteInexistantFolder)
{
   bool bSuccess = false;
   size_t usCount = Directory::EraseFolder(TEST_FOLDER + TEST_FOLDER_DEL, bSuccess);
   EXPECT_TRUE(bSuccess);
   EXPECT_EQ(0, usCount);
}

TEST_F(HelpersTest, GetLastWriteTime)
{
   ASSERT_TRUE(Directory::IsFile(TEST_FOLDER_3 + TEST_FILE));

   std::time_t tExpected;
   ASSERT_TRUE(GetFileTime((TEST_FOLDER_3 + TEST_FILE).c_str(), tExpected));
   EXPECT_EQ(tExpected, Directory::GetLastWriteTime(TEST_FOLDER_3 + TEST_FILE));
}

TEST_F(HelpersTest, GetFileSize)
{
   struct stat file_info;
   bool bRes = false;
   long lExpectedSize = 0;

   ASSERT_TRUE(Directory::IsFile(TEST_FOLDER + TEST_FILE));
   ASSERT_EQ(0, stat((TEST_FOLDER + TEST_FILE).c_str(), &file_info));
   lExpectedSize = file_info.st_size;

   EXPECT_EQ(lExpectedSize, Directory::FileSize(TEST_FOLDER_3 + TEST_FILE, bRes));
   EXPECT_TRUE(bRes);
}

TEST_F(HelpersTest, ExtractFileDirAndCheckForSlash)
{
   std::string strFilePath = TEST_FOLDER + TEST_FILE;
   std::string strResultFileName = Directory::RemoveFileName(strFilePath);
   EXPECT_STREQ(TEST_FOLDER.c_str(), strResultFileName.c_str());
   
   ASSERT_TRUE(strResultFileName.at(strResultFileName.size() - 1) == '/' || strResultFileName.at(strResultFileName.size() - 1) == '\\');
}

TEST_F(HelpersTest, ExtractZipFileAndCleanUp)
{
   size_t uUnzippedFilesCount = 0;
   EXPECT_TRUE(Zip::ExtractAllFilesFromZip(TEST_FOLDER + TEST_CLEANUP, TEST_FOLDER + TEST_ZIPFILE, uUnzippedFilesCount,
      TestZipProgressCallback, TestZipErrorLogger));
   std::cout << std::endl;
   EXPECT_EQ(TEST_ZIPFILECOUNT, uUnzippedFilesCount);
   
   size_t uCleanedUpCount = Directory::CleanUpFiles(TEST_FOLDER + TEST_CLEANUP, 1, true); // clean files older than a day from now
   EXPECT_LE(uCleanedUpCount, TEST_ZIPFILECOUNT); // should not exceed TEST_ZIPFILECOUNT
}

// Check for failure
TEST_F(HelpersTest, ExtractInexistentZipFile)
{
   size_t uUnzippedFilesCount = 0;
   EXPECT_FALSE(Zip::ExtractAllFilesFromZip(TEST_FOLDER + TEST_CLEANUP, TEST_FOLDER + "foobar.xxx.zip", uUnzippedFilesCount,
      TestZipProgressCallback, TestZipErrorLogger));
   EXPECT_EQ(0, uUnzippedFilesCount);

   size_t uCleanedUpCount = Directory::CleanUpFiles(TEST_FOLDER + "Inexistent_Folder", 1, true);
   EXPECT_EQ(0, uCleanedUpCount);
}

TEST_F(HelpersTest, ExtractSingleFileFromZip)
{
   EXPECT_TRUE(Zip::ExtractSingleFileFromZip(TEST_FOLDER, TEST_FOLDER + TEST_ZIPFILE, TEST_ZIPENTRY, TestZipErrorLogger));
   EXPECT_TRUE(Directory::IsFile(TEST_FOLDER + TEST_ZIPOUTPUT));
   
   bool bRes = false;
   EXPECT_GT(Directory::FileSize(TEST_FOLDER + TEST_ZIPOUTPUT, bRes), 0);
   EXPECT_TRUE(bRes);
}

// check for failure
TEST_F(HelpersTest, ExtractInexistentEntryFromZip)
{
   EXPECT_FALSE(Zip::ExtractSingleFileFromZip(TEST_FOLDER + "inexistent_foobar.xxx", TEST_FOLDER + TEST_ZIPFILE, "inexistent_foobar.xxx",
      TestZipErrorLogger));
   EXPECT_FALSE(Directory::IsFile(TEST_FOLDER + "inexistent_foobar.xxx"));
}

TEST_F(HelpersTest, AddAndRemoveEntriesFromZip)
{
   // Create an empty zip file if the latter was deleted by RemoveEntryFromZip when it became empty...
   if (!Directory::IsFile(TEST_FOLDER + TEST_ZIPFILE_REMOVE))
      std::ofstream ofZipTest(TEST_FOLDER + TEST_ZIPFILE_REMOVE);

   // Add a dummy dir. and file entries
   ASSERT_TRUE(Zip::AddDirectoryEntryToZip(TEST_FOLDER + TEST_ZIPFILE_REMOVE, "Dummy_Dir"));
   ASSERT_TRUE(Zip::AddDirectoryEntryToZip(TEST_FOLDER + TEST_ZIPFILE_REMOVE, "Dummy_Dir2/"));
   ASSERT_TRUE(Zip::AddFileToZip(TEST_FOLDER + TEST_FILE, TEST_FOLDER + TEST_ZIPFILE_REMOVE, "Dummy_File.txt", TestZipErrorLogger));

   EXPECT_EQ(Zip::RemoveEntryFromZip(TEST_FOLDER + TEST_ZIPFILE_REMOVE, "Dummy_Dir"), 1);
   EXPECT_EQ(Zip::RemoveEntryFromZip(TEST_FOLDER + TEST_ZIPFILE_REMOVE, "Dummy_Dir2/"), 1);
   EXPECT_EQ(Zip::RemoveEntryFromZip(TEST_FOLDER + TEST_ZIPFILE_REMOVE, "Dummy_File.txt"), 1);
}

// check for failure
TEST_F(HelpersTest, AddInexistentFileToZip)
{
   ASSERT_FALSE(Zip::AddFileToZip(TEST_FOLDER + "inexistent_file.txt", TEST_FOLDER + TEST_ZIPFILE_REMOVE, "Dummy_File.txt", TestZipErrorLogger));
   ASSERT_FALSE(Zip::AddFileToZip(TEST_FOLDER + TEST_FILE, TEST_FOLDER + "inexistent_file.zip", "Dummy_File.txt", TestZipErrorLogger));
   ASSERT_FALSE(Zip::AddFileToZip(TEST_FOLDER + TEST_FILE, TEST_FOLDER + TEST_ZIPFILE_REMOVE, "", TestZipErrorLogger));
}

// check for failure
TEST_F(HelpersTest, ZipRemoveFailure)
{
   EXPECT_LE(Zip::RemoveEntryFromZip(TEST_FOLDER + TEST_ZIPFILE_REMOVE, "foobar.xxx.txt"), 0);
   EXPECT_LE(Zip::RemoveEntryFromZip(TEST_FOLDER + "inexistent_file_foobar.zip", "foobar.xxx.txt"), 0);
   EXPECT_LE(Zip::RemoveEntryFromZip(TEST_FOLDER + "inexistent_file_foobar.zip", "inexistent_dir/"), 0);
}

TEST_F(HelpersTest, GetTextFromZip)
{
   bool bRes = false;
   const std::string strExpected = "Hello World !";
   const std::string strResult = Zip::ExtractTextFromZip(TEST_FOLDER + TEST_ZIPFILE, TEST_ZIPTEXT, bRes, TestZipErrorLogger);

   EXPECT_TRUE(bRes);
   EXPECT_STREQ(strExpected.c_str(), strResult.c_str());
}

}  // namespace

int main(int argc, char **argv)
{
   if (argc > 1 && GlobalTestInit(argv[1])) // loading test parameters from the INI file...
   {
      ::testing::InitGoogleTest(&argc, argv);
      int iTestResults = RUN_ALL_TESTS();

      ::GlobalTestCleanUp();

      return iTestResults;
   }
   else
   {
      std::cerr << "[ERROR] Encountered an error while loading test parameters from provided INI file !" << std::endl;
      return 1;
   }
}
