#ifndef __NODE_INFO_STRUCT_H__
#define __NODE_INFO_STRUCT_H__

using namespace gf;

struct SNodeInfo
{
	int					Id;
	bool				Static;
	bool				BoundingPhysics;
	bool				MultipleInstanced;
	bool				ShadowCasting;
	XMFLOAT3			Position;
	XMFLOAT3			Rotation;
	XMFLOAT3			Scaling;
	ISceneNode*			Node;

	SNodeInfo()
		:Rotation(0, 0, 0)
		, Scaling(1.0f, 1.0f, 1.0f)
		, Static(false)
		, BoundingPhysics(false)
		, ShadowCasting(true)
	{
		
	}
};


#endif
