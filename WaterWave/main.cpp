#include <Windows.h>
#include <GameFinal.h>

#pragma comment(lib, "GameFinal.lib")
#pragma comment(lib, "winmm.lib")

using namespace gf;

const u32 SCREEN_WIDTH = 1024;
const u32 SCREEN_HEIGHT = 768;
const f32 CAMERA_MOVE_UNIT = 50.0f;
const f32 CAMERA_ROTATE_UNIT = 1.0f;

void updateCamera(ICameraNode* camera, f32 delta);

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

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT2 Tex;
};

ISimpleMesh* createWaterMesh(IMeshManager* meshManager, const std::string& name, 
	float width, float depth, int iVertexCol, int iVertexRow);

int main()
{
	SDeviceContextSettings settings;
	settings.MultiSamplingCount = 1;
	settings.MultiSamplingQuality = 0;
	IDevice* device = createDevice(EDT_DIRECT3D11, SCREEN_WIDTH, SCREEN_HEIGHT, EWS_NONE, true, settings);

	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->getSceneManager();
	IMeshManager* meshManager = driver->getMeshManager();
	IPipelineManager* pipelineManager = driver->getPipelineManager();

	IResourceGroupManager* resourceGroupManager = driver->getResourceGroupManager();
	resourceGroupManager->init("Resources.cfg");
	resourceGroupManager->loadResourceGroup("General");

	ISimpleMesh* mesh = createWaterMesh(meshManager, "waterwave", 1000.0f, 1000.0f, 30, 30);
	IMeshNode* meshNode = smgr->addMeshNode(mesh, nullptr, nullptr);
	meshNode->setMaterialName("test/wave_material");

	IPipeline* pipeline = pipelineManager->get("test/wave_pipeline");

	ILightNode* light = smgr->addLightNode(1);
	light->setType(ELT_POINT);
	light->setAmbient(XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f));
	light->setPosition(10.0f, 50.0f, -10.0f);
	light->setSpecular(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	light->setDiffuse(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	light->setAttenuation(1.0f, 0.0f, 0.0f);
	light->setRange(1000.0f);

	// gTangentWorldMatrix
	XMVECTOR normal = XMVectorSet(0, 1.0f, 0, 0);
	XMVECTOR tangent = XMVectorSet(1.0f, 0, 0, 0);
	XMVECTOR B = XMVector3Cross(normal, tangent);

	XMMATRIX TBN = XMMATRIX(tangent, B, normal, XMVectorSet(0, 0, 0, 1.0f));
	pipeline->setMatrix("gTangentWorldMatrix", TBN);

	ICameraNode* camera = smgr->addFpsCameraNode(1, nullptr, XMFLOAT3(0, 40.0f, -4.0f), XMFLOAT3(0, 40.0f, 0.0f));
	camera->setFarZ(10000.0f);
	char caption[200];

	ITimer* timer = device->getTimer();
	timer->reset();

	static float t1 = 0.0f;
	static float t2 = 0.0f;
	
	E_FILL_MODE fillMode = E_FILL_SOLID;

	SCompositorCreateParam param;
	param.Bloom.BlurPassCount = 3;
	param.Bloom.BrightnessThreshold = 0.85f;
	param.Bloom.BlurTexelDistance = 1.0f;
	param.Bloom.BlurTextureWidth = 400;
	param.Bloom.BlurTextureHeight = 300;
	
	ICompositor* bloom = smgr->createCompositor(ECT_BLOOM, param);
	smgr->addCompositor(bloom);

	while (device->run())
	{
		const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		driver->beginScene(true, true, clearColor);

		/*
		if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		{
			if (fillMode == E_FILL_SOLID)
				fillMode = E_FILL_WIREFRAME;
			else
				fillMode = E_FILL_SOLID;

			//pipeline->getRenderState()->set(ERS_FILL_MODE, fillMode);
			//pipeline->getRenderState()->confirm();
		}
		*/

		float dt = timer->tick() * 0.001f;
		t1 += dt * 0.032f;
		t2 += dt * 0.02f;
		if (t1 > 1.0f)
			t1 -= 1.0f;
		if (t2 > 1.0f)
			t2 -= 1.0f;

		XMMATRIX texTransform1 = XMMatrixTranslation(t1, 0, 0);
		XMMATRIX texTransform2 = XMMatrixTranslation(0, t2, 0);

		pipeline->setMatrix("gTexTransform1", texTransform1);
		pipeline->setMatrix("gTexTransform2", texTransform2);

		updateCamera(camera, dt);

		smgr->update(dt);
		smgr->drawAll();

		driver->endScene();

		sprintf(caption, "FPS:%f", getFps(dt));
		device->setWindowCaption(caption);
	}

	device->drop();

	return 0;
}

ISimpleMesh* createWaterMesh(IMeshManager* meshManager, const std::string& name,
	float width, float depth, int iVertexCol, int iVertexRow)
{
	float fWidthInterval = width / (iVertexCol - 1);
	float fDepthInterval = depth / (iVertexRow - 1);

	int iVertexNum = iVertexRow * iVertexCol;
	int iIndiceNum = (iVertexCol - 1) * (iVertexNum - 1) * 4;

	Vertex* vertices = new Vertex[iVertexNum];
	WORD* indices = new WORD[iIndiceNum];

	float fStartZ = depth * 0.5f;
	float fStartX = -width * 0.5f;

	for (int i = 0; i < iVertexRow; i++)
	{
		float z = fStartZ - i * fDepthInterval;
		for (int j = 0; j < iVertexCol; j++)
		{
			float x = fStartX + j * fWidthInterval;
			Vertex& v = vertices[i * iVertexCol + j];
			v.Pos = XMFLOAT3(x, 0.0f, z);
			v.Tex = XMFLOAT2((float)j / (iVertexCol - 1), (float)i / (iVertexRow - 1));
		}
	}

	int index = 0;
	for (int i = 0; i < iVertexRow - 1; i++)
	{
		for (int j = 0; j < iVertexCol - 1; j++)
		{
			indices[index++] = i * iVertexCol + j;
			indices[index++] = i * iVertexCol + j + 1;
			indices[index++] = (i + 1) * iVertexCol + j;
			indices[index++] = (i + 1) * iVertexCol + j + 1;
		}
	}

	ISimpleMesh* mesh = meshManager->createSimpleMesh(name, vertices, indices, iVertexNum, sizeof(Vertex), iIndiceNum, math::SAxisAlignedBox(), false);

	delete vertices;
	delete indices;

	return mesh;
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

