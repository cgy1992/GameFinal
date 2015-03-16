#include "stdafx.h"
#include "EditorScene.h"
#include "EditorWindow.h"
#include "Enums.h"

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

	ILightNode* light = smgr->addDirectionalLight(1, nullptr, XMFLOAT3(3.0f, -2.0f, 1.5f));
	light->setSpecular(XMFLOAT4(1.0f, 1.0f, 1.0f, 32.0f));
	light->setDiffuse(XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f));
	light->enableShadow(true);

	
	//smgr->setAmbient(XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f));
	//light->setDiffuse(XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f));

	mCamera = smgr->addFpsCameraNode(1, nullptr, XMFLOAT3(0, 30.0f, -4.0f),
		XMFLOAT3(0, 30.0f, 0.0f), XMFLOAT3(0, 1.0f, 0), true);
	mCamera->setShadowRange(300.0f);

	ISimpleMesh* groundMesh = mMeshManager->createPlaneMesh("ground", mGroundSize, mGroundSize, 50, 50, 10.0f, 10.0f);
	IMeshNode* groundNode = smgr->addMeshNode(groundMesh, nullptr, nullptr, false);
	groundNode->setMaterialName("ground_material");
	groundNode->setTag(MESH_NODE_TAG);

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

	//mPointLightCollectionNode->removeShadow(1);

	mSceneManager->update(0);
	setupTerrain();


	IVideoDriver* driver = IVideoDriver::getInstance();
	mVideoDriver->setDeferredShading(true);
	driver->setDeferredShadingAlgorithm(EDSA_CS_TILE_BASED_DEFERRED_SHADING);

	IShaderManager* shaderManager = IShaderManager::getInstance();
	mTileBasedDSShader = shaderManager->load(EST_COMPUTE_SHADER, "defer_shader_cs.hlsl", "cs_main");
	smgr->setTileBasedDeferredShadingCS(mTileBasedDSShader);


	SDirectionalLight lightData;
	light->getLightData(&lightData);
	mTileBasedDSShader->setRawData("gLight", &lightData, sizeof(SDirectionalLight));

	mVideoDriver->setDeferredShading(false);

	mDeferredShading = true;

}

void EditorScene::setupTerrain()
{
	IMeshManager* meshManager = IMeshManager::getInstance();

	std::string rawFileName("terrain-heightmap4.raw");
	ITerrainMesh* terrainMesh = meshManager->createTerrainMesh("terrain", rawFileName,
		10.0f, 1.0f, false, true, 1.0f);

	mTerrainNode = mSceneManager->addTerrainNode(terrainMesh);
	mTerrainNode->setMaterialName("terrain/terrain_material");
}

void EditorScene::update(f32 dt)
{
	OnKeyBoard(dt);
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


void EditorScene::updateCamera(f32 delta)
{
	ICameraNode* camera = mCamera;
	const f32 CAMERA_MOVE_UNIT = 30.0f;
	const f32 CAMERA_ROTATE_UNIT = 1.0f;

	if (mCameraFocusingObject)
		focusOnObject(delta);
	
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

u32 EditorScene::AddObject()
{
	if (!mAddedNode)
		return -1;

	SNodeInfo nodeInfo;
	nodeInfo.Id = getNextNodeSequence();
	nodeInfo.Static = false;
	nodeInfo.Node = mAddedNode;

	XMFLOAT3 pos = mAddedNode->getPosition();
	nodeInfo.Position = pos;

	mNodeInfos.insert(std::make_pair(nodeInfo.Id, nodeInfo));
	mNodeIdMap.insert(std::make_pair(mAddedNode, nodeInfo.Id));

	mAddedNode = nullptr;
	return nodeInfo.Id;
}

u32 EditorScene::AddLightNode()
{
	if (!mAddedLightNodeInfo)
		return -1;

	u32 id = mAddedLightNodeInfo->Id;
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
	nodeInfo.Static = true;
	nodeInfo.Node = mAddedNode;
	nodeInfo.Position = mAddedNode->getPosition();
	
	u32 id = collectionNodeInfo->AddNodeInfo(nodeInfo);
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
		mAddedNode->setPosition(position);
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

void EditorScene::focusOnObject(f32 dt)
{
	if (mSelectedNode == nullptr)
		return;

	XMFLOAT3 camPos = mCamera->getPosition();
	XMVECTOR camPos_v = XMLoadFloat3(&camPos);
	
	math::SOrientedBox obb = mSelectedNode->getOrientedBox();
	XMVECTOR objPos_v = XMLoadFloat3(&obb.Center);

	XMVECTOR dir_v = objPos_v - camPos_v;
	XMVECTOR dist_v = XMVector3Length(dir_v);
	f32 dist = XMVectorGetX(dist_v);
	mCamera->lookAt(obb.Center);

	if (dist < 30.0f)
	{
		mCameraFocusingObject = false;
	}
	else
	{
		f32 movement = dist * 20.0f * dt;
		if (dist - movement < 30.0f)
		{
			movement = dist - 30.0f + 0.01f;
		}
		mCamera->walk(movement);
	}
}

void EditorScene::BeginFocusingObject()
{
	if (mSelectedNode == nullptr)
	{
		mCameraFocusingObject = false;
	}
	else
	{
		mCameraFocusingObject = true;
	}
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

void EditorScene::OnKeyBoard(f32 delta)
{
	bool shift = false;
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
	{
		updateSelectedObjectTransform(delta);
	}
	else
	{
		updateCamera(delta);
	}
}

void EditorScene::updateSelectedObjectTransform(f32 delta)
{
	SNodeInfo* info = GetSelectedNodeInfo();
	if (!info)
		return;

	const f32 MOVE_UNIT = 10.0f;
	const f32 SCALING_UNIT = 5.0f;

	XMFLOAT3 look = mCamera->getLookVector();
	XMFLOAT3 up(0, 1.0f, 0);
	XMFLOAT3 right = mCamera->getRightVector();

	XMVECTOR look_v = XMVectorSet(look.x, 0, look.z, 0);
	look_v = XMVector4Normalize(look_v);
	
	XMStoreFloat3(&look, look_v);

	XMFLOAT3 movement(0, 0, 0);
	XMFLOAT3 scaling(0, 0, 0);

	if (GetAsyncKeyState('W') & 0x8000)
	{
		movement = math::VectorMultiply(look, delta * MOVE_UNIT);
	}

	if (GetAsyncKeyState('S') & 0x8000)
	{
		movement = math::VectorMultiply(look, -delta * MOVE_UNIT);
	}

	if (GetAsyncKeyState('A') & 0x8000)
	{
		movement = math::VectorMultiply(right, -delta * MOVE_UNIT);
	}

	if (GetAsyncKeyState('D') & 0x8000)
	{
		movement = math::VectorMultiply(right, delta * MOVE_UNIT);
	}

	if (GetAsyncKeyState('R') & 0x8000)
	{
		movement = math::VectorMultiply(up, delta * MOVE_UNIT);
	}

	if (GetAsyncKeyState('F') & 0x8000)
	{
		movement = math::VectorMultiply(up, -delta * MOVE_UNIT);
	}

	if (GetAsyncKeyState(VK_ADD) & 0x8000)
	{
		scaling = XMFLOAT3(1.0f, 1.0f, 1.0f);
	}
	
	if (GetAsyncKeyState(VK_SUBTRACT) & 0x8000)
	{
		scaling = XMFLOAT3(-1.0f, -1.0f, -1.0f);
	}

	scaling = math::VectorMultiply(scaling, delta * SCALING_UNIT);

	info->Position = math::VectorAdd(info->Position, movement);
	info->Scaling = math::VectorAdd(info->Scaling, scaling);

	EditorWindow* window = EditorWindow::getInstance();

	UpdateNodeInfo(info);
	window->mMeshNodePanel.ShowNodeInfo(info);

}

int EditorScene::AddCollectionNode(const std::string& meshName, int maxNum)
{
	IModelMesh* mesh = mMeshManager->getModelMesh(meshName, true);
	IInstanceCollectionNode* collectionNode = mSceneManager->addInstanceCollectionNode(mesh, nullptr, maxNum, 0);
	//collectionNode->setNeedCulling(false);

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

	SNodeInfo nodeInfo;
	nodeInfo.Id = getNextNodeSequence();
	nodeInfo.Node = collectionNode;
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

