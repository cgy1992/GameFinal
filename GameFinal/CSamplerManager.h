#ifndef __SAMPLER_MANAGER_CLASS_H__
#define __SAMPLER_MANAGER_CLASS_H__

#include "ISamplerManager.h"
#include "IResourceFactory.h"
namespace gf
{
	class CSamplerManager : public ISamplerManager
	{
	public:
		CSamplerManager(IResourceFactory* pResourceFactory);

		virtual ISampler* create(const std::string& name, const SSamplerDesc& desc);

		virtual ISampler* get(const std::string& name) const;

		_DEFINE_RESOURCE_DESTROY_METHODS(CSamplerManager, mSamplerMap, ISampler);

	private:

		void createStockSamplers();

		IResourceFactory*						mResourceFactory;
		std::map<std::string, ISampler*>		mSamplerMap;
	};
}
#endif
