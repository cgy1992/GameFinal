#ifndef __EMPTY_NODE_CLASS_H__
#define __EMPTY_NODE_CLASS_H__

#include "ISceneNode.h"

namespace gf
{
	class CEmptyNode : public ISceneNode
	{
	public:
		CEmptyNode(ISceneNode* parent, ISceneManager* smgr,
			const XMFLOAT3& position = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& rotation = XMFLOAT3(0, 0, 0),
			const XMFLOAT3& scale = XMFLOAT3(1.0f, 1.0f, 1.0f))
			:ISceneNode(parent, smgr, false, position, rotation, scale)
		{

		}

		virtual void render(E_PIPELINE_USAGE usage) {}

		virtual E_SCENE_NODE_TYPE getNodeType() const
		{
			return ESNT_EMPTY;
		}
	};

}



#endif
