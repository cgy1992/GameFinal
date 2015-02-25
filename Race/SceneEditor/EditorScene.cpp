#include "stdafx.h"
#include "EditorScene.h"

using namespace gf;

EditorScene* EditorScene::_instance = nullptr;


EditorScene::EditorScene(IDevice* device)
:mDevice(device)
, mSceneManager(nullptr)
, mGroundSize(300.0f)
, mAddedNode(nullptr)
, mPickingCubeNode(nullptr)
, mSelectedCubeNode(nullptr)
, mSelectedNode(nullptr)
, mCameraFocusingObject(false)
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

	mCamera = smgr->addFpsCameraNode(1, nullptr, XMFLOAT3(0, 30.0f, -4.0f),
		XMFLOAT3(0, 30.0f, 0.0f), XMFLOAT3(0, 1.0f, 0), true);
	mCamera->setShadowRange(300.0f);

	ISimpleMesh* groundMesh = mMeshManager->createPlaneMesh("ground", mGroundSize, mGroundSize, 50, 50, 10.0f, 10.0f);
	IMeshNode* groundNode = smgr->addMeshNode(groundMesh, nullptr, nullptr, false);
	groundNode->setMaterialName("ground_material");

	ITextureCube* skyTexture = mTextureManager->loadCubeTexture("skybox1.dds");
	smgr->setSkyDome(skyTexture);

	mTimer = mDevice->getTimer();

	ISimpleMesh* cubeMesh = mMeshManager->createCubeMesh("cube1");

	mSelectedCubeNode = smgr->addMeshNode(cubeMesh, nullptr, nullptr, false);
	mSelectedCubeNode->setMaterialName("selected_cube_material");
	mSelectedCubeNode->setVisible(false);

	mPickingCubeNode = smgr->addMeshNode(cubeMesh, nullptr, nullptr, false);
	mPickingCubeNode->setMaterialName("picking_cube_material");
	mPickingCubeNode->setVisible(false);

	

	mSceneManager->update(0);

}

void EditorScene::update(f32 dt)
{
	updateCamera(dt);
	//changeTreePosition();
	mSceneManager->update(dt);
}

void EditorScene::drawAll()
{
	mSceneManager->drawAll();
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

void EditorScene::changeTreePosition()
{
	if (!mAddedNode)
		return;

	IDevice* device = IDevice::getInstance();

	POINT point;
	GetCursorPos(&point);
	u32 sx = point.x;
	u32 sy = point.y;
	device->screenToClient(sx, sy);

	sx = (static_cast<f32>(sx - 200)) / BufferWndWidth * device->getClientWidth();
	sy = (static_cast<f32>(sy)) / BufferWndHeight * device->getClientHeight();

	math::SRay ray = mSceneManager->getPickingRay(sx, sy);

	const f32 EPSILON = 0.0001f;

	if (fabs(ray.Direction.y) < EPSILON)
		return;

	f32 t = (0 - ray.Origin.y) / ray.Direction.y;
	if (t < 0)
		return;

	f32 x = ray.Origin.x + t * ray.Direction.x;
	f32 z = ray.Origin.z + t * ray.Direction.z;

	if (x < -mGroundSize * 0.5f || x > mGroundSize * 0.5f
		|| z < -mGroundSize * 0.5f || z > mGroundSize * 0.5f)
		return;

	mAddedNode->setPosition(x, 0, z);
}

void EditorScene::addTreeNode(u32 sx, u32 sy)
{
	IDevice* device = IDevice::getInstance();
	IMeshManager* meshManager = IMeshManager::getInstance();
	IModelMesh* treeMesh = meshManager->getModelMesh("roundtreeA.mesh", true);

	sx = (static_cast<f32>(sx - 200)) / BufferWndWidth * device->getClientWidth();
	sy = (static_cast<f32>(sy)) / BufferWndHeight * device->getClientHeight();

	math::SRay ray = mSceneManager->getPickingRay(sx, sy);

	const f32 EPSILON = 0.0001f;

	if (fabs(ray.Direction.y) < EPSILON)
		return;

	f32 t = (0 - ray.Origin.y) / ray.Direction.y;
	if (t < 0)
		return;

	f32 x = ray.Origin.x + t * ray.Direction.x;
	f32 z = ray.Origin.z + t * ray.Direction.z;

	if (x < -mGroundSize * 0.5f || x > mGroundSize * 0.5f
		|| z < -mGroundSize * 0.5f || z > mGroundSize * 0.5f)
		return;

	IMeshNode* treeNode = mSceneManager->addModelMeshNode(treeMesh, nullptr, true, XMFLOAT3(x, 0, z));
	treeNode->addShadow(1);
}

EditorScene::~EditorScene()
{
	mSceneManager->destroy();
}

void EditorScene::PrepareAddingObject(const std::string& meshName)
{
	IModelMesh* treeMesh = mMeshManager->getModelMesh(meshName, true);
	IMeshNode* treeNode = mSceneManager->addModelMeshNode(treeMesh, nullptr, false);
	treeNode->addShadow(1);
	mAddedNode = treeNode;
}

void EditorScene::AddObject()
{
	mAddedNode = nullptr;
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

void EditorScene::ChangeAddedObjectPosition(u32 sx, u32 sy)
{
	if (!mAddedNode)
		return;

	math::SRay ray = computePickingRay(sx, sy);

	const f32 EPSILON = 0.0001f;

	if (fabs(ray.Direction.y) < EPSILON)
		return;

	f32 t = (0 - ray.Origin.y) / ray.Direction.y;
	if (t < 0)
		return;

	f32 x = ray.Origin.x + t * ray.Direction.x;
	f32 z = ray.Origin.z + t * ray.Direction.z;

	if (x < -mGroundSize * 0.5f || x > mGroundSize * 0.5f
		|| z < -mGroundSize * 0.5f || z > mGroundSize * 0.5f)
		return;

	mAddedNode->setPosition(x, 0, z);
}

void EditorScene::PickingObject(u32 sx, u32 sy)
{
	math::SRay ray = computePickingRay(sx, sy);

	f32 dist;
	ISceneNode* node = mSceneManager->intersectRay(ray, &dist, ENT_SOLID_NODE);

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

void EditorScene::SelectObject(u32 sx, u32 sy)
{
	math::SRay ray = computePickingRay(sx, sy);

	f32 dist;
	ISceneNode* node = mSceneManager->intersectRay(ray, &dist, ENT_SOLID_NODE);

	if (node)
	{
		mSelectedCubeNode->setVisible(true);
		mSelectedNode = node;

		math::SOrientedBox obb = node->getOrientedBox();
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
	else
	{
		mSelectedCubeNode->setVisible(false);
		mSelectedNode = nullptr;
	}
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

