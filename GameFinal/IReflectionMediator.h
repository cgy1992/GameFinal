#ifndef __REFLECTION_MEDIATOR_H_INTERFACE__
#define __REFLECTION_MEDIATOR_H_INTERFACE__

#include "IMeshNode.h"
#include "IReflectionPlane.h"

namespace gf
{
	class IReflectionMediator : public IReferenceCounted
	{
	public:
		virtual void setMapping(IMeshNode* meshNode, IReflectionPlane* plane) = 0;
		virtual void setMapping(IMeshNode* meshNode, u32 id) = 0;
		virtual void removeMapping(IMeshNode* meshNode) = 0;
		virtual void removeMappings(u32 planeId) = 0;
		virtual IReflectionPlane* getReflectionPlane(IMeshNode* meshNode) = 0;
	};
}



#endif
