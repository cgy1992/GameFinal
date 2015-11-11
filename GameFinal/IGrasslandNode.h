#ifndef __GF_GRASSLAND_NODE_INTERFACE__
#define __GF_GRASSLAND_NODE_INTERFACE__

#include "ITerrainNode.h"

namespace gf
{
	class IGrasslandNode : public IMeshNode
	{
	public:
		IGrasslandNode(ITerrainNode* parent,
			ISceneManager* smgr);



	};
}

#endif