#include<Windows.h>
#include<GameFinal.h>

#pragma comment(lib, "GameFinal.lib")
#pragma comment(lib, "winmm.lib")

using namespace gf;

const u32 SCREEN_WIDTH = 1024;
const u32 SCREEN_HEIGHT = 768;
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
	settings.MultiSamplingCount = 1;
	settings.MultiSamplingQuality = 0;

	IApplication* device = gf::createDevice(EDT_DIRECT3D11, SCREEN_WIDTH, SCREEN_HEIGHT, EWS_NONE, true, settings);
	IVideoDriver* driver = device->getVideoDriver();

	math::SAxisAlignedBox smgr_size;
	smgr_size.Center = XMFLOAT3(0, 0, 0);
	smgr_size.Extents = XMFLOAT3(50.0f, 50.0f, 50.0f);

	ISceneManager* smgr = device->createSceneManager(smgr_size);
	IMeshManager* meshManager = driver->getMeshManager();
	IMaterialManager* materialManager = driver->getMaterialManager();
	ITextureManager* textureManager = driver->getTextureManager();

	IResourceGroupManager* resourceGroupManager = driver->getResourceGroupManager();
	resourceGroupManager->init("Resources.cfg");
	//resourceGroupManager->loadResourceGroup("General");

	ITextureCube* skyTexture = textureManager->loadCubeTexture("Snow.dds");
	//smgr->setSkyDome(skyTexture);

	ISceneNode* emptyNode = smgr->addEmptyNode();
	emptyNode->translate(0, 3.0f, 0);

	ISimpleMesh* cubeMesh = meshManager->createCubeMesh("cube1");
	IMeshNode* cubeMeshNode = smgr->addMeshNode(cubeMesh, nullptr, emptyNode, false, XMFLOAT3(0, 0.0f, 0));
	cubeMeshNode->setMaterialName("test/material01");

	ISimpleMesh* planeMesh = meshManager->createPlaneMesh("plane1", 10.0, 10.0f, 50, 50, 10.0f, 10.0f);
	IMeshNode* planeMeshNode = smgr->addMeshNode(planeMesh, nullptr, nullptr, true);
	planeMeshNode->setMaterialName("test/ground_material");

	IAnimatedMesh* animMesh = meshManager->getAnimatedMesh("lxq.mesh");
	IAnimatedMeshNode* animNode = smgr->addAnimatedMeshNode(animMesh);
	animNode->scale(0.02f, 0.02f, 0.02f);

	IModelMesh* heroMesh = meshManager->getModelMesh("hero.mesh");
	IMeshNode* heroNode = smgr->addModelMeshNode(heroMesh, nullptr, true, XMFLOAT3(20.0f, 5.0f, 5.0f), 
		XMFLOAT3(0, 0, 0), XMFLOAT3(0.01f, 0.01f, 0.01f));

//	heroNode->scale(0.01f, 0.01f, 0.01f);
//	heroNode->translate(2.0f, 0.5f, 0);

	// create sampler state
	SSamplerDesc samplerDesc;
	samplerDesc.Filter = ESF_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = EAM_WRAP;
	samplerDesc.AddressV = EAM_WRAP;
	samplerDesc.AddressW = EAM_WRAP;
	ISampler* sampler = driver->getSamplerManager()->create(std::string("sampler1"), samplerDesc);

	IPipeline* pipeline = driver->getPipelineManager()->get("test/pipeline01");

	ILightNode* light = smgr->addPointLight(1, nullptr, true, XMFLOAT3(2.0f, 5.0f, -3.0f), 100.0f);
	light->setSpecular(XMFLOAT4(1.0f, 1.0f, 1.0f, 32.0f));
	light->setDiffuse(XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));
	light->setAttenuation(1.0f, 0.0f, 0.0f);
	light->enableShadow(false);

	ICameraNode* camera = smgr->addFpsCameraNode(1, nullptr, XMFLOAT3(0, 1.0f, -4.0f), XMFLOAT3(0, 1.0f, 0.0f));
	camera->setNearZ(0.1f);
	IPipeline* blurPipeline = IPipelineManager::getInstance()->get("test/posteffect_pipeline");

	SCompositorCreateParam param;
	
	/*
	param.Blur.PassCount = 10;
	param.Blur.TexelDistance = 3.0f;
	param.Blur.Algorithm = SCompositorCreateParam::EBLUR_NEAR_POINTS_4;
	*/
	
	param.Bloom.BlurPassCount = 2;
	param.Bloom.BlurTexelDistance = 2.0f;
	param.Bloom.BrightnessThreshold = 0.8f;
	param.Bloom.BlurTextureWidth = 400;
	param.Bloom.BlurTextureHeight = 300;
	

	//param.MotionBlur.SampleNum = 4;
	ICompositor* compositor = smgr->createCompositor(ECT_BLOOM, param);
	smgr->addCompositor(compositor);
	
	f32 rotx = 0;
	f32 roty = 0;
	f32 rotz = 0;

	char caption[200];
	ITimer* timer = device->getTimer();
	timer->reset();

	const f32 color2[] = { 0.0f, 0.0f, 0.0f, 1.0f };

	while (device->run())
	{
		const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		driver->beginScene(true, true, clearColor);

		f32 dt = timer->tick();

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

		cubeMeshNode->resetTransform();
		cubeMeshNode->scale(2.0f);
		cubeMeshNode->transform(rotM);
		animNode->addTime(dt * 3000.0f);

		updateCamera(camera, dt);

		if (GetAsyncKeyState(0x31) & 0x8000)
			compositor->setIntAttribute(ECA_ALGORITHM, SCompositorCreateParam::EBLUR_NEAR_POINTS_4);
		if (GetAsyncKeyState(0x32) & 0x8000)
			compositor->setIntAttribute(ECA_ALGORITHM, SCompositorCreateParam::EBLUR_NEAR_POINTS_8);
		if (GetAsyncKeyState(0x33) & 0x8000)
			compositor->setIntAttribute(ECA_ALGORITHM, SCompositorCreateParam::EBLUR_GAUSSIAN);

		smgr->update(dt);

		smgr->drawAll();

		driver->endScene();

		sprintf(caption, "FPS:%f", getFps(dt));
		device->setWindowCaption(caption);
	}

	smgr->destroy();
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