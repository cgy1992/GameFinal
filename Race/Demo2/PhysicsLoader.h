#pragma once
#include "tinyxml/tinyxml2.h"
using namespace gf;

enum E_BOUNDING_CATEGORY
{
	BOX_BOUNDING = 0,
	SPHERE_BOUNDING = 1,
	BOUNDING_CATEGORY_COUNT = 2,
};

struct SPhysicalBounding
{
	E_BOUNDING_CATEGORY		Category;
	XMFLOAT3				Center;
	XMFLOAT3				Rotation;
	f32						Mass;
	f32						Friction;
	union 
	{
		struct _tagBox
		{
			f32 Size[3];

		} Box;

		struct _tagSphere
		{
			f32 Radius;

		} Sphere;
	};
};

struct SPhysicalBoundingCollection
{
	std::vector<SPhysicalBounding> Boundings;
};

class PhysicsLoader
{
public:
	static bool Load(const char* filename);
	
	static SPhysicalBoundingCollection* GetBoundings(const std::string& meshName);
	
	static void ReadMeshBoundings(tinyxml2::XMLElement* node);
	
	static void ReadBoundingNode(SPhysicalBoundingCollection* collection,
		tinyxml2::XMLElement* bounding_node);

//	static PhysicsLoader* getInstance();

private:
	static std::map<std::string, SPhysicalBoundingCollection>		BoundingInfos;
	
};

