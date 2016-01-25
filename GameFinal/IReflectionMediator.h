#ifndef __REFLECTION_MEDIATOR_H_INTERFACE__
#define __REFLECTION_MEDIATOR_H_INTERFACE__

#include "IMeshNode.h"
#include "IReflectionPlane.h"

namespace gf
{
	class IReflectionMediator : IReferenceCounted
	{
	public:

		virtual void setMapping(IMeshNode* meshNode, IReflectionPlane* plane);
		virtual void setMapping(IMeshNode* meshNode, u32 id);
		void removeMapping(IMeshNode* meshNode);
		IReflectionPlane* getReflectionPlane(IMeshNode* meshNode);
	};
}



#endif
