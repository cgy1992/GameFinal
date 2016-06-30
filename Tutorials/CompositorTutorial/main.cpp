#include <GameFinal.h>
#pragma comment(lib, "GameFinal.lib")

using namespace gf;

class MyCompositor : public ICompositor
{
	virtual void render(ISceneManager* smgr, bool lastCompositor) {
		// TODO: Do your post effect's processing
	}
};

ISceneManager* setupScene(IApplication* device) {

	// create scene manager
	ISceneManager* smgr = device->createSceneManager();

	// get mesh manager
	IMeshManager* meshManager = IMeshManager::getInstance();

	// set up ground.
	ISimpleMesh* groundMesh = meshManager->createPlaneMesh("ground", 100.0f, 100.0f, 10, 10, 50, 50);
	IMeshNode* groundNode = smgr->addMeshNode(groundMesh, "ground_material", nullptr, true);

	// create a sphere
	ISimpleMesh* sphereMesh = meshManager->createSphereMesh("sphere", 1.0f, 100, 100);
	IMeshNode* sphereNode = smgr->addMeshNode(sphereMesh, "sphere_material", nullptr, true, XMFLOAT3(-1.2f, 1.0f, 0));

	// create a box
	ISimpleMesh* boxMesh = meshManager->createCubeMesh("box", 1.2f, 3.0f, 1.2f);
	IMeshNode* boxNode = smgr->addMeshNode(boxMesh, "box_material", nullptr, true, XMFLOAT3(1.0f, 1.5f, 1.0f));

	// create a teapot
	IModelMesh* teapotMesh = meshManager->getModelMesh("teapot.mesh");
	IMeshNode* teapot = smgr->addModelMeshNode(teapotMesh, nullptr, true, XMFLOAT3(1.2f, 0, -1.0f));
	teapot->setMaterialName("teapot_material");

	// add point light
	ILightNode* light = smgr->addPointLight(1, nullptr, false, XMFLOAT3(0, 4.0f, 0), 100);
	light->setSpecular(XMFLOAT4(10.0f, 10.0, 10.0f, 32.0f));
	light->setDiffuse(XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));
	
	light->setAttenuation(1.0f, 0.05f, 0);
	light->enableShadow(true);

	sphereNode->addShadow(1);
	boxNode->addShadow(1);
	teapot->addShadow(1);

	// create a fps camera node
	IFpsCameraNode* camera = smgr->addFpsCameraNode(1, nullptr, XMFLOAT3(0, 1.0f, -6.0f),
		XMFLOAT3(0, 1.0f, 0.0f), XMFLOAT3(0, 1.0f, 0));
	camera->setNearZ(1.0f);
	camera->setFarZ(1000.0f);

	// set ambient in the environment.
	smgr->setAmbient(XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));

	// set sky dome
	smgr->setSkyDome("Snow.dds");

	//param.Bloom.BlurPassCount = 2;
	/*
	SCompositorCreateParam param;
	param.Bloom.BlurPassCount = 4;
	param.Bloom.BlurTexelDistance = 2.0f;
	param.Bloom.BrightnessThreshold = 0.7f;
	param.Bloom.BlurTextureWidth = 400;
	param.Bloom.BlurTextureHeight = 300;
	*/

	/*
	SCompositorCreateParam param;
	param.Blur.Algorithm = SCompositorCreateParam::EBLUR_GAUSSIAN;
	param.Blur.TexelDistance = 2.0f;
	param.Blur.PassCount = 5;
	*/

	SCompositorCreateParam param;
	param.MotionBlur.SampleNum = 4;

	//param.MotionBlur.SampleNum = 4;
	ICompositor* compositor = smgr->createCompositor(ECT_BLUR, param);
	smgr->addCompositor(compositor);

	return smgr;
}

int main()
{
	IApplication* device = createDevice(EDT_DIRECT3D11, 800, 600);
	IVideoDriver* driver = device->getVideoDriver();
	IResourceGroupManager* resourceGroupManager = driver->getResourceGroupManager();
	resourceGroupManager->init("Resources.cfg");

	IInputDriver* input = device->getInputDriver();

	ISceneManager* smgr = setupScene(device);

	ITimer* timer = device->getTimer();
	timer->reset();

	const f32 color[] = { 0, 0, 0, 1.0f };
	while (device->run()) {
		float dt = timer->tick();
		if (input->keyDown(GVK_ESCAPE))
			break;
		driver->beginScene(true, true, color);
		smgr->update(dt);
		
		smgr->drawAll();
		driver->endScene();
	}

	smgr->destroy();
	device->drop();

	return 0;
}

