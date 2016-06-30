#include <GameFinal.h>

#pragma comment(lib, "GameFinal.lib")
#pragma comment(lib, "winmm.lib")

using namespace gf;

ISceneManager* setupScene(IApplication* device) {

	// create scene manager
	ISceneManager* smgr = device->createSceneManager();

	// get mesh manager
	IMeshManager* meshManager = IMeshManager::getInstance();



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
	light->setSpecular(XMFLOAT4(1.0f, 1.0, 1.0f, 32.0f));
	light->setDiffuse(XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));
	light->setAttenuation(1.0f, 0.05f, 0);
	light->enableShadow(true);

	sphereNode->addShadow(1);
	boxNode->addShadow(1);
	teapot->addShadow(1);

	// create a fps camera node
	IFpsCameraNode* camera = smgr->addFpsCameraNode(1, nullptr, XMFLOAT3(0, 1.0f, -6.0f),
		XMFLOAT3(0, 1.0f, 0.0f), XMFLOAT3(0, 1.0f, 0));
	//	camera->setStandHeight(5.0f);

	//ICameraNode* camera = smgr->addCameraNode(1, nullptr, XMFLOAT3(0, 1.0f, -6.0f),
	//	XMFLOAT3(0, 1.0f, 0.0f), XMFLOAT3(0, 1.0f, 0));
	camera->setNearZ(1.0f);
	camera->setFarZ(1000.0f);

	// set ambient in the environment.
	smgr->setAmbient(XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));

	return smgr;
}

int main()
{
	SAppSettings settings;
	//settings.Window.Style = EWS_FULLSCREEN | EWS_FULLRESOLUTION;
	settings.Window.Width = 1024;
	settings.Window.Height = 768;

	IApplication* device = gf::createApp(settings);
	IVideoDriver* driver = device->getVideoDriver();
	IResourceGroupManager* resourceGroupManager = driver->getResourceGroupManager();
	resourceGroupManager->init("Resources.cfg");

	IInputDriver* input = device->getInputDriver();

	ISceneManager* smgr = setupScene(device);

	smgr->setSkyDome("Snow.dds");

	IMeshManager* meshManager = IMeshManager::getInstance();

	// set up ground.
	ISimpleMesh* groundMesh = meshManager->createPlaneMesh("ground", 100.0f, 100.0f, 10, 10, 50, 50);

	IMeshNode* groundNode1 = smgr->addMeshNode(groundMesh, "ground_material", nullptr, true,
		XMFLOAT3(0, 0, 0));

	IMeshNode* groundNode2 = smgr->addMeshNode(groundMesh, "ground_material2", nullptr, true,
		XMFLOAT3(0, 0, 10.0f), XMFLOAT3(-XM_PI / 2, 0, 0));

	IReflectionPlane* plane1 = smgr->addReflectionPlane(1,
		100, 100);

	IReflectionPlane* plane2 = smgr->addReflectionPlane(2,
		100, 100, XMFLOAT3(0, 0, 10.0f), XMFLOAT3(0, 0, -1.0f));

	groundNode1->setReflectionPlane(plane1);
	groundNode2->setReflectionPlane(plane2);

	IMaterial* groundMtrl1 = IMaterialManager::getInstance()->get("ground_material");
	IMaterial* groundMtrl2 = IMaterialManager::getInstance()->get("ground_material2");

	device->setUpdateCallback([input, smgr, driver](float dt)->bool{
		const f32 color[] = { 1.0, 0.0f, 0.0f, 1.0f };
		if (input->keyDown(GVK_ESCAPE))
			return false;

		smgr->update(dt);
		driver->beginScene(true, true, color);

		smgr->drawReflectionMaps();

		smgr->drawAll();
		driver->endScene();

		return true;
	});

	device->run();

	smgr->destroy();
	device->drop();

	return 0;
}

