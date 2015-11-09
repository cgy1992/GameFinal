#include <GameFinal.h>

#pragma comment(lib, "GameFinal.lib")
#pragma comment(lib, "winmm.lib")

using namespace gf;

ISceneManager* setupScene(IDevice* device) {

	// create scene manager
	ISceneManager* smgr = device->createSceneManager();

	// get mesh manager
	IMeshManager* meshManager = IMeshManager::getInstance();

	// set up ground.
	ISimpleMesh* groundMesh = meshManager->createPlaneMesh("ground", 100.0f, 100.0f, 10, 10, 50, 50);
	IMeshNode* groundNode = smgr->addMeshNode(groundMesh, "ground_material", nullptr, true);

	IModelMesh* apartmentMesh = meshManager->getModelMesh("apartmentB.mesh");
	IMeshNode* apartmentNode = smgr->addModelMeshNode(apartmentMesh, nullptr, true, XMFLOAT3(0, 0, 0));

	// add directional light
	ILightNode* light = smgr->addDirectionalLight(1, nullptr, XMFLOAT3(5.0f, -5.0f, 2.0f));
	light->setSpecular(XMFLOAT4(1.0f, 1.0, 1.0f, 32.0f));
	light->setDiffuse(XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));
	light->enableShadow(true);

	apartmentNode->addShadow(1);

	// create a camera node
	ICameraNode* camera = smgr->addCameraNode(1, nullptr, XMFLOAT3(15.0f, 20.0f, -25.0f), XMFLOAT3(0, 1.0f, 0.0f), XMFLOAT3(0, 1.0f, 0));
	camera->setShadowRange(100.0f);

	// set ambient in the environment.
	smgr->setAmbient(XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));

	return smgr;
}

int main()
{
	SDeviceContextSettings settings;
	settings.MultiSamplingCount = 4;
	settings.MultiSamplingQuality = 32;

	IDevice* device = createDevice(EDT_DIRECT3D11, 800, 600, EWS_NONE, true, settings);
	IVideoDriver* driver = device->getVideoDriver();
	IInputDriver* input = device->getInputDriver();
	IResourceGroupManager* resourceGroupManager = driver->getResourceGroupManager();
	resourceGroupManager->init("Resources.cfg");

	ISceneManager* smgr = setupScene(device);

	ITimer* timer = device->getTimer();
	timer->reset();

	const f32 color[] = { 0, 0, 0, 1.0f };
	while (device->run()) {
		float dt = timer->tick();
		if (input->keyDown(GVK_ESCAPE))
			break;
		smgr->update(dt);
		driver->beginScene(true, true, color);
		smgr->drawAll();
		driver->endScene();
	}

	smgr->destroy();
	device->drop();

	return 0;
}

