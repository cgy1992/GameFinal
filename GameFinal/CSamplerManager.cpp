#include "stdafx.h"
#include "CSamplerManager.h"
namespace gf
{
	

	CSamplerManager::CSamplerManager(IResourceFactory* pResourceFactory)
		:mResourceFactory(pResourceFactory)
	{
		createStockSamplers();
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

	void CSamplerManager::createStockSamplers()
	{
		// create shadow map sampler
		SSamplerDesc desc;
		desc.AddressU = EAM_BORDER;
		desc.AddressV = EAM_BORDER;
		desc.AddressW = EAM_BORDER;
		desc.BorderColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		desc.Filter = ESF_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
		desc.ComparsionFunc = ECF_LESS;
		ISampler* shadowMapSampler = create(ISamplerManager::SHADOW_MAP_SAMPLER, desc);

		desc.AddressU = EAM_WRAP;
		desc.AddressV = EAM_WRAP;
		desc.AddressW = EAM_WRAP;
		desc.Filter = ESF_FILTER_MIN_MAG_MIP_POINT;
		desc.ComparsionFunc = ECF_NEVER;
		ISampler* wrapPointSampler = create(ISamplerManager::WRAP_POINT_SAMPLER, desc);
	}

}
