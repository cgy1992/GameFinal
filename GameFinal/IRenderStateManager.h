#ifndef __RENDER_STATE_MANAGER_INTERFACE_H__
#define __RENDER_STATE_MANAGER_INTERFACE_H__

#include "IRenderState.h"
namespace gf
{
	class IRenderStateManager : public IReferenceCounted
	{
	public:
		virtual IRenderState* create(const std::string& name) = 0;
		virtual IRenderState* get(const std::string& name, bool addIfNotFound = true) = 0;

		

		_DECLARE_SINGLETON_INSTANCE(IRenderStateManager);

		_DECLARE_RESOURCE_DESTROY_METHODS(IRenderState);
	};
}
#endif

