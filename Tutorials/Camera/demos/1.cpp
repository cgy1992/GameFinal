#include <GameFinal.h>

#pragma comment(lib, "GameFinal.lib")
#pragma comment(lib, "winmm.lib")

using namespace gf;

ICameraNode* camera = nullptr;

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

	// add directional light
	ILightNode* light = smgr->addDirectionalLight(1, nullptr, XMFLOAT3(5.0f, -5.0f, 2.0f));
	light->setSpecular(XMFLOAT4(1.0f, 1.0, 1.0f, 32.0f));
	light->setDiffuse(XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));

	// create a camera node
	camera = smgr->addCameraNode(1, nullptr, XMFLOAT3(0, 1.0f, -6.0f), XMFLOAT3(0, 1.0f, 0.0f), XMFLOAT3(0, 1.0f, 0));
	camera->setFovAngle(3.1415f / 3);  // set field-of-view to PI/3 (60 degrees) 
	camera->setNearZ(1.0f);  // set the distance to the near clipping plane
	camera->setFarZ(300.0f); // set the distance to the far clipping plane

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

	ITimer* timer = device->getTimer();
	timer->reset();

	const f32 color[] = { 0, 0, 0, 1.0f };
	while (device->run()) {
		// if we press ESC, then exit the game loop.
		if (input->keyDown(GVK_ESCAPE))
			break;

		float dt = timer->tick();
		
		if (input->isPressed(GVK_UP))
			camera->walk(2.0f * dt); // move forward
		if (input->isPressed(GVK_DOWN))
			camera->walk(-2.0f * dt); // move backward
		if (input->isPressed(GVK_LEFT))
			camera->strafe(-1.0f * dt); // move left
		if (input->isPressed(GVK_RIGHT))
			camera->strafe(1.0f * dt); // move right
		
		smgr->update(dt);
		driver->beginScene(true, true, color);
		smgr->drawAll();
		driver->endScene();
	}

	smgr->destroy();
	device->drop();

	return 0;
}

