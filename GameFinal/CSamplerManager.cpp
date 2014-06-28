#include "stdafx.h"
#include "CSamplerManager.h"
namespace gf
{
	CSamplerManager::CSamplerManager(IResourceFactory* pResourceFactory)
		:mResourceFactory(pResourceFactory)
	{

	}

	ISampler* CSamplerManager::create(const std::string& name, const SSamplerDesc& desc)
	{
		auto it = mSamplerMap.find(name);
		if (it != mSamplerMap.end())
		{
			GF_PRINT_CONSOLE_INFO("The sampler named '%s' already exists.\n", name.c_str());
			return nullptr;
		}

		ISampler* sampler = mResourceFactory->createSampler(name, desc);
		if (!sampler)
			return nullptr;

		mSamplerMap.insert(std::make_pair(name, sampler));
		return sampler;
	}

	ISampler* CSamplerManager::get(const std::string& name) const
	{
		auto it = mSamplerMap.find(name);
		if (it == mSamplerMap.end())
			return nullptr;

		return it->second;
	}
}
