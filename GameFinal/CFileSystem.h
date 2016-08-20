#ifndef __FILE_SYSTEM_CLASS_H__
#define __FILE_SYSTEM_CLASS_H__


#include "IFileSystem.h"

namespace gf
{
	class CFileSystem : public IFileSystem
	{
	public:

		virtual bool isDirectory(const std::string& name);

		virtual bool listFilesInDir(
			const std::string& dir,
			bool recursive,
			SFileInfoList* fileInfoList);
	};

}

#endif
