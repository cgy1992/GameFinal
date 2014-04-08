#include <Windows.h>
#include <GameFinal.h>

#pragma comment(lib, "GameFinal.lib")
#pragma comment(lib, "winmm.lib")

struct Vertex
{
	XMFLOAT3 pos;
	XMFLOAT4 color;
	float size;

	Vertex() {}
	Vertex(XMFLOAT3 pos, XMFLOAT4 color, float size)
		:pos(pos), color(color), size(size){}
};

int main()
{
	SDeviceContextSettings settings;
	settings.MultiSamplingCount = 4;
	settings.MultiSamplingQuality = 32;
	IDevice* device = createDevice(EDT_DIRECT3D11, 800, 600, EWS_NONE, true, settings);

	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->getSceneManager();
	IMeshManager* meshManager = driver->getMeshManager();

	IResourceGroupManager* resourceGroupManager = driver->getResourceGroupManager();
	resourceGroupManager->init("Resources.cfg");
	resourceGroupManager->loadResourceGroup("General");

	Vertex vertices[5];
	vertices[0] = Vertex(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), 0.5f);
	vertices[1] = Vertex(XMFLOAT3(-1.0f, 1.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f), 0.5f);
	vertices[2] = Vertex(XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), 0.5f);
	vertices[3] = Vertex(XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f), 0.5f);
	vertices[4] = Vertex(XMFLOAT3(1.0f, -1.0f, 0.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f), 0.5f);

	ISimpleMesh* mesh = meshManager->createSimpleMesh("pointlist", vertices, NULL, 5, sizeof(Vertex), 0, false);
	IMeshNode* meshNode = smgr->addMeshNode(mesh, nullptr, nullptr);
	meshNode->setMaterialName("test/gs_material");

	//ICameraNode* camera = smgr->addFpsCameraNode(1, nullptr, XMFLOAT3(0, 0.0f, -4.0f), XMFLOAT3(0, 0.0f, 0.0f));
	ICameraNode* camera = smgr->addCameraNode(1, nullptr);

	while (device->run())
	{
		const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		driver->beginScene(true, true, clearColor);

		smgr->drawAll();

		driver->endScene();

		//device->setWindowCaption(caption);
	}

	device->drop();

	return 0;
}