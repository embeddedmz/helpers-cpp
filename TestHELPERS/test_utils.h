#ifndef INCLUDE_TEST_UTILS_H_
#define INCLUDE_TEST_UTILS_H_

#include <algorithm>
#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <functional>
#include <iostream>
#include <iomanip>
#include <iterator>
#include <memory>
#include <mutex>
#include <streambuf>
#include <string>
#include <sstream>
#include <sys/stat.h>
#include <thread>
#include <vector>

#ifdef WINDOWS
#ifdef _DEBUG
#ifdef _USE_VLD_
#include <vld.h>
#endif
#endif
#endif

#include "SimpleIni.h"
#include "gtest/gtest.h"   // Google Test Framework
#include "Helpers.h"       // Test subject (SUT)

bool GlobalTestInit(const std::string& strConfFile);
void GlobalTestCleanUp(void);

int TestZipProgressCallback(double dFileSize, double dBytesCount);
void TestZipErrorLogger(const std::string& strMsg);

std::string GetTimeStamp();
bool GetFileTime(const char* const & pszFilePath, time_t& tLastModificationTime);

template<typename K, typename E, typename C>
bool AreMapsEqual(const std::map<K, E, C>& map, const std::unordered_map<K, E>& umap)
{
   return
      map.size() == umap.size() &&
      std::all_of(map.cbegin(), map.cend(), [&](std::pair<K, E> item)
   {
      auto iter = umap.find(item.first);
      return iter != umap.end() && iter->second == item.second;
   }
   );
}

#endif
