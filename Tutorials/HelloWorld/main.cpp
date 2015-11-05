#include <GameFinal.h>

#pragma comment(lib, "GameFinal.lib")
#pragma comment(lib, "winmm.lib")

using namespace gf;


int main()
{
	IDevice* device = createDevice(EDT_DIRECT3D11, 800, 600);
	IVideoDriver* driver = device->getVideoDriver();
	
	ISceneManager* smgr = device->createSceneManager();
	
	IResourceGroupManager* resourceGroupManager = driver->getResourceGroupManager();
	resourceGroupManager->init("Resources.cfg");

	// get mesh manager.
	IMeshManager* meshManager = driver->getMeshManager();

	// set up ground.
	// create ground's material
	SMaterial groundMaterial;
	groundMaterial.setAttribute("ambient", XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
	groundMaterial.setAttribute("diffuse", XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
	groundMaterial.setAttribute("specular", XMFLOAT4(0.05f, 0.05f, 0.05f, 32.0f));
	groundMaterial.setAttribute("emissive", XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	groundMaterial.setPipeline("gf/geometry_dirlight");

	ISimpleMesh* groundMesh = meshManager->createPlaneMesh("ground", 100.0f, 100.0f, 10, 10);
	IMeshNode* groundNode = smgr->addMeshNode(groundMesh, &groundMaterial, nullptr, true);

	// create sphere's material
	SMaterial sphereMaterial;
	sphereMaterial.setAttribute("ambient", XMFLOAT4(0.7f, 0.5f, 0.1f, 1.0f));
	sphereMaterial.setAttribute("diffuse", XMFLOAT4(1.0f, 0.9f, 0.17f, 1.0f));
	sphereMaterial.setAttribute("specular", XMFLOAT4(0.5f, 0.5f, 0.5f, 32.0f));
	sphereMaterial.setAttribute("emissive", XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	sphereMaterial.setPipeline("gf/geometry_dirlight");

	ISimpleMesh* sphereMesh = meshManager->createSphereMesh("sphere", 1.0f, 100, 100);
	IMeshNode* sphereNode = smgr->addMeshNode(sphereMesh, &sphereMaterial, nullptr, true, XMFLOAT3(0, 1.0f, 0));

	// add directional light
	ILightNode* light = smgr->addDirectionalLight(1, nullptr, XMFLOAT3(1.0f, -1.0f, 1.0f));
	light->setSpecular(XMFLOAT4(1.0f, 1.0, 1.0f, 32.0f));
	light->setDiffuse(XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));

	//ICameraNode* camera = smgr->addFpsCameraNode(1, nullptr, XMFLOAT3(0, 1.0f, -6.0f), XMFLOAT3(0, 1.0f, 0.0f), XMFLOAT3(0, 1.0f, 0), true);
	ICameraNode* camera = smgr->addCameraNode(1, nullptr, XMFLOAT3(0, 1.0f, -6.0f), XMFLOAT3(0, 1.0f, 0.0f), XMFLOAT3(0, 1.0f, 0));

	smgr->setAmbient(XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));

	ITimer* timer = device->getTimer();
	timer->reset();

	const f32 color[] = { 0, 0, 0, 1.0f };
	while (device->run()) {
		
		float dt = timer->tick();
		smgr->update(dt);
		driver->beginScene(true, true, color);
		smgr->drawAll();
		driver->endScene();
	}

	smgr->destroy();
	device->drop();

	return 0;

}

