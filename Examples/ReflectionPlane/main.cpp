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
	SDeviceContextSettings settings;
	settings.MultiSamplingCount = 1;
	settings.MultiSamplingQuality = 0;

	IDevice* device = createDevice(EDT_DIRECT3D11, 1024, 768, EWS_NONE, true, settings);
	IVideoDriver* driver = device->getVideoDriver();
	IResourceGroupManager* resourceGroupManager = driver->getResourceGroupManager();
	resourceGroupManager->init("Resources.cfg");

	IInputDriver* input = device->getInputDriver();

	ISceneManager* smgr = setupScene(device);

	smgr->setSkyDome("Snow.dds");


	IReflectionPlane* plane = smgr->addReflectionPlane(1,
		XMFLOAT4(0, 1.0f, 0, 0), 100, 100);

	IMaterial* groundMtrl = IMaterialManager::getInstance()->get("ground_material");

	ITimer* timer = device->getTimer();
	timer->reset();

	const f32 color[] = { 0, 0.2f, 0.4f, 1.0f };
	while (device->run()) {
		float dt = timer->tick();
		if (input->keyDown(GVK_ESCAPE))
			break;

		smgr->update(dt);
		driver->beginScene(true, true, color);
		
		groundMtrl->setTexture(2, NULL);

		smgr->drawReflectionMaps();

		//groundMtrl->setTexture(1, ITextureManager::getInstance()->get("floor.dds"));
		groundMtrl->setTexture(2, plane->getReflectionMap());
		std::string varname = "gReflectionMatrix";
		XMFLOAT4X4 M = plane->getViewProjMatrix();
		groundMtrl->getPipeline(0)->setMatrix(varname, M, false);

		smgr->drawAll();
		driver->endScene();
	}

	smgr->destroy();
	device->drop();

	return 0;
}

