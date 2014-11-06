#ifndef __SAMPLER_MANAGER_INTERFACE_H__
#define __SAMPLER_MANAGER_INTERFACE_H__

#include "ISampler.h"
namespace gf
{
	class ISamplerManager : public IReferenceCounted
	{
	public:
		virtual ISampler* create(const std::string& name, const SSamplerDesc& desc) = 0;

		virtual ISampler* get(const std::string& name) const = 0;

		_DECLARE_SINGLETON_INSTANCE(ISamplerManager);

		_DECLARE_RESOURCE_DESTROY_METHODS(ISampler);

		// here are some built-in samplers' names
		const static std::string WRAP_POINT_SAMPLER;
		const static std::string SHADOW_MAP_SAMPLER;

	};
}

#endif
