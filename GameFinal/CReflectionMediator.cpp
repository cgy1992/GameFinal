#include "stdafx.h"
#include "CReflectionMediator.h"
#include "ISceneManager.h"

namespace gf
{
	CReflectionMediator::CReflectionMediator(ISceneManager* smgr)
		:mSceneManager(smgr)
	{
		
	}

	void CReflectionMediator::setMapping(IMeshNode* meshNode, IReflectionPlane* plane)
	{
		mMeshReflectionMappings[meshNode] = plane->getId();
	}

	void CReflectionMediator::setMapping(IMeshNode* meshNode, u32 id) 
	{
		mMeshReflectionMappings[meshNode] = id;
	}

	void CReflectionMediator::removeMapping(IMeshNode* meshNode)
	{
		auto it = mMeshReflectionMappings.find(meshNode);
		if (it != mMeshReflectionMappings.end()) {
			mMeshReflectionMappings.erase(it);
		}
	}

	IReflectionPlane* CReflectionMediator::getReflectionPlane(IMeshNode* meshNode)
	{
		auto it = mMeshReflectionMappings.find(meshNode);
		if (it != mMeshReflectionMappings.end()) {
			u32 id = it->second;
			return mSceneManager->getReflectionPlane(id);
		}
		
		return nullptr;
	}

	void CReflectionMediator::removeMappings(u32 id)
	{
		auto it = this->mMeshReflectionMappings.begin();
		while (it != mMeshReflectionMappings.end()) {
			if (it->second == id) {
				auto it2 = it;
				it++;
				mMeshReflectionMappings.erase(it2);
			}
			else
			{
				it++;
			}
		}

	}



}

