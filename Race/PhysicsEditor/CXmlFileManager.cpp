#include "stdafx.h"
#include "CXmlFileManager.h"
#include "CEditorScene.h"


bool CXmlFileManager::Read(const char* filename)
{
	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(filename) != tinyxml2::XML_NO_ERROR)
		return false;

	tinyxml2::XMLElement* root_node = doc.FirstChildElement("meshes");
	if (!root_node)
		return false;

	tinyxml2::XMLElement* mesh_node = root_node->FirstChildElement("mesh");
	while (mesh_node)
	{
		ReadMeshNodeInfo(mesh_node);
		mesh_node = mesh_node->NextSiblingElement("mesh");
	}

	return true;
}

bool CXmlFileManager::Save(const char* filename)
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement* root_node = doc.NewElement("meshes");
	doc.InsertFirstChild(root_node);

	CEditorScene* scene = CEditorScene::getInstance();

	for (auto it = scene->mMeshNodeInfos.begin(); it != scene->mMeshNodeInfos.end(); it++)
	{
		const std::string meshName = it->first;
		SMeshNodeInfo* info = &it->second;
		InsertMeshNodeInfo(doc, root_node, meshName, info);
	}

	doc.SaveFile(filename);
	return true;
}

void CXmlFileManager::InsertMeshNodeInfo(
	tinyxml2::XMLDocument& doc,
	tinyxml2::XMLElement* root_node,
	const std::string& meshName,
	SMeshNodeInfo* info)
{
	tinyxml2::XMLElement* mesh_node = doc.NewElement("mesh");
	root_node->InsertEndChild(mesh_node);
	mesh_node->SetAttribute("name", meshName.c_str());

	char text[256];
	for (u32 i = 0; i < info->BoundingShapes.size(); i++)
	{
		SBoundingShape* shape = info->BoundingShapes[i];
		tinyxml2::XMLElement* bounding_node = doc.NewElement(shape->getShapeNameA());
		mesh_node->InsertEndChild(bounding_node);

		// insert center.
		sprintf_s(text, "%f,%f,%f", shape->Center.x, shape->Center.y, shape->Center.z);
		bounding_node->SetAttribute("center", text);

		// insert mass.
		sprintf_s(text, "%f", shape->Mass);
		bounding_node->SetAttribute("mass", text);

		// insert friction.
		sprintf_s(text, "%f", shape->Friction);
		bounding_node->SetAttribute("friction", text);

		// insert rotation
		sprintf_s(text, "%f,%f,%f", shape->Rotation.x, shape->Rotation.y, shape->Rotation.z);
		bounding_node->SetAttribute("rotation", text);

		if (shape->getCategory() == BOX_BOUNDING)
		{
			SBoxBounding* box = dynamic_cast<SBoxBounding*>(shape);
			// insert size.
			sprintf_s(text, "%f,%f,%f", box->Size.x, box->Size.y, box->Size.z);
			bounding_node->SetAttribute("size", text);
		}
	}
}

void CXmlFileManager::ReadMeshNodeInfo(tinyxml2::XMLElement* mesh_node)
{
	CEditorScene* scene = CEditorScene::getInstance();

	const char* szMeshName = mesh_node->Attribute("name");
	std::string meshName = szMeshName;
	
	auto it = scene->mMeshNodeInfos.find(meshName);
	if (it == scene->mMeshNodeInfos.end())
		return;

	SMeshNodeInfo* info = &it->second;
	tinyxml2::XMLElement* bounding_node = mesh_node->FirstChildElement();
	while (bounding_node)
	{
		ReadBoundingNode(info, bounding_node);
		bounding_node = bounding_node->NextSiblingElement();
	}
}



void CXmlFileManager::ReadBoundingNode(SMeshNodeInfo* info,
	tinyxml2::XMLElement* bounding_node)
{
	const char* category_str = bounding_node->Value();
	SBoundingShape* shape = CreateBoundingShape(category_str);

	CEditorScene* scene = CEditorScene::getInstance();

	int err;
	// read center.
	const char* center_str = bounding_node->Attribute("center");
	if (center_str)
	{
		sscanf_s(center_str, "%f,%f,%f", 
			&shape->Center.x, &shape->Center.y, &shape->Center.z);
	}
	
	// read mass
	const char* mass_str = bounding_node->Attribute("mass");
	if (mass_str)
	{
		sscanf_s(mass_str, "%f", &shape->Mass);
	}

	// read friction
	const char* friction_str = bounding_node->Attribute("friction");
	if (friction_str)
	{
		sscanf_s(friction_str, "%f", &shape->Friction);
	}

	// read rotation
	const char* rotation_str = bounding_node->Attribute("rotation");
	if (rotation_str)
	{
		sscanf_s(rotation_str, "%f,%f,%f",
			&shape->Rotation.x, &shape->Rotation.y, &shape->Rotation.z);
	}

	if (shape->getCategory() == BOX_BOUNDING)
	{
		SBoxBounding* box = dynamic_cast<SBoxBounding*>(shape);
		const char* size_str = bounding_node->Attribute("size");
		if (size_str)
		{
			sscanf_s(size_str, "%f,%f,%f", &box->Size.x, &box->Size.y, &box->Size.z);
		}

		IMeshNode* node = scene->mSceneManager->addMeshNode(scene->mCubeMesh,
			nullptr, nullptr, false, shape->Center, shape->Rotation, box->Size);
		node->setMaterialName("bounding_wire_material");
		box->WireFrameNode = node;
	}

	info->BoundingShapes.push_back(shape);
}

SBoundingShape* CXmlFileManager::CreateBoundingShape(const char* category)
{
	if (_stricmp(category, "Box") == 0)
		return new SBoxBounding;

	return nullptr;
}



