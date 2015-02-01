#include<Windows.h>
#include<GameFinal.h>

#pragma comment(lib, "GameFinal.lib")
#pragma comment(lib, "winmm.lib")

using namespace gf;

const u32 SCREEN_WIDTH = 1024;
const u32 SCREEN_HEIGHT = 768;
const f32 CAMERA_MOVE_UNIT = 10.0f;
const f32 CAMERA_ROTATE_UNIT = 1.0f;

void updateCamera(ICameraNode* camera, f32 delta);
void updateLightDirection(f32 delta, ILightNode* light);

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

int main()
{
	SDeviceContextSettings settings;
	settings.MultiSamplingCount = 4;
	settings.MultiSamplingQuality = 32;

	IDevice* device = gf::createDevice(EDT_DIRECT3D11, SCREEN_WIDTH, SCREEN_HEIGHT, EWS_NONE, true, settings);
	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->createSceneManager();
	IMeshManager* meshManager = driver->getMeshManager();
	IMaterialManager* materialManager = driver->getMaterialManager();
	ITextureManager* textureManager = driver->getTextureManager();
	IPipelineManager* pipelineManager = driver->getPipelineManager();

	IResourceGroupManager* resourceGroupManager = driver->getResourceGroupManager();
	resourceGroupManager->init("Resources.cfg");

	// create groun
	const f32 groundSize = 100.0f;
	ISimpleMesh* groundMesh = meshManager->createPlaneMesh("ground", groundSize, groundSize, 10, 10, 30.0f, 30.0f);
	IMaterial* groundMaterial = materialManager->get("ground_material");
	IMeshNode* groundNode = smgr->addMeshNode(groundMesh, groundMaterial, nullptr, true);
	
	ICameraNode* camera = smgr->addFpsCameraNode(1, nullptr, XMFLOAT3(0, 1.0f, -4.0f), XMFLOAT3(0, 1.0f, 0.0f), XMFLOAT3(0, 1.0f, 0), true);
	camera->setFarZ(3000.0f);
	
	smgr->setAmbient(XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));


	// test creating compute shader.
	IShaderManager* shaderMgr = driver->getShaderManager();
	IShader* shader1 = shaderMgr->load(EST_COMPUTE_SHADER, "test_cs.hlsl", "cs_main");
	IShader* shader2 = shaderMgr->load(EST_COMPUTE_SHADER, "test_cs2.hlsl", "cs_main");


	// test case 1
	ITexture* texture1 = textureManager->get("Fieldstone.dds", true);
	ITexture* texture2 = textureManager->get("wangyuanji.png", true);

	ITexture* outputTexture = textureManager->createTexture2D("outputTexture", 512, 512,
		ETBT_SHADER_RESOURCE | ETBT_UNORDERED_ACCESS, nullptr, 1, EGF_R8G8B8A8_UNORM);

	shader1->setTexture("gInputA", texture1);
	shader1->setTexture("gInputB", texture2);
	shader1->setTexture("gOutput", outputTexture);
	driver->runComputeShader(shader1, 512 / 16, 512 / 16, 1);
	driver->resetRWTextures();
	groundMaterial->setTexture(0, outputTexture);

	// test case 2
	const u32 elementNum = 100;
	std::vector<f32> A(elementNum);
	std::vector<f32> B(elementNum);

	for (u32 i = 0; i < elementNum; i++)
	{
		A[i] = math::RandomFloat(0, 100.0f);
		B[i] = math::RandomFloat(0, 100.0f);
	}

	IBuffer* buffer1 = textureManager->createBuffer("buffer1", elementNum, ETBT_SHADER_RESOURCE, EGF_R32_FLOAT, 0, &A[0]);
	IBuffer* buffer2 = textureManager->createBuffer("buffer2", elementNum, ETBT_SHADER_RESOURCE, EGF_R32_FLOAT, 0, &B[0]);
	IBuffer* outputBuffer = textureManager->createBuffer("outputBuffer", elementNum, ETBT_UNORDERED_ACCESS | ETBT_CPU_ACCESS_READ, EGF_R32_FLOAT, 0);

	

	char caption[200];

	ITimer* timer = device->getTimer();
	timer->reset();

	const f32 color2[] = { 1.0f, 0.0f, 0.0f, 1.0f };

	while (device->run())
	{
		const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		driver->beginScene(true, true, clearColor);

		u32 ms = timer->tick();
		float dt = ms * 0.001f;

		updateCamera(camera, dt);

		smgr->update(ms);

		smgr->drawAll();

		driver->endScene();

		sprintf_s(caption, "FPS:%f", getFps(dt));
		device->setWindowCaption(caption);
	}

	//sphereMaterial.drop();
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
void updateLightDirection(f32 delta, ILightNode* light)
{
	f32 fAngleSpeed = 0.3f;
	f32 lightDistance = -20;
	XMFLOAT4 dir = light->getDirection();
	XMVECTOR dir_vec = XMLoadFloat4(&dir);
	
	//XMVECTOR axis = XMVectorSet(1.5f, 10.0f, 3.0f, 0.0f);
	XMVECTOR axis = XMVectorSet(0, 1.0f, 0, 0);
	XMMATRIX rotMat = XMMatrixRotationAxis(axis, fAngleSpeed * delta);
	dir_vec = XMVector3TransformNormal(dir_vec, rotMat);
	XMStoreFloat4(&dir, dir_vec);

	light->setDirection(XMFLOAT3(dir.x, dir.y, dir.z));
	light->setPosition(XMFLOAT3(dir.x * lightDistance, dir.y * lightDistance, dir.z * lightDistance));

}
