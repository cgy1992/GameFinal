#pragma once
#include "tinyxml/tinyxml2.h"

#include "SMeshNodeInfo.h"

class CXmlFileManager
{
public:
	static bool Read(const char* filename);
	static bool Save(const char* filename);
	static void InsertMeshNodeInfo(tinyxml2::XMLDocument& doc,
		tinyxml2::XMLElement* root_node, 
		const std::string& meshName,
		SMeshNodeInfo* info);

	static void ReadMeshNodeInfo(tinyxml2::XMLElement* mesh_node);
	static void ReadBoundingNode(SMeshNodeInfo* info,
		tinyxml2::XMLElement* bounding_node);
	static SBoundingShape* CreateBoundingShape(const char* category);

private:



};

