#include <GameFinal.h>
#pragma comment(lib, "GameFinal.lib")

using namespace gf;

ISceneManager* setupScene(IDevice* device) {

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

	// add 100 point light
	for (u32 i = 0; i < 100; i++) {
		XMFLOAT3 pos(math::RandomFloat(-20.0, 20.0f), math::RandomFloat(3.0, 10.0f), math::RandomFloat(-20.0, 20.0f));
		XMFLOAT4 diffuse(math::RandomFloat(0, 0.3f), math::RandomFloat(0, 0.3f), math::RandomFloat(0, 0.3f), 1.0f);
		XMFLOAT4 specular(math::RandomFloat(0, 0.3f), math::RandomFloat(0, 0.3f), math::RandomFloat(0, 0.3f), 32.0f);

		ILightNode* light = smgr->addPointLight(i, nullptr, false, pos, 20.0f);
		light->setSpecular(specular);
		light->setDiffuse(diffuse);
		light->setAttenuation(1.0f, 0.05f, 0);
	}

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

	return smgr;
}

int main()
{

	IDevice* device = createDevice(EDT_DIRECT3D11, 800, 600);
	IVideoDriver* driver = device->getVideoDriver();
	IResourceGroupManager* resourceGroupManager = driver->getResourceGroupManager();
	resourceGroupManager->init("Resources.cfg");

	IInputDriver* input = device->getInputDriver();

	ISceneManager* smgr = setupScene(device);
	driver->setDeferredShading(true);
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

