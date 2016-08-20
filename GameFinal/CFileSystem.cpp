#include "stdafx.h"
#include "CFileSystem.h"
#include <sys/types.h>
#include <sys/stat.h>
namespace gf
{
	bool CFileSystem::listFilesInDir(const std::string& name,
		bool recursive, SFileInfoList* fileInfoList)
	{
		if (!fileInfoList) 
			return false;

		if (!isDirectory(name))
			return false;

#if GF_PLATFORM == GF_PLATFORM_WIN32
		std::string dirPath = name;
		if (dirPath.back() != '/')
			dirPath.insert(dirPath.end(), '/');

		std::string dirPathWithWildcard = dirPath;
		dirPathWithWildcard.insert(dirPathWithWildcard.end(), '*');

		WIN32_FIND_DATAA ffd;
		HANDLE hFind;
		hFind = FindFirstFileA(dirPathWithWildcard.c_str(), &ffd);
		if (INVALID_HANDLE_VALUE == hFind) {
			return false;
		}

		do {

			// if the item is a directory
			if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				if (recursive && 
					strcmp(ffd.cFileName, ".") != 0 &&
					strcmp(ffd.cFileName, "..") != 0 ) 
				{
					std::string subDirPath = dirPath + ffd.cFileName;
					listFilesInDir(subDirPath, recursive, fileInfoList);
				}
			}
			else {
				SFileInfo fileInfo;
				fileInfo.Filename = ffd.cFileName;
				fileInfo.Path = dirPath;
				fileInfo.CompressedSize = ffd.nFileSizeLow;
				fileInfo.UncompressedSize = ffd.nFileSizeLow;
				fileInfoList->push_back(fileInfo);
			}
		}
		while (FindNextFileA(hFind, &ffd) != 0);
#endif
		FindClose(hFind);
		return true;
	}



	bool CFileSystem::isDirectory(const std::string& name)
	{
#if GF_PLATFORM == GF_PLATFORM_WIN32
		struct _stat fileStat;
		if ((_stat(name.c_str(), &fileStat) == 0) && (fileStat.st_mode & _S_IFDIR))
		{
			return true;
		}
		return false;
#endif
	}


	
}