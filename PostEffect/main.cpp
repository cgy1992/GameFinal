#include<Windows.h>
#include<GameFinal.h>

#pragma comment(lib, "GameFinal.lib")
#pragma comment(lib, "winmm.lib")

using namespace gf;

const u32 SCREEN_WIDTH = 800;
const u32 SCREEN_HEIGHT = 600;
const f32 CAMERA_MOVE_UNIT = 5.0f;
const f32 CAMERA_ROTATE_UNIT = 1.0f;

void updateCamera(ICameraNode* camera, f32 delta);

f32 getFps(float dt)
{
	static u32 frameCount = 0;
	static f32 elapseTime = 0.0f;
	static f32 fps = 1.0f;
	elapseTime += dt;
	frameCount++;
	if (elapseTime > 0.1f)
	{
		fps = static_cast<f32>(frameCount) / elapseTime;
		frameCount = 0;
		elapseTime = 0.0f;
	}
	return fps;
}

ISimpleMesh* createQuadMesh(IMeshManager* meshManager);

int main()
{
	SDeviceContextSettings settings;
	settings.MultiSamplingCount = 4;
	settings.MultiSamplingQuality = 32;

	IDevice* device = gf::createDevice(EDT_DIRECT3D11, 800, 600, EWS_NONE, true, settings);
	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->getSceneManager();
	IMeshManager* meshManager = driver->getMeshManager();
	IMaterialManager* materialManager = driver->getMaterialManager();
	ITextureManager* textureManager = driver->getTextureManager();

	IResourceGroupManager* resourceGroupManager = driver->getResourceGroupManager();
	resourceGroupManager->init("Resources.cfg");
	resourceGroupManager->loadResourceGroup("General");

	ISceneNode* emptyNode = smgr->addEmptyNode();
	emptyNode->translate(0, 3.0f, 0);
	emptyNode->scale(2.0f, 2.0f, 2.0f);

	ISimpleMesh* cubeMesh = meshManager->createCubeMesh("cube1");
	IMeshNode* cubeMeshNode = smgr->addMeshNode(cubeMesh, nullptr, emptyNode, XMFLOAT3(0, 0.0f, 0));
	//cubeMeshNode->scale(0.5f, 0.5f, 0.5f);
	cubeMeshNode->setMaterialName("test/material01");
	//cubeMeshNode->remove();

	ISimpleMesh* planeMesh = meshManager->createPlaneMesh("plane1", 10.0, 10.0f, 50, 50, 10.0f, 10.0f);
	IMeshNode* planeMeshNode = smgr->addMeshNode(planeMesh, nullptr);
	planeMeshNode->setMaterialName("test/ground_material");

	IAnimatedMesh* animMesh = meshManager->getAnimatedMesh("lxq.mesh");
	IAnimatedMeshNode* animNode = smgr->addAnimatedMeshNode(animMesh);
	animNode->scale(0.02f, 0.02f, 0.02f);

	IModelMesh* heroMesh = meshManager->getModelMesh("hero.mesh");
	IMeshNode* heroNode = smgr->addModelMeshNode(heroMesh);

	heroNode->scale(0.01f, 0.01f, 0.01f);
	heroNode->translate(2.0f, 0.5f, 0);

	ISceneNode* node1 = smgr->addEmptyNode();
	node1->addChild(heroNode);
	node1->addChild(animNode);

	// create sampler state
	SSamplerDesc samplerDesc;
	samplerDesc.Filter = ESF_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = EAM_WRAP;
	samplerDesc.AddressV = EAM_WRAP;
	samplerDesc.AddressW = EAM_WRAP;
	ISampler* sampler = driver->getSamplerManager()->create(std::string("sampler1"), samplerDesc);

	IPipeline* pipeline = driver->getPipelineManager()->get("test/pipeline01");
	//pipeline->setSampler(std::string("sampleType"), sampler);

	ILightNode* light = smgr->addLightNode(1);
	light->setType(ELT_POINT);
	light->setAmbient(XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
	light->setPosition(2.0f, 5.0f, -3.0f);
	light->setSpecular(XMFLOAT4(1.0f, 1.0f, 1.0f, 32.0f));
	light->setDiffuse(XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));
	light->setAttenuation(1.0f, 0.0f, 0.0f);
	light->setRange(100.0f);

	ICameraNode* camera = smgr->addFpsCameraNode(1, nullptr, XMFLOAT3(0, 1.0f, -4.0f), XMFLOAT3(0, 1.0f, 0.0f));

	IMaterial* quadMaterial = materialManager->get("test/posteffect_material");
	ISimpleMesh* quadMesh = createQuadMesh(meshManager);
	IMeshNode* quadNode = smgr->addMeshNode(quadMesh, quadMaterial);
	quadNode->setNeedCulling(false);
	
	IRenderTarget* pRenderTarget = textureManager->getRenderTarget("target1");
	//IDepthStencilSurface* pDepthSurface = textureManager->createDepthStencilSurface("depth_surface1", 800, 600,
	//	16, 0, false, 1, 0, true);

	IDepthStencilSurface* pDepthSurface = textureManager->getDepthStencilSurface("depth_stencil_01");

	//IRenderTarget* pRenderTarget = textureManager->createRenderTarget("target1", true);
	//quadMaterial->setTexture(0, pRenderTarget);

	f32 rotx = 0;
	f32 roty = 0;
	f32 rotz = 0;

	//driver->setViewport(0, 0, 400, 300);

	char caption[200];
	ITimer* timer = device->getTimer();
	timer->reset();

	const f32 color2[] = { 0.0f, 0.0f, 0.0f, 1.0f };

	while (device->run())
	{
		const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		driver->beginScene(true, true, clearColor);
		pRenderTarget->clear(clearColor);
		pDepthSurface->clearDepth(1.0f);

		//driver->setRenderTarget(pRenderTarget);
		//driver->setDepthStencilSurface(nullptr);
		driver->setRenderTargetAndDepthStencil(pRenderTarget, pDepthSurface);

		float dt = timer->tick() * 0.001f;

		rotx += dt * 2.0f;
		roty += dt * 1.0f;
		rotz += dt * 0.5f;
		if (rotx > XM_2PI) rotx -= XM_2PI;
		if (roty > XM_2PI) roty -= XM_2PI;
		if (rotz > XM_2PI) rotz -= XM_2PI;

		XMMATRIX Mx = XMMatrixRotationX(rotx);
		XMMATRIX My = XMMatrixRotationY(roty);
		XMMATRIX Mz = XMMatrixRotationZ(rotz);
		XMMATRIX rotM = Mx * My * Mz;

		cubeMeshNode->setOrientation(rotM);
		animNode->addTime(dt * 3000.0f);

		updateCamera(camera, dt);

		smgr->update(dt);

		smgr->draw(node1);
		smgr->draw(cubeMeshNode);
		smgr->draw(planeMeshNode);

		//driver->setDefaultRenderTarget();
		driver->setDefaultRenderTargetAndDepthStencil();
		driver->clearDepth(1.0f);

		smgr->draw(quadNode);

		driver->endScene();

		sprintf(caption, "FPS:%f", getFps(dt));
		device->setWindowCaption(caption);
	}

	device->drop();

	return 0;
}

void updateCamera(ICameraNode* camera, f32 delta)
{
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

ISimpleMesh* createQuadMesh(IMeshManager* meshManager)
{
	XMFLOAT3 vertices[6];
	vertices[0] = XMFLOAT3(-1.0f, 1.0f, 0.0f);
	vertices[1] = XMFLOAT3(1.0f, 1.0f, 0.0f);
	vertices[2] = XMFLOAT3(1.0f, -1.0f, 0.0f);

	vertices[3] = XMFLOAT3(-1.0f, 1.0f, 0.0f);
	vertices[4] = XMFLOAT3(1.0f, -1.0f, 0.0f);
	vertices[5] = XMFLOAT3(-1.0f, -1.0f, 0.0f);

	ISimpleMesh* mesh = meshManager->createSimpleMesh("quad", (void*)&vertices[0], 0, 6, sizeof(XMFLOAT3), 0, math::SAxisAlignedBox(), false);
	return mesh;
}