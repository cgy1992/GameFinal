#ifndef __REFLECTION_MEDIATOR_H_CLASS__
#define __REFLECTION_MEDIATOR_H_CLASS__

#include "IMeshNode.h"
#include "IReflectionPlane.h"
#include "IReflectionMediator.h"
#include <unordered_map>

namespace gf
{
	/************************************************************************/
	/* This class maintains the mapping between mesh node and reflection plane.                                                                     */
	/************************************************************************/
	class CReflectionMediator: public IReflectionMediator
	{
	public:
		CReflectionMediator(ISceneManager* smgr);
		virtual void setMapping(IMeshNode* meshNode, IReflectionPlane* plane);
		virtual void setMapping(IMeshNode* meshNode, u32 id);
		virtual void removeMapping(IMeshNode* meshNode);
		virtual void removeMappings(u32 planeId);
		virtual IReflectionPlane* getReflectionPlane(IMeshNode* meshNode);
	
	private:
		ISceneManager* mSceneManager;
		std::unordered_map<IMeshNode*, u32>	mMeshReflectionMappings;
	};
}

#endif
