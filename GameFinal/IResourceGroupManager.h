#ifndef __RESOURCE_GROUP_MANAGER_INTERFACE_H__
#define __RESOURCE_GROUP_MANAGER_INTERFACE_H__

#include "IReferenceCounted.h"
#include "IResourceLoader.h"

namespace gf
{

	enum E_RESOURCE_STORE_METHOD
	{
		ERSM_FILESYSTEM,
		ERSM_ZIP
	};

	enum E_RESOURCE_TYPE
	{
		ERT_SHADER,
		ERT_INPUTLAYOUT,
		ERT_PIPELINE,
		ERT_MESH,
		ERT_MATERIAL
	};

	/* 按照加载的先后顺序排列 */
	enum E_RESOURCE_FILE_TYPE
	{
		ERFT_UNKNOWN = -1,
		ERFT_TEXTURE = 0,
		ERFT_TEXTURE_XML,
		ERFT_SHADER,
		ERFT_INPUTLAYOUT,
		ERFT_PIPELINE_XML,
		ERFT_PIPELINE,
		ERFT_MATERIAL_XML,
		ERFT_MATERIAL,
		ERFT_MESH,
		ERFT_RAW,
		ERFT_TYPE_COUNT
	};

	class IResourceGroupManager : public IReferenceCounted
	{
	public:

		virtual bool init(const std::string& configFileName) = 0;

		virtual bool loadResourceGroup(u32 groupid) = 0;

		virtual bool loadResourceGroup(const std::string& groupname) = 0;

		virtual IPipeline* loadPipeline(const std::string& pipelineName) = 0;

		virtual IMaterial* loadMaterial(const std::string& materialName) = 0;

		virtual IMesh* loadMesh(const std::string& name) = 0;

		virtual bool getFullPath(const std::string& resourceFileName, 
			std::string& fullpath, 
			E_RESOURCE_FILE_TYPE filetype = ERFT_UNKNOWN) const = 0;

	protected:

		IResourceLoader*				mResourceLoader;

		_DECLARE_SINGLETON_INSTANCE(IResourceGroupManager);
	};
}

#endif