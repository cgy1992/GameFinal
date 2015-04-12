#include "stdafx.h"
#include "CFileParser.h"
#include "EditorScene.h"
#include "Enums.h"

bool CFileParser::SaveScene(const char* filename)
{
	FILE* fp = fopen(filename, "wb");
	if (!fp)
		return false;

	WriteFileHeader(fp);
	WriteMeshNodes(fp);
	WriteCollections(fp);
	WriteLightNodes(fp);
	
	fclose(fp);

	return true;
}

bool CFileParser::ReadScene(const char* filename)
{
	EditorScene* scene = EditorScene::getInstance();
	if (!scene)
		return false;

	FILE* fp = fopen(filename, "rb");
	if (!fp)
		return false;

	SSceneFileHeader header = ReadFileHeader(fp);
	ReadMeshNodes(header.MeshNodeCount, fp);
	ReadCollections(header.CollectionNodeCount, fp);
	ReadLightNodes(header.LightNodeCount, fp);
	fclose(fp);

	return true;
}

SSceneFileHeader CFileParser::ReadFileHeader(FILE* fp)
{
	EditorScene* scene = EditorScene::getInstance();
	ISceneManager* smgr = scene->mSceneManager;
	SSceneFileHeader header;
	fread(&header, sizeof(header), 1, fp);

	EditorScene::nodeSequenceNumber = header.LastSequenceNum;

	// set ambient
	smgr->setAmbient(header.Ambient);

	// set global directional light
	ILightNode* light = smgr->addDirectionalLight(1, nullptr, header.GlobalLightDirection);
	light->setDiffuse(header.GlobalLightDiffuse);
	light->setSpecular(header.GlobalLightSpecular);
	light->enableShadow(true);
	scene->mDirectionalLightNode = light;
	scene->UpdateGlobalLighting();

	// set camera
	ICameraNode* camera = smgr->addFpsCameraNode(1, nullptr, header.CameraPosition);
	camera->look(header.CameraLook);
	camera->setShadowRange(300.0f);
	scene->mCamera = camera;

	// set terrain.
	IMeshManager* meshManager = IMeshManager::getInstance();
	/*
	ITerrainMesh* terrainMesh = meshManager->createTerrainMesh("terrain", header.TerrainHeightFile,
		header.TerrainVertexSpace, header.TerrainHeightScale, true, false, 1.0f);

	ITerrainNode* terrainNode = smgr->addTerrainNode(terrainMesh);
	terrainNode->setMaterialName(header.TerrainMaterial);
	*/

	ITerrainMesh* terrainMesh = meshManager->createTerrainMesh("terrain", header.TerrainHeightFile,
		header.TerrainVertexSpace, 0.15f, false, true, 1.0f);

	ITerrainNode* terrainNode = smgr->addTerrainNode(terrainMesh);
	terrainNode->setMaterialName("terrain/grass_terrain_material");

	scene->mTerrainNode = terrainNode;

	// set defer shading
	scene->mDeferredShading = header.DeferredShading;
	scene->mVideoDriver->setDeferredShading(scene->mDeferredShading);

	return header;
}

void CFileParser::WriteFileHeader(FILE* fp)
{
	EditorScene* scene = EditorScene::getInstance();
	ISceneManager* smgr = scene->mSceneManager;
	ILightNode* directionLight = scene->mDirectionalLightNode;
	ICameraNode* camera = scene->mCamera;
	ITerrainNode* terrainNode = scene->mTerrainNode;

	SSceneFileHeader header;
	header.LastSequenceNum = EditorScene::nodeSequenceNumber;
	header.Ambient = smgr->getAmbient();
	header.GlobalLightDirection = XMFLOAT3(directionLight->getDirection().x,
		directionLight->getDirection().y, directionLight->getDirection().z);
	header.GlobalLightDiffuse = directionLight->getDiffuse();
	header.GlobalLightSpecular = directionLight->getSpecular();

	header.CameraPosition = camera->getPosition();
	header.CameraRight = camera->getRightVector();
	header.CameraUp = camera->getUpVector();
	header.CameraLook = camera->getLookVector();
	header.DeferredShading = scene->mDeferredShading;
	header.MeshNodeCount = scene->mNodeInfos.size();
	header.CollectionNodeCount = scene->mCollectionNodeInfos.size();
	header.LightNodeCount = scene->mLightNodeInfos.size();

	std::string heightMapFile = terrainNode->getTerrainMesh()->getHeightMapName();
	strcpy_s(header.TerrainHeightFile, heightMapFile.c_str());
	strcpy_s(header.TerrainMaterial, terrainNode->getMaterial()->getName().c_str());
	header.TerrainVertexSpace = terrainNode->getTerrainMesh()->getVertexSpace();
	header.TerrainHeightScale = terrainNode->getTerrainMesh()->getHeightScale();

	fwrite(&header, sizeof(header), 1, fp);
}

SSceneFileMeshNodeInfo CFileParser::ConvertNodeInfo(SNodeInfo* nodeInfo)
{
	SSceneFileMeshNodeInfo fileinfo;
	fileinfo.Id = nodeInfo->Id;
	strcpy_s(fileinfo.Name, nodeInfo->Name.c_str());
	
	IMeshNode* node = dynamic_cast<IMeshNode*>(nodeInfo->Node);
	IMesh* mesh = node->getMesh();
	if (mesh)
		strcpy_s(fileinfo.MeshName, mesh->getName().c_str());

	fileinfo.Static = nodeInfo->Static;
	fileinfo.BoundingPhysics = nodeInfo->BoundingPhysics;
	fileinfo.ShadowCasting = nodeInfo->ShadowCasting;
	fileinfo.Position = nodeInfo->Position;
	fileinfo.Rotation = nodeInfo->Rotation;
	fileinfo.Scaling = nodeInfo->Scaling;
	fileinfo.Category = nodeInfo->Category;
	fileinfo.Tag = nodeInfo->Node->getTag();

	return fileinfo;
}

void CFileParser::AddMeshNodeToScene(SSceneFileMeshNodeInfo* fileinfo)
{
	EditorScene* scene = EditorScene::getInstance();

	if (fileinfo->Category != MESH_CATEGORY)
		return;

	IMeshManager* meshManager = IMeshManager::getInstance();
	IModelMesh* mesh = meshManager->getModelMesh(fileinfo->MeshName, true);
	if (!mesh)
		return;

	ISceneManager* smgr = scene->mSceneManager;
	IMeshNode* node = smgr->addModelMeshNode(mesh, nullptr, false,
		fileinfo->Position, fileinfo->Rotation, fileinfo->Scaling);

	SNodeInfo info;
	info.Id = fileinfo->Id;
	info.Name = fileinfo->Name;
	info.Static = fileinfo->Static;
	info.BoundingPhysics = fileinfo->BoundingPhysics;
	info.ShadowCasting = fileinfo->ShadowCasting;
	info.Position = fileinfo->Position;
	info.Rotation = fileinfo->Rotation;
	info.Scaling = fileinfo->Scaling;
	info.CollectionId = 0;
	info.Category = fileinfo->Category;
	info.Node = node;
	node->setTag(fileinfo->Tag);

	if (info.ShadowCasting)
		node->addShadow(1);

	scene->mNodeInfos.insert(std::make_pair(info.Id, info));
	scene->mNodeIdMap.insert(std::make_pair(info.Node, info.Id));
}

void CFileParser::WriteMeshNodes(FILE* fp)
{
	EditorScene* scene = EditorScene::getInstance();

	for (auto it = scene->mNodeInfos.begin(); it != scene->mNodeInfos.end(); it++)
	{
		SNodeInfo* info = &it->second;
		SSceneFileMeshNodeInfo fileinfo = ConvertNodeInfo(info);
		fwrite(&fileinfo, sizeof(fileinfo), 1, fp);
	}
}

void CFileParser::ReadMeshNodes(u32 count, FILE* fp)
{
	SSceneFileMeshNodeInfo info;
	for (u32 i = 0; i < count; i++)
	{
		fread(&info, sizeof(info), 1, fp);
		AddMeshNodeToScene(&info);
	}
}

void CFileParser::ReadCollectionNode(FILE* fp)
{
	EditorScene* scene = EditorScene::getInstance();
	IMeshManager* meshManager = IMeshManager::getInstance();

	SSceneFileMeshNodeInfo fileinfo;
	fread(&fileinfo, sizeof(fileinfo), 1, fp);

	SNodeInfo info;
	info.Id = fileinfo.Id;
	info.Name = fileinfo.Name;
	info.Static = fileinfo.Static;
	info.BoundingPhysics = fileinfo.BoundingPhysics;
	info.ShadowCasting = fileinfo.ShadowCasting;
	info.Position = fileinfo.Position;
	info.Rotation = fileinfo.Rotation;
	info.Scaling = fileinfo.Scaling;
	info.CollectionId = 0;
	info.Category = fileinfo.Category;

	IModelMesh* mesh = meshManager->getModelMesh(fileinfo.MeshName, true);
	IInstanceCollectionNode* collectionNode = scene->CreateCollectionNode(mesh, 5000);
	collectionNode->setTag(fileinfo.Tag);

	info.Node = collectionNode;
	scene->mNodeInfos.insert(std::make_pair(info.Id, info));
	scene->mNodeIdMap.insert(std::make_pair(collectionNode, info.Id));

	SCollectionNodeInfo collectionNodeInfo;
	collectionNodeInfo.Id = info.Id;
	collectionNodeInfo.MeshName = mesh->getName();
	collectionNodeInfo.CollectionNode = collectionNode;

	u32 instanceCount;
	fread(&instanceCount, sizeof(u32), 1, fp);

	for (u32 i = 0; i < instanceCount; i++)
	{
		SSceneFileInstanceNodeInfo instanceInfo;
		fread(&instanceInfo, sizeof(instanceInfo), 1, fp);

		IInstanceNode* instanceNode = collectionNode->addInstance(false, instanceInfo.Position, instanceInfo.Rotation, instanceInfo.Scaling);
		instanceNode->setTag(instanceInfo.Tag);
		if (instanceInfo.ShadowCasting)
			instanceNode->addShadow(1);

		SNodeInfo nodeinfo;
		nodeinfo.Id = instanceInfo.Id;
		nodeinfo.Category = INSTANCE_CATEGORY;
		nodeinfo.Static = instanceInfo.Static;
		nodeinfo.CollectionId = collectionNodeInfo.Id;
		nodeinfo.BoundingPhysics = instanceInfo.BoundingPhysics;
		nodeinfo.Position = instanceInfo.Position;
		nodeinfo.Rotation = instanceInfo.Rotation;
		nodeinfo.Scaling = instanceInfo.Scaling;
		nodeinfo.ShadowCasting = instanceInfo.ShadowCasting;
		nodeinfo.Node = instanceNode;
		collectionNodeInfo.AddNodeInfo(nodeinfo);
	}

	scene->mCollectionNodeInfos.push_back(collectionNodeInfo);
}

void CFileParser::WriteCollectionNode(FILE* fp, SCollectionNodeInfo* collectionNodeInfo)
{
	EditorScene* scene = EditorScene::getInstance();
	SNodeInfo* nodeInfo = &scene->mNodeInfos[collectionNodeInfo->Id];
	
	SSceneFileMeshNodeInfo fileNodeInfo = ConvertNodeInfo(nodeInfo);
	fwrite(&fileNodeInfo, sizeof(fileNodeInfo), 1, fp);

	u32 instanceCount = collectionNodeInfo->InstanceNodeInfos.size();
	fwrite(&instanceCount, sizeof(instanceCount), 1, fp);

	for (auto it = collectionNodeInfo->InstanceNodeInfos.begin();
		it != collectionNodeInfo->InstanceNodeInfos.end(); it++)
	{
		SNodeInfo* info = &it->second;
		SSceneFileInstanceNodeInfo instanceInfo;
		instanceInfo.Id = info->Id;
		instanceInfo.BoundingPhysics = info->BoundingPhysics;
		instanceInfo.ShadowCasting = info->ShadowCasting;
		instanceInfo.Static = info->Static;
		instanceInfo.Tag = info->Node->getTag();
		instanceInfo.Position = info->Position;
		instanceInfo.Rotation = info->Rotation;
		instanceInfo.Scaling = info->Scaling;
		fwrite(&instanceInfo, sizeof(instanceInfo), 1, fp);
	}
}

void CFileParser::ReadCollections(u32 count, FILE* fp)
{
	for (u32 i = 0; i < count; i++)
	{
		ReadCollectionNode(fp);
	}
}

void CFileParser::WriteCollections(FILE* fp)
{
	EditorScene* scene = EditorScene::getInstance();
	for (u32 i = 0; i < scene->mCollectionNodeInfos.size(); i++)
	{
		WriteCollectionNode(fp, &scene->mCollectionNodeInfos[i]);
	}
}

void CFileParser::ReadLightNodes(u32 count, FILE* fp)
{
	EditorScene* scene = EditorScene::getInstance();
	ISceneManager* smgr = scene->mSceneManager;

	SSceneFilePointLightInfo fileinfo;
	for (u32 i = 0; i < count; i++)
	{
		fread(&fileinfo, sizeof(fileinfo), 1, fp);
		
		SLightNodeInfo nodeInfo;
		nodeInfo.Id = fileinfo.Id;
		nodeInfo.Name = fileinfo.Name;
		nodeInfo.Static = fileinfo.Static;
		nodeInfo.Position = fileinfo.Position;
		nodeInfo.Diffuse = fileinfo.Diffuse;
		nodeInfo.Specular = fileinfo.Specular;
		nodeInfo.Attenuations = fileinfo.Attenuations;
		nodeInfo.Range = fileinfo.Range;

		nodeInfo.InstanceNode = scene->mPointLightCollectionNode->addInstance(false, fileinfo.Position);
		XMFLOAT4 color(0, 0, 1.0f, 1.0f);
		nodeInfo.InstanceNode->setData(&color);
		nodeInfo.InstanceNode->setTag(WIRE_NODE_TAG);

		ILightNode* light = smgr->addPointLight(nodeInfo.Id, nullptr, false,
			nodeInfo.Position, nodeInfo.Range);

		light->setTag(fileinfo.Tag);
		light->setDiffuse(nodeInfo.Diffuse);
		light->setSpecular(nodeInfo.Specular);
		light->setAttenuation(nodeInfo.Attenuations.x, nodeInfo.Attenuations.y, nodeInfo.Attenuations.z);
		nodeInfo.Node = light;

		scene->mLightNodeInfos.insert(std::make_pair(nodeInfo.Id, nodeInfo));
		scene->mNodeIdMap.insert(std::make_pair(light, nodeInfo.Id));
	}
}

SSceneFilePointLightInfo CFileParser::ConvertPointLightInfo(SLightNodeInfo* info)
{
	SSceneFilePointLightInfo fileinfo;
	fileinfo.Id = info->Id;
	strcpy_s(fileinfo.Name, info->Name.c_str());
	fileinfo.Static = info->Static;
	fileinfo.Position = info->Position;
	fileinfo.Attenuations = info->Attenuations;
	fileinfo.Diffuse = info->Diffuse;
	fileinfo.Specular = info->Specular;
	fileinfo.Range = info->Range;
	fileinfo.Tag = info->Node->getTag();

	return fileinfo;
}

void CFileParser::WriteLightNodes(FILE* fp)
{
	EditorScene* scene = EditorScene::getInstance();
	for (auto it = scene->mLightNodeInfos.begin(); it != scene->mLightNodeInfos.end(); it++)
	{
		SLightNodeInfo* lightInfo = &it->second;
		SSceneFilePointLightInfo info = ConvertPointLightInfo(lightInfo);
		fwrite(&info, sizeof(info), 1, fp);
	}
}




