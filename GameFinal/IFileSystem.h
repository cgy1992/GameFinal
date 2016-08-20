#ifndef __FILE_SYSTEM_INTERFACE_H__
#define __FILE_SYSTEM_INTERFACE_H__

#include "IReferenceCounted.h"

namespace gf
{
	struct SFileInfo
	{
		std::string Filename;
		std::string Path;
		u32			CompressedSize;
		/// Uncompressed size
		u32			UncompressedSize;

		SFileInfo() {
			CompressedSize = 0;
			UncompressedSize = 0;
		}
	};

	typedef std::vector<SFileInfo> SFileInfoList;

	class IFileSystem : public IReferenceCounted
	{
	public:
		//getBasePath();
		virtual bool listFilesInDir(
			const std::string& dir,
			bool recursive,
			SFileInfoList* fileInfoList) = 0;

		virtual bool isDirectory(const std::string& name) = 0;
	};
}

#endif