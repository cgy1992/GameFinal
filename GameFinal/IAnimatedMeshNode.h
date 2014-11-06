#ifndef __ANIMATED_MESH_NODE_INTERFACE_H__
#define __ANIMATED_MESH_NODE_INTERFACE_H__

#include "IMeshNode.h"

namespace gf
{


	class IAnimatedMeshNode : public IMeshNode
	{
	public:
		IAnimatedMeshNode(ISceneNode* parent,
			ISceneManager* smgr,
			bool bStatic,
			const XMFLOAT3& position = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& rotation = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& scale = XMFLOAT3(1.0f, 1.0f, 1.0f))
			:IMeshNode(parent, smgr, bStatic, position, rotation, scale)
		{

		}


		virtual bool setAnimation(u32 id) = 0;

		virtual bool setAnimation(const std::string& name) = 0;

		virtual void addTime(f32 t) = 0;

		virtual E_SCENE_NODE_TYPE getNodeType() const
		{
			return ESNT_ANIMATED_MESH;
		}
	};

}

#endif
