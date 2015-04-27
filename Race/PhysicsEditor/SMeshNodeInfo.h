#ifndef __MESH_NODE_INFO_H__
#define __MESH_NODE_INFO_H__

using namespace gf;

enum E_BOUNDING_CATEGORY
{
	BOX_BOUNDING = 0,
	SPHERE_BOUNDING = 1,
	CYLINDER_BOUNDING = 2,
	BOUNDING_CATEGORY_COUNT = 3,
};

struct SBoundingShape
{
	IMeshNode*			WireFrameNode;
	XMFLOAT3			Center;
	XMFLOAT3			Rotation;
	f32					Mass;
	f32					Friction;

	SBoundingShape()
		:WireFrameNode(nullptr)
		, Center(0, 0, 0)
		, Mass(5.0f)
		, Friction(0.5f)
		, Rotation(0, 0, 0)
	{

	}

	virtual LPTSTR getShapeName() const = 0;
	virtual const char* getShapeNameA() const = 0;
	virtual E_BOUNDING_CATEGORY getCategory() const = 0;
	virtual XMFLOAT4X4 getTransform() const = 0;
};

struct SBoxBounding : public SBoundingShape
{
	XMFLOAT3	Size;
	

	SBoxBounding() 
	{

	}

	virtual LPTSTR getShapeName() const
	{
		return TEXT("Box");
	}

	virtual const char* getShapeNameA() const
	{
		return "Box";
	}
	
	virtual E_BOUNDING_CATEGORY getCategory() const
	{
		return BOX_BOUNDING;
	}

	virtual XMFLOAT4X4 getTransform() const
	{
		XMMATRIX S = XMMatrixScaling(Size.x, Size.y, Size.z);
		XMMATRIX R = XMMatrixRotationRollPitchYaw(Rotation.x, Rotation.y, Rotation.z);
		XMMATRIX T = XMMatrixTranslation(Center.x, Center.y, Center.z);
		XMMATRIX M = S * R * T;

		XMFLOAT4X4 mat;
		XMStoreFloat4x4(&mat, M);
		return mat;
	}


};



struct SCylinderBounding : public SBoundingShape
{
	f32	Height;
	f32	Radius;

	SCylinderBounding()
		:Height(1.0f),
		Radius(1.0f)
	{

	}

	virtual LPTSTR getShapeName() const
	{
		return TEXT("Cylinder");
	}

	virtual const char* getShapeNameA() const
	{
		return "Cylinder";
	}

	virtual E_BOUNDING_CATEGORY getCategory() const
	{
		return CYLINDER_BOUNDING;
	}

	virtual XMFLOAT4X4 getTransform() const
	{
		XMMATRIX S = XMMatrixScaling(Radius, Height, Radius);
		XMMATRIX R = XMMatrixRotationRollPitchYaw(Rotation.x, Rotation.y, Rotation.z);
		XMMATRIX T = XMMatrixTranslation(Center.x, Center.y, Center.z);
		XMMATRIX M = S * R * T;

		XMFLOAT4X4 mat;
		XMStoreFloat4x4(&mat, M);
		return mat;
	}
};

struct SMeshNodeInfo
{
	IMeshNode*							Node;
	std::vector<SBoundingShape*>		BoundingShapes;

	SMeshNodeInfo()
		:Node(nullptr)
	{

	}

	void SetVisible(bool bVisible)
	{
		if (Node)
			Node->setVisible(bVisible);
		
		for (u32 i = 0; i < BoundingShapes.size(); i++)
			BoundingShapes[i]->WireFrameNode->setVisible(bVisible);
	}

};

#endif

