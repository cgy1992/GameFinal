#ifndef __TERRAIN_NODE_INTERFACE_H__
#define __TERRAIN_NODE_INTERFACE_H__

#include "IMeshNode.h"
#include "ITerrainMesh.h"
#include "IPipeline.h"
#include "IMaterial.h"

namespace gf
{
	class ITerrainNode : public IMeshNode
	{
	public:
		ITerrainNode(ISceneNode* parent,
			ISceneManager* smgr,
			const XMFLOAT3& position = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& rotation = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& scale = XMFLOAT3(1.0f, 1.0f, 1.0f))
			:IMeshNode(parent, smgr, false, position, rotation, scale)
		{

		}

		virtual ITerrainMesh* getTerrainMesh() = 0;

		virtual bool isInsideTerrainScope(f32 x, f32 z, bool localPivot = false) const = 0;

		virtual f32 getHeight(f32 x, f32 z, bool localPivot = false) const = 0;

		virtual bool addGrassLand(u32 grassNum, f32 grassWidth, f32 grassHeight, 
			const std::string& materialName,
			u32 pitchPerRow = 8,
			f32 amplitude = 0.5f,
			XMFLOAT3 windDirection = XMFLOAT3(-1.0f, 0, 1.0f)) = 0;

		virtual void removeGrassLand() = 0;
	};
}

#endif

