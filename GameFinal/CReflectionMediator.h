#ifndef __REFLECTION_MEDIATOR_H_CLASS__
#define __REFLECTION_MEDIATOR_H_CLASS__

#include "IMeshNode.h"
#include "IReflectionPlane.h"
#include <unordered_map>

namespace gf
{
	/************************************************************************/
	/* This class maintains the mapping between mesh node and reflection plane.                                                                     */
	/************************************************************************/
	class CReflectionMediator
	{
	public:
		void setMapping(IMeshNode* meshNode, IReflectionPlane* plane);
		void setMapping(IMeshNode* meshNode, u32 id);
		void removeMapping(IMeshNode* meshNode);
		IReflectionPlane* getReflectionPlane(IMeshNode* meshNode);
	private:
		ISceneManager* mSceneManager;
		std::unordered_map<IMeshNode*, u32>	mMeshReflectionMappings;
	};
}

#endif
