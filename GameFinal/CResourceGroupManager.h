#ifndef __RESOURCE_GROUP_MANAGER_CLASS_H__
#define __RESOURCE_GROUP_MANAGER_CLASS_H__

#include "IResourceGroupManager.h"
#include "IShaderManager.h"
#include "IInputLayoutManager.h"
#include "IMaterialManager.h"
#include "IMeshManager.h"
#include "ITextureManager.h"
#include "IRenderStateManager.h"
#include "IPipelineManager.h"
#include "IResourceXmlParser.h"
#include "ISamplerManager.h"

namespace gf
{


	class CResourceGroupManager : public IResourceGroupManager
	{
	public:
		struct SResourceDirectory
		{
			std::string						Path;
			E_RESOURCE_STORE_METHOD			StoreMethod;
		};

		struct SResourceFile
		{
			std::string						Name;
			E_RESOURCE_FILE_TYPE			Type;
			s32								DirectoryIndex;
		};

		struct SResourceGroup
		{
			std::string							Name;
			std::vector<SResourceDirectory>		Directories;
			std::vector<SResourceFile>			Files;
		};

		struct SResourceLocationInfo
		{
			u16									GroupId;
			u16									DirectoryId;
			E_RESOURCE_FILE_TYPE				Type;
			std::string							FileName; /* 记录某个material在哪个xml文件内 */
		};

	public:
		CResourceGroupManager(ITextureManager* TextureManager,
			IShaderManager* ShaderManager,
			IInputLayoutManager* InputlayoutManager,
			IRenderStateManager* RenderStateManager,
			IPipelineManager* PipelineManager,
			IMaterialManager* MaterialManager,
			IMeshManager* MeshManager,
			ISamplerManager* SamplerManager);

		virtual ~CResourceGroupManager();

		virtual bool init(const std::string& configFileName);

		virtual bool loadResourceGroup(u32 groupid);

		virtual bool loadResourceGroup(const std::string& groupname);

		virtual bool getFullPath(const std::string& resourceFileName, 
			std::string& fullpath,
			E_RESOURCE_FILE_TYPE filetype = ERFT_UNKNOWN) const;

		virtual bool getDirectoryPath(const std::string& resourceFileName,
			std::string& dirPath,
			E_RESOURCE_FILE_TYPE filetype = ERFT_UNKNOWN) const;

		virtual bool getPipelineFilePath(const std::string& pipelineName, std::string& path) const;

		virtual bool getMaterialFilePath(const std::string& materialName, std::string& path) const;


		virtual IPipeline* loadPipeline(const std::string& pipelineName);

		virtual IMaterial* loadMaterial(const std::string& materialName);

		virtual IMesh* loadMesh(const std::string& name);

	private:
		bool isResourceGroupLexical(const std::string& s, std::string& groupname);

		bool isResourceDirectoryLexical(const std::string& s, E_RESOURCE_STORE_METHOD& storeMethod, std::string& dirstr);

		bool setupFilePathsInDirectory(u32 groupIndex, u32 dirIndex);

		bool registerResourceFileLocation(u32 groupIndex, s32 dirIndex, const std::string& filename);

		void seperateDirAndName(const std::string& fullpath, std::string& dirpath, std::string& filename);

		void registerMaterialInXml(const std::string& filename, const SResourceLocationInfo& locationInfo);

		void registerPipelineInXml(const std::string& filename, const SResourceLocationInfo& locationInfo);

		E_RESOURCE_FILE_TYPE getResourceFileType(const std::string& path);

		std::vector<SResourceGroup>							mResourceGroups;

		/* the u32 data type is used to store both group id and directory id in the group.
		   the higher 16 bit is used to save group id,
		   the lower 16 bit is used to save directory id. */
		std::map<std::string, SResourceLocationInfo>		mResourceFilesLocationInfos;
		std::vector<const char*>							mResourceFileExtensions[ERFT_TYPE_COUNT];

		/* all material file names which contain no directory path. */
		std::vector<std::string>							mMaterialFileNames;
		
		/* material name and the index in 'mMaterialFileNames' 
		This map is used to find out that the material is in which file. */
		std::map<std::string, u32>							mMaterialNameAndFileNameIndexMapping;


		/* all pipeline xml file names */
		std::vector<std::string>							mPipelineFileNames;

		std::map<std::string, u32>							mPipelineNameAndFileNameIndexMapping;

	};

}

#endif