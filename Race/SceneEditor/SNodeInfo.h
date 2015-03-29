#ifndef __NODE_INFO_STRUCT_H__
#define __NODE_INFO_STRUCT_H__

using namespace gf;
#include "SceneFileStructs.h"

struct SNodeInfo
{
	int					Id;
	std::string			Name;
	bool				Static;
	bool				BoundingPhysics;
	bool				ShadowCasting;
	XMFLOAT3			Position;
	XMFLOAT3			Rotation;
	XMFLOAT3			Scaling;
	

	ISceneNode*			Node;
	int					CollectionId;
	E_NODE_CATEGORY		Category;

	SNodeInfo()
		:Rotation(0, 0, 0)
		, Position(0, 0, 0)
		, Scaling(1.0f, 1.0f, 1.0f)
		, Static(false)
		, BoundingPhysics(false)
		, ShadowCasting(true)
		, CollectionId(0)
		, Category(MESH_CATEGORY)
	{
		
	}
};


struct SLightNodeInfo
{
	int					Id;
	std::string			Name;
	bool				Static;
	XMFLOAT3			Position;
	XMFLOAT4			Diffuse;
	XMFLOAT4			Specular;
	f32					Range;
	XMFLOAT3			Attenuations;
	ILightNode*			Node;
	IInstanceNode*		InstanceNode;

	SLightNodeInfo()
		:Position(0, 0, 0)
		, Static(true)
		, Diffuse(1.0f, 1.0f, 1.0f, 1.0f)
		, Specular(1.0f, 1.0f, 1.0f, 1.0f)
		, Range(10.0f)
		, Attenuations(1.0f, 1.0f, 0)
	{

	}
};

#endif
