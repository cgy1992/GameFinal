#include "stdafx.h"
#include "CPipelineManager.h"
#include "CPipeline.h"

namespace gf
{


	CPipelineManager::CPipelineManager(IResourceFactory* pResourceFactory)
		:mResourceFactory(pResourceFactory)
	{
		
	}


	IPipeline* CPipelineManager::create(const std::string& name,
		IShader** shaders,
		u32 shaderCount,
		IInputLayout* inputlayout,
		E_PRIMITIVE_TYPE primitiveType,
		IRenderState* renderState)
	{
		auto it = mPipelinesCache.find(name);
		if (it != mPipelinesCache.end())
		{
			GF_PRINT_CONSOLE_INFO("create pipeline (%s) repeatedly!\n", name.c_str());
			return it->second;
		}

		IPipeline* pipeline = mResourceFactory->createPipeline(name,
			shaders, shaderCount, inputlayout, primitiveType, renderState);

		if (!pipeline)
		{
			GF_PRINT_CONSOLE_INFO("create pipeline '%s' failed.\n", name.c_str());
			return nullptr;
		}

		mPipelinesCache.insert(std::make_pair(name, pipeline));

		return pipeline;
	}


	IPipeline* CPipelineManager::get(const std::string& name, bool bLoadIfNotExist)
	{
		auto it = mPipelinesCache.find(name);
		if (it != mPipelinesCache.end())
			return it->second;

		if (bLoadIfNotExist)
		{
			return IResourceGroupManager::getInstance()->loadPipeline(name);
		}

		return nullptr;
	}

}
