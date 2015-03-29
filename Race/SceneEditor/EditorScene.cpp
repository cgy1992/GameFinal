#include "stdafx.h"
#include "EditorScene.h"
#include "EditorWindow.h"
#include "Enums.h"
#include "CFileParser.h"

using namespace gf;

EditorScene* EditorScene::_instance = nullptr;
u32 EditorScene::nodeSequenceNumber = 10;

u32 EditorScene::getNextNodeSequence()
{
	return ++nodeSequenceNumber;
}

EditorScene::EditorScene(IDevice* device)
:mDevice(device)
, mSceneManager(nullptr)
, mGroundSize(300.0f)
, mAddedNode(nullptr)
, mPickingCubeNode(nullptr)
, mSelectedCubeNode(nullptr)
, mSelectedNode(nullptr)
, mCameraFocusingObject(false)
, mTerrainNode(nullptr)
, mTileBasedDSShader(nullptr)
, mDeferredShading(false)
, mPointLightCollectionNode(nullptr)
, mAddedLightNodeInfo(nullptr)
, mPickingPointLightInstance(nullptr)
, mSelectedLightNodeInfo(nullptr)
{
	mVideoDriver = device->getVideoDriver();
	mTextureManager = mVideoDriver->getTextureManager();
	
	mMeshManager = mVideoDriver->getMeshManager();
	mMaterialManager = mVideoDriver->getMaterialManager();
	
	mResourceGroupManager = mVideoDriver->getResourceGroupManager();
	mResourceGroupManager->init("Resources.cfg");

	BufferWndWidth = mVideoDriver->getBackBufferWidth();
	BufferWndHeight = mVideoDriver->getBackBufferHeight();
}

void EditorScene::setupInitialScene()
{
	math::SAxisAlignedBox aabb;
	aabb.Center = XMFLOAT3(0, 0.0f, 0);
	aabb.Extents = XMFLOAT3(3000.0f, 1000.0f, 3000.0f);

	ISceneManager* smgr = mDevice->createSceneManager(aabb);
	mSceneManager = smgr;

	ITextureCube* skyTexture = mTextureManager->loadCubeTexture("skybox1.dds");
	smgr->setSkyDome(skyTexture);

	mTimer = mDevice->getTimer();

	ISimpleMesh* cubeMesh = mMeshManager->createCubeMesh("cube1");

	mSelectedCubeNode = smgr->addMeshNode(cubeMesh, nullptr, nullptr, false);
	mSelectedCubeNode->setMaterialName("selected_cube_material");
	mSelectedCubeNode->setVisible(false);
	mSelectedCubeNode->setTag(WIRE_NODE_TAG);

	mPickingCubeNode = smgr->addMeshNode(cubeMesh, nullptr, nullptr, false);
	mPickingCubeNode->setMaterialName("picking_cube_material");
	mPickingCubeNode->setVisible(false);
	mPickingCubeNode->setTag(WIRE_NODE_TAG);

	ISimpleMesh* sphereMesh = mMeshManager->createSphereMesh("pointlight_mesh", 1.0f, 10, 10);
	
	mPointLightCollectionNode = smgr->addInstanceCollectionNode(sphereMesh, 
		nullptr, 4000, sizeof(XMFLOAT4));
	mPointLightCollectionNode->setMaterialName("multi_wire_material");
	mPointLightCollectionNode->setTag(WIRE_NODE_TAG);
	mPointLightCollectionNode->setVisible(false);

	mSceneManager->update(0);
	

	IVideoDriver* driver = IVideoDriver::getInstance();
	mVideoDriver->setDeferredShading(true);
	driver->setDeferredShadingAlgorithm(EDSA_CS_TILE_BASED_DEFERRED_SHADING);

	IShaderManager* shaderManager = IShaderManager::getInstance();
	mTileBasedDSShader = shaderManager->load(EST_COMPUTE_SHADER, "defer_shader_cs.hlsl", "cs_main");
	smgr->setTileBasedDeferredShadingCS(mTileBasedDSShader);

	if (!CFileParser::ReadScene("main.scene"))
	{
		setupTerrain();

		ILightNode* light = smgr->addDirectionalLight(1, nullptr, XMFLOAT3(3.0f, -2.0f, 1.5f));
		light->setSpecular(XMFLOAT4(1.0f, 1.0f, 1.0f, 32.0f));
		light->setDiffuse(XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));
		light->enableShadow(true);
		mDirectionalLightNode = light;

		smgr->setAmbient(XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));

		mCamera = smgr->addFpsCameraNode(1, nullptr, XMFLOAT3(0, 30.0f, -4.0f),
			XMFLOAT3(0, 30.0f, 0.0f), XMFLOAT3(0, 1.0f, 0), true);
		mCamera->setShadowRange(300.0f);

		mVideoDriver->setDeferredShading(false);

		mDeferredShading = true;
	}

	UpdateGlobalLighting();
	
	smgr->update();
}

void EditorScene::setupTerrain()
{
	IMeshManager* meshManager = IMeshManager::getInstance();
	std::string rawFileName("terrain-heightmap4.raw");
	
	/*
	ITerrainMesh* terrainMesh = meshManager->createTerrainMesh("terrain", rawFileName,
		10.0f, 1.0f, false, true, 1.0f);
	mTerrainNode = mSceneManager->addTerrainNode(terrainMesh);
	mTerrainNode->setMaterialName("terrain/terrain_material");
	*/
	
	ITerrainMesh* terrainMesh = meshManager->createTerrainMesh("terrain", 
		rawFileName, 5.0f, 0.3f, true, false, 1.0f);
	mTerrainNode = mSceneManager->addTerrainNode(terrainMesh);
	mTerrainNode->setMaterialName("terrain/grass_tessellation_material");

}

void EditorScene::update(f32 dt)
{
	EditorWindow* window = EditorWindow::getInstance();
	if (window)
		window->OnKeyBoard(dt);
	//changeTreePosition();
	mSceneManager->update(dt);
}

void EditorScene::drawAll()
{
	if (mDeferredShading)
	{
		mVideoDriver->setDeferredShading(true);
		mSceneManager->drawAll();
		mVideoDriver->setDeferredShading(false);
		//mSceneManager->draw(mPickingCubeNode);
		//mSceneManager->draw(mSelectedCubeNode);
		//mSceneManager->draw(mPointLightCollectionNode);
		mSceneManager->draw(WIRE_NODE_TAG);

		IMeshNode* skyNode = mSceneManager->getSkyNode();
		mSceneManager->draw(skyNode);
	}
	else
	{
		mSceneManager->drawAll();
	}
}


void EditorScene::UpdateCamera(f32 delta)
{
	ICameraNode* camera = mCamera;
	const f32 CAMERA_MOVE_UNIT = 60.0f;
	const f32 CAMERA_ROTATE_UNIT = 1.0f;
	
	if (GetAsyncKeyState('W') & 0x8000)
	{
		camera->walk(CAMERA_MOVE_UNIT * delta);
	}

	if (GetAsyncKeyState('S') & 0x8000)
	{
		camera->walk(-CAMERA_MOVE_UNIT * delta);
	}

	if (GetAsyncKeyState('A') & 0x8000)
	{
		camera->strafe(-CAMERA_MOVE_UNIT * delta);
	}

	if (GetAsyncKeyState('D') & 0x8000)
	{
		camera->strafe(CAMERA_MOVE_UNIT * delta);
	}

	if (GetAsyncKeyState('R') & 0x8000)
	{
		camera->fly(CAMERA_MOVE_UNIT * delta);
	}

	if (GetAsyncKeyState('F') & 0x8000)
	{
		camera->fly(-CAMERA_MOVE_UNIT * delta);
	}

	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		camera->pitch(-CAMERA_ROTATE_UNIT * delta);
	}

	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		camera->pitch(CAMERA_ROTATE_UNIT * delta);
	}

	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		camera->yaw(-CAMERA_ROTATE_UNIT * delta);
	}

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		camera->yaw(CAMERA_ROTATE_UNIT * delta);
	}

	if (GetAsyncKeyState('N') & 0x8000)
	{
		camera->roll(CAMERA_ROTATE_UNIT * delta);
	}

	if (GetAsyncKeyState('M') & 0x8000)
	{
		camera->roll(-CAMERA_ROTATE_UNIT * delta);
	}
}

EditorScene::~EditorScene()
{
	mSceneManager->destroy();
}

bool EditorScene::PrepareAddingLight()
{
	SLightNodeInfo* nodeInfo = new SLightNodeInfo();
	nodeInfo->Id = getNextNodeSequence();

	nodeInfo->InstanceNode = mPointLightCollectionNode->addInstance(false, nodeInfo->Position);
	XMFLOAT4 color(0, 0, 1.0f, 1.0f);
	nodeInfo->InstanceNode->setData(&color);
	nodeInfo->InstanceNode->setTag(WIRE_NODE_TAG);
	
	ILightNode* lightNode = mSceneManager->addPointLight(nodeInfo->Id, nullptr, false, nodeInfo->Position, nodeInfo->Range);
	nodeInfo->Node = lightNode;
	nodeInfo->Node->setTag(LIGHT_NODE_TAG);
	lightNode->setDiffuse(nodeInfo->Diffuse);
	lightNode->setSpecular(nodeInfo->Specular);
	lightNode->setAttenuation(nodeInfo->Attenuations.x, nodeInfo->Attenuations.y, nodeInfo->Attenuations.z);
	lightNode->setRange(nodeInfo->Range);

	mAddedLightNodeInfo = nodeInfo;
	return true;
}

bool EditorScene::PrepareAddingObject(const std::string& meshName)
{
	IModelMesh* mesh = mMeshManager->getModelMesh(meshName, true);
	IMeshNode* node = mSceneManager->addModelMeshNode(mesh, nullptr, false);
	node->addShadow(1);
	node->setTag(MESH_NODE_TAG);
	mAddedNode = node;
	return true;
}


bool EditorScene::PrepareAddingInstance(u32 collectionId)
{
	SCollectionNodeInfo* collectionInfo = nullptr;
	for (u32 i = 0; i < mCollectionNodeInfos.size(); i++)
	{
		if (mCollectionNodeInfos[i].Id == collectionId)
		{
			collectionInfo = &mCollectionNodeInfos[i];
			break;
		}
	}

	IInstanceCollectionNode* collectionNode = collectionInfo->CollectionNode;
	IInstanceNode* node = collectionNode->addInstance(false, XMFLOAT3(0, 0, 0));
	node->setTag(MESH_NODE_TAG);
	node->addShadow(1);
	mAddedNode = node;
	return true;
}

u32 EditorScene::AddObject(const char* nodeName)
{
	if (!mAddedNode)
		return -1;

	SNodeInfo nodeInfo;
	nodeInfo.Id = getNextNodeSequence();
	nodeInfo.Static = false;
	nodeInfo.Node = mAddedNode;
	nodeInfo.Name = nodeName;

	XMFLOAT3 pos = mAddedNode->getPosition();
	nodeInfo.Position = pos;

	mNodeInfos.insert(std::make_pair(nodeInfo.Id, nodeInfo));
	mNodeIdMap.insert(std::make_pair(mAddedNode, nodeInfo.Id));

	mAddedNode = nullptr;
	return nodeInfo.Id;
}

u32 EditorScene::AddLightNode(const char* nodeName)
{
	if (!mAddedLightNodeInfo)
		return -1;

	u32 id = mAddedLightNodeInfo->Id;
	mAddedLightNodeInfo->Name = nodeName;

	mLightNodeInfos.insert(std::make_pair(id, *mAddedLightNodeInfo));
	mNodeIdMap.insert(std::make_pair(mAddedLightNodeInfo->Node, id));

	delete mAddedLightNodeInfo;
	mAddedLightNodeInfo = nullptr;
	return id;
}

u32 EditorScene::AddInstanceNode()
{
	if (!mAddedNode || mAddedNode->getNodeType() != ESNT_INSTANCE_MESH)
		return -1;

	IInstanceNode* node = dynamic_cast<IInstanceNode*>(mAddedNode);
	SCollectionNodeInfo* collectionNodeInfo = nullptr;

	for (u32 i = 0; i < mCollectionNodeInfos.size(); i++)
	{
		if (mCollectionNodeInfos[i].CollectionNode == node->getCollectionNode())
		{
			collectionNodeInfo = &mCollectionNodeInfos[i];
			break;
		}
	}

	if (!collectionNodeInfo)
		return -1;

	SNodeInfo nodeInfo;
	int id = EditorScene::getNextNodeSequence();
	nodeInfo.Id = id;
	nodeInfo.Static = true;
	nodeInfo.Node = mAddedNode;
	nodeInfo.Position = mAddedNode->getPosition();
	
	collectionNodeInfo->AddNodeInfo(nodeInfo);
	mAddedNode = nullptr;
	return id;
}

void EditorScene::CancelAddingObject()
{
	if (mAddedNode)
	{
		mAddedNode->destroy();
		mAddedNode = nullptr;
	}
}

math::SRay EditorScene::computePickingRay(u32 sx, u32 sy)
{
	IVideoDriver* driver = mDevice->getVideoDriver();

	sx = (static_cast<f32>(sx)) / BufferWndWidth * driver->getBackBufferWidth();
	sy = (static_cast<f32>(sy)) / BufferWndHeight * driver->getBackBufferHeight();

	math::SRay ray = mSceneManager->getPickingRay(sx, sy);
	return ray;
}

bool EditorScene::GetPickingPosition(u32 sx, u32 sy, XMFLOAT3& position)
{
	math::SRay ray = computePickingRay(sx, sy);
	if (!mTerrainNode)
	{
		const f32 EPSILON = 0.0001f;

		if (fabs(ray.Direction.y) < EPSILON)
			return false;

		f32 t = (0 - ray.Origin.y) / ray.Direction.y;
		if (t < 0)
			return false;

		f32 x = ray.Origin.x + t * ray.Direction.x;
		f32 z = ray.Origin.z + t * ray.Direction.z;

		if (x < -mGroundSize * 0.5f || x > mGroundSize * 0.5f
			|| z < -mGroundSize * 0.5f || z > mGroundSize * 0.5f)
			return false;

		position = XMFLOAT3(x, 0, z);
		return true;
	}
	else
	{
		ITerrainMesh* terrainMesh = mTerrainNode->getTerrainMesh();
		XMFLOAT3 intersectPoint;
		if (terrainMesh->intersectRay(ray, intersectPoint))
		{
			position = intersectPoint;
			return true;
		}
	}
	return false;
}

void EditorScene::ChangeAddingLightPosition(u32 sx, u32 sy)
{
	if (!mAddedLightNodeInfo)
		return;

	XMFLOAT3 position;
	if (GetPickingPosition(sx, sy, position))
	{
		mAddedLightNodeInfo->Position = position;
		mAddedLightNodeInfo->Node->setPosition(position);
		mAddedLightNodeInfo->InstanceNode->setPosition(position);
	}
}

void EditorScene::ChangeAddedObjectPosition(u32 sx, u32 sy)
{
	if (!mAddedNode)
		return;

	XMFLOAT3 position;
	if (GetPickingPosition(sx, sy, position))
	{
		mAddedNode->setPosition(XMFLOAT3(position.x, position.y, position.z));
	}
}

void EditorScene::PickingObject(u32 sx, u32 sy)
{
	math::SRay ray = computePickingRay(sx, sy);

	f32 dist;
	ISceneNode* node = mSceneManager->intersectRayWithTag(ray, &dist, 1, ENT_SOLID_NODE);

	if (node)
	{
		mPickingCubeNode->setVisible(true);
		//mPickingCubeNode->setPosition(node->getPosition());

		math::SOrientedBox obb = node->getOrientedBox();
		XMMATRIX scaleMatrix = XMMatrixScaling(obb.Extents.x * 2, obb.Extents.y * 2, obb.Extents.z * 2);
		XMMATRIX rotMatrix = XMMATRIX(
			obb.Axis[0].x, obb.Axis[0].y, obb.Axis[0].z, 0,
			obb.Axis[1].x, obb.Axis[1].y, obb.Axis[1].z, 0,
			obb.Axis[2].x, obb.Axis[2].y, obb.Axis[2].z, 0,
			0, 0, 0, 1.0f);

		XMMATRIX transMatrix = XMMatrixTranslation(obb.Center.x, obb.Center.y, obb.Center.z);
		XMMATRIX M = scaleMatrix * rotMatrix * transMatrix;
		mPickingCubeNode->setTransform(M);
	}
	else
	{
		mPickingCubeNode->setVisible(false);
	}
}

int EditorScene::SelectObject(u32 sx, u32 sy)
{
	math::SRay ray = computePickingRay(sx, sy);

	f32 dist;
	ISceneNode* node = mSceneManager->intersectRayWithTag(ray, &dist, 1, ENT_SOLID_NODE);
	if (node)
	{
		SelectObject(node);
		return GetIdBySceneNode(node);
	}

	return -1;
}

void EditorScene::CancelSelectObject()
{
	mSelectedCubeNode->setVisible(false);
	mSelectedNode = nullptr;
}

void EditorScene::updateSelectedNodeCube()
{
	math::SOrientedBox obb = mSelectedNode->getOrientedBox();
	XMMATRIX scaleMatrix = XMMatrixScaling(obb.Extents.x * 2, obb.Extents.y * 2, obb.Extents.z * 2);
	XMMATRIX rotMatrix = XMMATRIX(
		obb.Axis[0].x, obb.Axis[0].y, obb.Axis[0].z, 0,
		obb.Axis[1].x, obb.Axis[1].y, obb.Axis[1].z, 0,
		obb.Axis[2].x, obb.Axis[2].y, obb.Axis[2].z, 0,
		0, 0, 0, 1.0f);

	XMMATRIX transMatrix = XMMatrixTranslation(obb.Center.x, obb.Center.y, obb.Center.z);
	XMMATRIX M = scaleMatrix * rotMatrix * transMatrix;
	mSelectedCubeNode->setTransform(M);
}

void EditorScene::SelectObject(ISceneNode* node)
{
	if (node)
	{
		mSelectedCubeNode->setVisible(true);
		mSelectedNode = node;
		updateSelectedNodeCube();
	}
	else
	{
		mSelectedCubeNode->setVisible(false);
		mSelectedNode = nullptr;
	}
}

bool EditorScene::SelectObject(u32 id)
{
	SNodeInfo* info = GetNodeInfoById(id);
	if (info->Category != COLLECTION_CATEGORY)
	{
		SelectObject(info->Node);
		return true;
	}
	return false;
}

SNodeInfo* EditorScene::GetNodeInfoById(u32 id)
{
	auto it = mNodeInfos.find(id);
	if (it != mNodeInfos.end())
		return &(it->second);

	for (u32 i = 0; i < mCollectionNodeInfos.size(); i++)
	{
		SNodeInfo* info = mCollectionNodeInfos[i].GetNodeInfoById(id);
		if (info)
			return info;
	}

	return NULL;
}

SLightNodeInfo*	EditorScene::GetLightNodeInfoById(u32 id)
{
	auto it = mLightNodeInfos.find(id);
	if (it != mLightNodeInfos.end())
		return &(it->second);
	return NULL;
}

SNodeInfo* EditorScene::GetSelectedNodeInfo()
{
	return GetNodeInfoBySceneNode(mSelectedNode);
}

void EditorScene::UpdateNodeInfo(SNodeInfo* info)
{
	XMMATRIX S = XMMatrixScaling(info->Scaling.x, info->Scaling.y, info->Scaling.z);
	XMMATRIX R = XMMatrixRotationRollPitchYaw(info->Rotation.x, info->Rotation.y, info->Rotation.z);
	XMMATRIX T = XMMatrixTranslation(info->Position.x, info->Position.y, info->Position.z);
	XMMATRIX M = S * R * T;
	info->Node->setTransform(M);
	info->Node->update();

	if (info->Node->getNodeType() & ESNT_MESH)
	{
		IMeshNode* meshNode = dynamic_cast<IMeshNode*>(info->Node);
		if (info->ShadowCasting)
			meshNode->addShadow(1);
		else
			meshNode->removeShadow(1);
	}

	updateSelectedNodeCube();
}

int EditorScene::AddCollectionNode(const char* nodeName, 
	const std::string& meshName, int maxNum)
{
	IModelMesh* mesh = mMeshManager->getModelMesh(meshName, true);
	IInstanceCollectionNode* collectionNode = CreateCollectionNode(mesh, maxNum);

	SNodeInfo nodeInfo;
	nodeInfo.Id = getNextNodeSequence();
	nodeInfo.Node = collectionNode;
	nodeInfo.Name = nodeName;
	nodeInfo.Category = COLLECTION_CATEGORY;
	mNodeInfos.insert(std::make_pair(nodeInfo.Id, nodeInfo));
	mNodeIdMap.insert(std::make_pair(collectionNode, nodeInfo.Id));

	SCollectionNodeInfo collectionNodeInfo;
	collectionNodeInfo.Id = nodeInfo.Id;
	collectionNodeInfo.MeshName = mesh->getName();
	collectionNodeInfo.CollectionNode = collectionNode;
	mCollectionNodeInfos.push_back(collectionNodeInfo);

	return nodeInfo.Id;
}

IInstanceCollectionNode* EditorScene::CreateCollectionNode(IModelMesh* mesh, int maxNum)
{
	IInstanceCollectionNode* collectionNode = mSceneManager->addInstanceCollectionNode(mesh, nullptr, maxNum, 0);
	IMaterialManager* materialManager = IMaterialManager::getInstance();
	IPipelineManager* pipelineManager = IPipelineManager::getInstance();

	std::string prefix = "multi_";
	for (u32 i = 0; i < mesh->getSubsetCount(); i++)
	{
		IMaterial* material = mesh->getMaterial(i);
		if (!material)
			continue;

		std::string multiMaterialName = prefix + material->getName();

		IMaterial* multi_material = materialManager->get(multiMaterialName, false);
		if (!multi_material)
		{
			SMaterial material2(*material);
			material2.setName(multiMaterialName);
			for (u32 j = 0; j < EPU_COUNT; j++)
			{
				IPipeline* pipeline = material2.getPipeline(j);
				if (pipeline)
				{
					std::string pipeline_name2 = prefix + pipeline->getName();
					IPipeline* pipeline2 = pipelineManager->get(pipeline_name2);
					material2.setPipeline(j, pipeline2);
				}
			}

			materialManager->add(material2);
			multi_material = materialManager->get(multiMaterialName, false);
		}

		collectionNode->setMaterial(i, multi_material);
	}

	collectionNode->addShadow(1);
	return collectionNode;
}

int EditorScene::GetIdBySceneNode(ISceneNode* node)
{
	if (!node)
		return -1;

	auto it = mNodeIdMap.find(node);
	if (it != mNodeIdMap.end())
		return it->second;

	for (u32 i = 0; i < mCollectionNodeInfos.size(); i++)
	{
		SNodeInfo* info = mCollectionNodeInfos[i].GetNodeInfo(node);
		if (info)
			return info->Id;
	}
	return -1;
}

SNodeInfo* EditorScene::GetNodeInfoBySceneNode(ISceneNode* node)
{
	int id = GetIdBySceneNode(node);
	if (id == -1)
		return NULL;

	return GetNodeInfoById(id);
}

std::vector<int> EditorScene::GetInstanceIdsInOneCollection(int collectionId)
{
	std::vector<int> ids;
	for (u32 i = 0; i < mCollectionNodeInfos.size(); i++)
	{
		if (mCollectionNodeInfos[i].Id == collectionId)
		{
			ids = mCollectionNodeInfos[i].GetAllInstancesIds();
			break;
		}
	}
	return ids;
}

bool EditorScene::DeleteNode(u32 id)
{
	SNodeInfo* info = GetNodeInfoById(id);
	if (!info)
		return false;

	ISceneNode* node = nullptr;
	if (info->Category == MESH_CATEGORY)
	{
		node = info->Node;
		auto it1 = mNodeInfos.find(id);
		auto it2 = mNodeIdMap.find(node);
		mNodeInfos.erase(it1);
		mNodeIdMap.erase(it2);
		node->destroy();
	}
	else if (info->Category == COLLECTION_CATEGORY)
	{
		for (u32 i = 0; i < mCollectionNodeInfos.size(); i++)
		{
			if (mCollectionNodeInfos[i].Id == id)
			{
				mCollectionNodeInfos[i].DeleteInstances();
				mCollectionNodeInfos.erase(mCollectionNodeInfos.begin() + i);
				break;
			}
		}
		node = info->Node;
		auto it1 = mNodeInfos.find(id);
		auto it2 = mNodeIdMap.find(node);
		mNodeInfos.erase(it1);
		mNodeIdMap.erase(it2);
		node->destroy();
	}
	else if (info->Category == INSTANCE_CATEGORY)
	{
		SCollectionNodeInfo* collectionNodeInfo = GetCollectionNodeInfoById(info->CollectionId);
		collectionNodeInfo->DeleteInstance(id);
	}

	return true;
}

SCollectionNodeInfo* EditorScene::GetCollectionNodeInfoById(u32 id)
{
	SCollectionNodeInfo* nodeInfo = nullptr;
	for (u32 i = 0; i < mCollectionNodeInfos.size(); i++)
	{
		if (mCollectionNodeInfos[i].Id == id)
		{
			nodeInfo = &(mCollectionNodeInfos[i]);
			break;
		}
	}
	return nodeInfo;
}

void EditorScene::PickingLight(u32 sx, u32 sy)
{
	SLightNodeInfo* lightInfo = getIntersectPointLightInfo(sx, sy);
	if (!lightInfo)
		return;

	if (lightInfo == mSelectedLightNodeInfo)
		return;

	if (mPickingPointLightInstance)
	{
		setInstanceNodeColor(mPickingPointLightInstance, XMFLOAT4(0, 0, 1.0f, 1.0f));
		mPickingPointLightInstance = nullptr;
	}
	
	if (lightInfo)
	{
		setInstanceNodeColor(lightInfo->InstanceNode, XMFLOAT4(1.0f, 1.0f, 0, 1.0f));
		mPickingPointLightInstance = lightInfo->InstanceNode;
	}
}

void EditorScene::setInstanceNodeColor(IInstanceNode* node, XMFLOAT4 color)
{
	node->setData(&color);
}

SLightNodeInfo* EditorScene::GetLightNodeInfoBySceneNode(ISceneNode* node)
{
	if (node->getNodeType() != ESNT_LIGHT)
		return nullptr;

	auto it = mNodeIdMap.find(node);
	if (it == mNodeIdMap.end())
		return nullptr;

	u32 id = it->second;
	auto it2 = mLightNodeInfos.find(id);
	if (it2 == mLightNodeInfos.end())
		return nullptr;

	return &(it2->second);
}

SLightNodeInfo* EditorScene::getIntersectPointLightInfo(u32 sx, u32 sy)
{
	math::SRay ray = computePickingRay(sx, sy);
	XMVECTOR Origin = XMLoadFloat3(&ray.Origin);
	Origin = XMVectorSetW(Origin, 1.0f);

	XMVECTOR Direction = XMLoadFloat3(&ray.Direction);
	Direction = XMVectorSetW(Direction, 0);

	f32 minDist = FLT_MAX;
	SLightNodeInfo* intersectInfo = nullptr;
	XNA::Sphere sphere;
	sphere.Radius = 1.0f;

	for (auto it = mLightNodeInfos.begin(); it != mLightNodeInfos.end(); it++)
	{
		SLightNodeInfo* info = &(it->second);
		sphere.Center = info->Position;
		f32 dist;
		if (XNA::IntersectRaySphere(Origin, Direction, &sphere, &dist))
		{
			if (dist < minDist)
			{
				minDist = dist;
				intersectInfo = info;
			}
		}
	}

	return intersectInfo;
}



bool EditorScene::SelectLight(u32 id)
{
	SLightNodeInfo* info = GetLightNodeInfoById(id);
	return SelectLight(info);
}

bool EditorScene::SelectLight(SLightNodeInfo* lightInfo)
{
	if (mSelectedLightNodeInfo)
	{
		setInstanceNodeColor(mSelectedLightNodeInfo->InstanceNode, XMFLOAT4(0, 0, 1.0f, 1.0f));
	}

	if (!lightInfo)
		return false;

	mSelectedLightNodeInfo = lightInfo;
	setInstanceNodeColor(lightInfo->InstanceNode, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	return true;
}

int EditorScene::SelectLight(u32 sx, u32 sy)
{
	SLightNodeInfo* lightInfo = getIntersectPointLightInfo(sx, sy);
	if (lightInfo)
	{
		SelectLight(lightInfo);
		return lightInfo->Id;
	}
	return -1;
}

SLightNodeInfo* EditorScene::GetSelectedLightNodeInfo()
{
	return mSelectedLightNodeInfo;
}

void EditorScene::UpdateLightNodeInfo(SLightNodeInfo* info)
{
	ILightNode* lightNode = info->Node;
	lightNode->setPosition(info->Position);
	lightNode->setRange(info->Range);
	lightNode->setAttenuation(info->Attenuations.x, info->Attenuations.y, info->Attenuations.z);
	lightNode->setDiffuse(info->Diffuse);
	lightNode->setSpecular(info->Specular);
	lightNode->update();

	info->InstanceNode->setPosition(info->Position);
	info->InstanceNode->update();
}

void EditorScene::UpdateGlobalLighting()
{
	SDirectionalLight lightData;
	mDirectionalLightNode->getLightData(&lightData);
	mTileBasedDSShader->setRawData("gLight", &lightData, sizeof(SDirectionalLight));

}

void EditorScene::FocusOnMeshNode(IMeshNode* node, f32 distance)
{
	//XMFLOAT3 vec = math::VectorMinus(mCamera->getPosition(), node->getPosition());
	XMVECTOR cam_pos = XMLoadFloat3(&mCamera->getPosition());
	XMVECTOR node_pos = XMLoadFloat3(&node->getPosition());
	
	XMVECTOR dir = cam_pos - node_pos;

	dir = XMVectorSetY(dir, 0);
	f32 len = XMVectorGetX(XMVector3Length(dir));
	if (len < 0.0001f)
		return;

	XMVECTOR up = XMVectorSet(0, len, 0, 0);
	dir = dir + up;
	dir = XMVector3Normalize(dir);

	math::SAxisAlignedBox aabb = node->getAabb();
	//f32 maxLength = aabb.Extents.x > aabb.Extents.y ? aabb.Extents.x : aabb.Extents.y;
	//cam_pos = node_pos + dir * maxLength * 7.0f;
	
	cam_pos = node_pos + dir * distance;
	XMFLOAT3 camPos, lookDir;
	XMStoreFloat3(&camPos, cam_pos);
	XMStoreFloat3(&lookDir, -dir);

	mCamera->setPosition(camPos);
	mCamera->look(lookDir);
}

void EditorScene::FocusSelectedObject()
{
	if (mSelectedNode)
	{
		IMeshNode* node = dynamic_cast<IMeshNode*>(mSelectedNode);
		FocusOnMeshNode(node, 80.0f);
	}
}

void EditorScene::FocusSelectedLight()
{
	if (mSelectedLightNodeInfo)
	{
		IInstanceNode* node = mSelectedLightNodeInfo->InstanceNode;
		FocusOnMeshNode(node, 30.0f);
	}
}

bool EditorScene::DeleteLight(u32 id)
{
	SLightNodeInfo* info = GetLightNodeInfoById(id);
	if (!info)
		return false;

	ILightNode* lightNode = info->Node;
	auto it = mNodeIdMap.find(lightNode);
	if (it == mNodeIdMap.end())
		return false;
	mNodeIdMap.erase(it);

	info->Node->destroy();
	info->InstanceNode->destroy();

	auto it2 = mLightNodeInfos.find(id);
	if (it2 == mLightNodeInfos.end())
		return false;
	mLightNodeInfos.erase(it2);
	

	mSelectedLightNodeInfo = nullptr;
	return true;
}



