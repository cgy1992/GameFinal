#include "stdafx.h"
#include "PhysicsLoader.h"
#include "tinyxml/tinyxml2.h"

std::map<std::string, SPhysicalBoundingCollection> PhysicsLoader::BoundingInfos;

SPhysicalBoundingCollection* PhysicsLoader::GetBoundings(const std::string& meshName)
{
	auto it = BoundingInfos.find(meshName);
	if (it == BoundingInfos.end())
		return nullptr;

	return &it->second;
}

bool PhysicsLoader::Load(const char* filename)
{
	IResourceGroupManager* rgmr = IResourceGroupManager::getInstance();
	std::string fullpath;
	if (!rgmr->getFileFullPath("main.physics.xml", fullpath))
		return false;

	
	tinyxml2::XMLDocument doc;
	
	if (doc.LoadFile(fullpath.c_str()) != tinyxml2::XML_NO_ERROR)
		return false;
	
	tinyxml2::XMLElement* root_node = doc.FirstChildElement("meshes");
	if (!root_node)
		return false;
	
	tinyxml2::XMLElement* mesh_node = root_node->FirstChildElement("mesh");
	while (mesh_node)
	{
		ReadMeshBoundings(mesh_node);
		mesh_node = mesh_node->NextSiblingElement("mesh");
	}
	
	//delete doc;
	return true;
}

void PhysicsLoader::ReadMeshBoundings(tinyxml2::XMLElement* mesh_node)
{
	const char* szMeshName = mesh_node->Attribute("name");
	std::string meshName = szMeshName;

	SPhysicalBoundingCollection collection;
	tinyxml2::XMLElement* bounding_node = mesh_node->FirstChildElement();
	while (bounding_node)
	{
		ReadBoundingNode(&collection, bounding_node);
		bounding_node = bounding_node->NextSiblingElement();
	}
	
	BoundingInfos.insert(std::make_pair(meshName, collection));
}

void PhysicsLoader::ReadBoundingNode(SPhysicalBoundingCollection* collection, tinyxml2::XMLElement* bounding_node)
{
	const char* category_str = bounding_node->Value();
	SPhysicalBounding shape;

	// read center.
	const char* center_str = bounding_node->Attribute("center");
	if (center_str)
	{
		sscanf_s(center_str, "%f,%f,%f",
			&shape.Center.x, &shape.Center.y, &shape.Center.z);
	}

	// read mass
	const char* mass_str = bounding_node->Attribute("mass");
	if (mass_str)
	{
		sscanf_s(mass_str, "%f", &shape.Mass);
	}

	// read friction
	const char* friction_str = bounding_node->Attribute("friction");
	if (friction_str)
	{
		sscanf_s(friction_str, "%f", &shape.Friction);
	}

	// read rotation
	const char* rotation_str = bounding_node->Attribute("rotation");
	if (rotation_str)
	{
		sscanf_s(rotation_str, "%f,%f,%f",
			&shape.Rotation.x, &shape.Rotation.y, &shape.Rotation.z);
	}

	if (_stricmp(category_str, "Box") == 0)
	{
		shape.Category = BOX_BOUNDING;
		const char* size_str = bounding_node->Attribute("size");
		if (size_str)
		{
			sscanf_s(size_str, "%f,%f,%f", &shape.Box.Size[0],
				&shape.Box.Size[1], &shape.Box.Size[2]);
		}
	}
	else if (_stricmp(category_str, "Cylinder") == 0)
	{
		shape.Category = CYLINDER_BOUNDING;
		const char* s = bounding_node->Attribute("height");
		if (s)
			sscanf_s(s, "%f", &shape.Cylinder.Height);

		s = bounding_node->Attribute("radius");
		if (s)
			sscanf_s(s, "%f", &shape.Cylinder.Radius);
	}

	collection->Boundings.push_back(shape);
}

