#include "stdafx.h"
#include "CEditorScene.h"
#include "CXmlFileManager.h"

CEditorScene* CEditorScene::_instance = nullptr;

CEditorScene::CEditorScene(IDevice* device)
:mDevice(device)
, mSceneManager(nullptr)
, mCamera(nullptr)
, mCurrentMeshNodeInfo(nullptr)
, mCurrentBoundingShape(nullptr)
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


CEditorScene::~CEditorScene()
{
	mSceneManager->destroy();
}

void CEditorScene::setupInitialScene()
{
	math::SAxisAlignedBox aabb;
	aabb.Center = XMFLOAT3(0, 0.0f, 0);
	aabb.Extents = XMFLOAT3(3000.0f, 1000.0f, 3000.0f);

	ISceneManager* smgr = mDevice->createSceneManager(aabb);
	mSceneManager = smgr;

	ISimpleMesh* planeMesh = mMeshManager->createPlaneMesh("ground_plane", 40, 40, 40, 40);
	IMeshNode* planeNode = smgr->addMeshNode(planeMesh, nullptr, nullptr, true);
	
	planeNode->setMaterialName("ground_wire_material");
	
	mCamera = smgr->addFpsCameraNode(1, nullptr, XMFLOAT3(0, 10.0f, -15.0f),
		XMFLOAT3(0, 5, 0.0f), XMFLOAT3(0, 1.0f, 0), true);
	//mCamera->setShadowRange(300.0f);

	ILightNode* light = smgr->addDirectionalLight(1, nullptr, XMFLOAT3(3.0f, -2.0f, 1.5f));
	light->setSpecular(XMFLOAT4(1.0f, 1.0f, 1.0f, 32.0f));
	light->setDiffuse(XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));
	light->enableShadow(true);
	smgr->setAmbient(XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));

	mCubeMesh = mMeshManager->createCubeMesh("cube_bounding");
	mSphereMesh = mMeshManager->createSphereMesh("sphere_bounding");
	mCylinderMesh = mMeshManager->createCylinderMesh("cylinder_bounding");

	mTimer = mDevice->getTimer();
	smgr->update(0);

	// init all mesh names
	std::vector<std::string> names;
	mResourceGroupManager->listResourceNames(ERFT_MESH, names);
	std::set<std::string> nameSet;
	for (u32 i = 0; i < names.size(); i++)
	{
		nameSet.insert(names[i]);
	}

	for (auto it = nameSet.begin(); it != nameSet.end(); it++)
	{
		std::string s = *it;
		mMeshNames.push_back(s);
	}

	for (u32 i = 0; i < mMeshNames.size(); i++)
	{
		mMeshNodeInfos.insert(std::make_pair(mMeshNames[i], SMeshNodeInfo()));
	}

	CXmlFileManager::Read("main.physics.xml");
	for (auto it = mMeshNodeInfos.begin(); it != mMeshNodeInfos.end(); it++)
	{
		it->second.SetVisible(false);
	}
}

void CEditorScene::update(f32 dt)
{
	//CEditorWindow* window = CEditorWindow::getInstance();
	//if (window)
	//	window->OnKeyBoard(dt);

	UpdateCamera(dt);
	mSceneManager->update(dt);
}

void CEditorScene::drawAll()
{
	mSceneManager->drawAll();
}

void CEditorScene::UpdateCamera(f32 delta)
{
	ICameraNode* camera = mCamera;
	const f32 CAMERA_MOVE_UNIT = 10.0f;
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

SMeshNodeInfo* CEditorScene::SelectMesh(const std::string meshName)
{
	SMeshNodeInfo* info = nullptr;
	auto it = mMeshNodeInfos.find(meshName);
	if (it == mMeshNodeInfos.end())
		return nullptr;

	info = &it->second;
	if (!info->Node)
	{
		IModelMesh* mesh = mMeshManager->getModelMesh(meshName, true);
		IMeshNode* node = mSceneManager->addModelMeshNode(mesh, nullptr, true);
		SMeshNodeInfo meshNodeInfo;
		info->Node = node;
	}

	if (mCurrentMeshNodeInfo)
		mCurrentMeshNodeInfo->SetVisible(false);

	mCurrentMeshNodeInfo = info;
	mCurrentMeshNodeInfo->SetVisible(true);
	return mCurrentMeshNodeInfo;
}

SBoxBounding* CEditorScene::AddBoxBounding()
{
	if (!mCurrentMeshNodeInfo)
		return nullptr;

	IMesh* mesh = mCurrentMeshNodeInfo->Node->getMesh();
	math::SAxisAlignedBox aabb = mesh->getAabb();
	SBoxBounding* box = new SBoxBounding;
	box->Center = aabb.Center;
	box->Size = XMFLOAT3(aabb.Extents.x * 2.0f, aabb.Extents.y * 2.0f, aabb.Extents.z * 2.0f);

	IMeshNode* node = mSceneManager->addMeshNode(mCubeMesh,
		nullptr, nullptr, false, aabb.Center, XMFLOAT3(0, 0, 0),
		box->Size);
	
	node->setMaterialName("bounding_wire_material");
	box->WireFrameNode = node;
	mCurrentMeshNodeInfo->BoundingShapes.push_back(box);
	
	return box;
}

SCylinderBounding* CEditorScene::AddCylinderBounding()
{
	if (!mCurrentMeshNodeInfo)
		return nullptr;

	IMesh* mesh = mCurrentMeshNodeInfo->Node->getMesh();
	math::SAxisAlignedBox aabb = mesh->getAabb();

	SCylinderBounding* cylinder = new SCylinderBounding;
	cylinder->Center = aabb.Center;
	cylinder->Height = aabb.Extents.y * 2.0f;
	cylinder->Radius = max(aabb.Extents.x, aabb.Extents.z);

	IMeshNode* node = mSceneManager->addMeshNode(mCylinderMesh,
		nullptr, nullptr, false, aabb.Center, XMFLOAT3(0, 0, 0),
		XMFLOAT3(cylinder->Radius, cylinder->Height, cylinder->Radius));

	node->setMaterialName("bounding_wire_material");
	cylinder->WireFrameNode = node;
	mCurrentMeshNodeInfo->BoundingShapes.push_back(cylinder);

	return cylinder;
}

SBoundingShape* CEditorScene::GetBoundingShapeByIndex(u32 index)
{
	if (!mCurrentMeshNodeInfo)
		return nullptr;

	return mCurrentMeshNodeInfo->BoundingShapes[index];
}

void CEditorScene::UpdateWireFrameNode(SBoundingShape* shape)
{
	E_BOUNDING_CATEGORY category = shape->getCategory();
	IMeshNode* node = shape->WireFrameNode;
	XMFLOAT4X4 mat = shape->getTransform();
	XMMATRIX M = XMLoadFloat4x4(&mat);

	node->setTransform(M);
	node->update();
}

void CEditorScene::DeleteBounding(u32 index)
{
	if (!mCurrentMeshNodeInfo)
		return;

	SBoundingShape* shape = mCurrentMeshNodeInfo->BoundingShapes[index];
	shape->WireFrameNode->destroy();
	mCurrentMeshNodeInfo->BoundingShapes.erase(mCurrentMeshNodeInfo->BoundingShapes.begin() + index);
	delete shape;
	mCurrentBoundingShape = nullptr;
}

SBoundingShape* CEditorScene::SelectBoundingShape(u32 index)
{
	SBoundingShape* shape = GetBoundingShapeByIndex(index);
	if (mCurrentBoundingShape)
	{
		IMeshNode* node = mCurrentBoundingShape->WireFrameNode;
		node->setMaterialName("bounding_wire_material");
	}

	mCurrentBoundingShape = shape;
	mCurrentBoundingShape->WireFrameNode->setMaterialName("selected_bounding_wire_material");
	return mCurrentBoundingShape;
}

