#ifndef __SAMPLER_MANAGER_INTERFACE_H__
#define __SAMPLER_MANAGER_INTERFACE_H__

#include "ISampler.h"

class ISamplerManager : public IReferenceCounted
{
public:
	virtual ISampler* create(const std::string& name, const SSamplerDesc& desc) = 0;

	virtual ISampler* get(const std::string& name) const = 0;

	_DECLARE_RESOURCE_DESTROY_METHODS(ISampler);

};


#endif
