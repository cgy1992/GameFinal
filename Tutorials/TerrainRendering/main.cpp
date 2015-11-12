#include <GameFinal.h>
#pragma comment(lib, "GameFinal.lib")

using namespace gf;

ISceneManager* setupScene(IDevice* device) {

	math::SAxisAlignedBox aabb;
	aabb.Center = XMFLOAT3(0, 0.0f, 0);
	aabb.Extents = XMFLOAT3(3000.0f, 1000.0f, 3000.0f);

	// create scene manager
	ISceneManager* smgr = device->createSceneManager(aabb);

	// get mesh manager
	IMeshManager* meshManager = IMeshManager::getInstance();

	// set sky dome
	smgr->setSkyDome("sky.dds");

	// add directional light
	ILightNode* light = smgr->addDirectionalLight(1, nullptr, XMFLOAT3(3.0f, -2.0f, 1.5f));
	light->setSpecular(XMFLOAT4(1.0f, 1.0f, 1.0f, 32.0f));
	light->setDiffuse(XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f));
	light->enableShadow(true);

	// create terrain mesh
	ITerrainMesh* terrainMesh = meshManager->createTerrainMesh("terrain", "terrain-heightmap.raw",
		3.0f, 0.2f, false, true, 50.0f);

	ITerrainNode* terrainNode = smgr->addTerrainNode(terrainMesh, "gf/terrain_material");

	/*
	ITerrainMesh* terrainMesh = meshManager->createTerrainMesh("terrain", "terrain-heightmap.raw",
		3.0f, 0.3f, true, false, 50.0f);

	ITerrainNode* terrainNode = smgr->addTerrainNode(terrainMesh, "gf/tess_terrain_material");
	*/

	terrainNode->addGrassLand(5000 * 64, 10.0f, 10.0f, "gf/grassland_material");

	// create a fps camera node
	IFpsCameraNode* camera = smgr->addFpsCameraNode(1, nullptr, XMFLOAT3(0, 1.0f, -6.0f),
		XMFLOAT3(0, 1.0f, 0.0f), XMFLOAT3(0, 1.0f, 0));
	camera->setShadowRange(200.0f);
	camera->setStandHeight(5.0f);


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

		smgr->update(dt);
		driver->beginScene(true, true, color);
		smgr->drawAll();
		driver->endScene();
		
	}

	smgr->destroy();
	device->drop();

	return 0;
}
