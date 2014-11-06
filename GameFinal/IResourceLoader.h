#ifndef __RESOURCE_LOADER_INTERFACE_H__
#define __RESOURCE_LOADER_INTERFACE_H__

#include "IReferenceCounted.h"
#include "IShaderManager.h"
#include "IResourceXmlParser.h"
#include "IModelFileParser.h"

namespace gf
{

	class IResourceLoader : public IReferenceCounted
	{
	public:
		virtual bool loadTexture(const std::string& name) = 0;

		virtual bool loadTexturesFromXml(const std::string& name) = 0;

		virtual bool loadPipelinesFromFile(const std::string& name) = 0;

		virtual bool loadMaterialsFromFile(const std::string& name) = 0;

		virtual bool loadMeshFromFile(const std::string& name) = 0;

		virtual IPipeline* loadPipeline(const std::string& pipelineName, const std::string& filepath) = 0;

		virtual IMaterial* loadMaterial(const std::string& materialName, const std::string& filepath) = 0;

		_DECLARE_SINGLETON_INSTANCE(IResourceLoader);
	};

}

#endif
