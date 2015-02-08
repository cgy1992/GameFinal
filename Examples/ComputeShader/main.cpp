#include<Windows.h>
#include<GameFinal.h>
#include <assert.h>

#pragma comment(lib, "GameFinal.lib")
#pragma comment(lib, "winmm.lib")

using namespace gf;

const u32 SCREEN_WIDTH = 1024;
const u32 SCREEN_HEIGHT = 768;
const f32 CAMERA_MOVE_UNIT = 10.0f;
const f32 CAMERA_ROTATE_UNIT = 1.0f;

void updateCamera(ICameraNode* camera, f32 delta);
void updateLightDirection(f32 delta, ILightNode* light);

void TestCase1();
void TestCase2();
void TestCase3();
void TestCase4();
void TestCase5();

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

	
	TestCase1(); // Blend two textures.
	TestCase2(); // Add two float arrays.

	TestCase3(); // Test two structure buffers.
	TestCase4(); // Add two matrices.
	TestCase5(); // matrix multiply.

	char caption[200];

	ITimer* timer = device->getTimer();
	timer->reset();

	const f32 color2[] = { 1.0f, 0.0f, 0.0f, 1.0f };

	while (device->run())
	{
		const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		driver->beginScene(true, true, clearColor);

		f32 dt = timer->tick();

		updateCamera(camera, dt);

		smgr->update(dt);

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

// Test case 1: Blending two textures
void TestCase1()
{
	ITextureManager* textureManager = ITextureManager::getInstance();
	IMaterial* groundMaterial = IMaterialManager::getInstance()->get("ground_material");
	IVideoDriver* driver = IVideoDriver::getInstance();
	IShaderManager* shaderMgr = driver->getShaderManager();
	IShader* shader = shaderMgr->load(EST_COMPUTE_SHADER, "test_cs.hlsl", "cs_main");

	// test case 1: blend two textures.
	ITexture* texture1 = textureManager->get("Fieldstone.dds", true);
	ITexture* texture2 = textureManager->get("wangyuanji.png", true);

	ITexture* outputTexture = textureManager->createTexture2D("outputTexture", 512, 512,
		ETBT_SHADER_RESOURCE | ETBT_UNORDERED_ACCESS, nullptr, 1, EGF_R8G8B8A8_UNORM);

	shader->setTexture("gInputA", texture1);
	shader->setTexture("gInputB", texture2);
	shader->setTexture("gOutput", outputTexture);
	driver->runComputeShader(shader, 512 / 16, 512 / 16, 1);
	driver->resetRWTextures();
	groundMaterial->setTexture(0, outputTexture);
}

// test case 2 : Add two float arrays.
void TestCase2()
{
	ITextureManager* textureManager = ITextureManager::getInstance();
	IVideoDriver* driver = IVideoDriver::getInstance();
	IShaderManager* shaderMgr = driver->getShaderManager();
	IShader* shader = shaderMgr->load(EST_COMPUTE_SHADER, "test_cs2.hlsl", "cs_main");
	
	const u32 elementNum = 512;
	std::vector<f32> A(elementNum);
	std::vector<f32> B(elementNum);
	std::vector<f32> C(elementNum);

	for (u32 i = 0; i < elementNum; i++)
	{
		A[i] = math::RandomFloat(0, 100.0f);
		B[i] = math::RandomFloat(0, 100.0f);
	}

	IBuffer* buffer1 = textureManager->createBuffer("buffer1", elementNum, ETBT_SHADER_RESOURCE, EGF_R32_FLOAT, 0, &A[0]);
	IBuffer* buffer2 = textureManager->createBuffer("buffer2", elementNum, ETBT_SHADER_RESOURCE, EGF_R32_FLOAT, 0, &B[0]);
	IBuffer* outputBuffer = textureManager->createBuffer("outputBuffer", elementNum, ETBT_UNORDERED_ACCESS, EGF_R32_FLOAT, 0);
	IBuffer* copyBuffer = textureManager->createBuffer("copy_buffer", elementNum, ETBT_CPU_ACCESS_READ, EGF_R32_FLOAT, 0);
	shader->setTexture("gInputA", buffer1);
	shader->setTexture("gInputB", buffer2);
	shader->setTexture("gOutput", outputBuffer);
	driver->runComputeShader(shader, elementNum / 32, 1, 1);
	driver->resetRWTextures();
	outputBuffer->copyDataToAnotherTexture(copyBuffer);

	STextureData outputData;
	copyBuffer->lock(ETLT_READ, &outputData);
	memcpy(&C[0], outputData.Data, elementNum * sizeof(f32));
	copyBuffer->unlock();

	for (u32 i = 0; i < elementNum; i++) {
		float r1 = A[i] + B[i];
		float r2 = C[i];
		std::cout << r1 << " " << r2 << std::endl;
	}
}

// Test case 3 : Structure Buffer Test
void TestCase3()
{
	ITextureManager* textureManager = ITextureManager::getInstance();
	IVideoDriver* driver = IVideoDriver::getInstance();
	IShaderManager* shaderMgr = driver->getShaderManager();
	IShader* shader = shaderMgr->load(EST_COMPUTE_SHADER, "test_cs3.hlsl", "cs_main");

	// test case 3: Add two structure buffers.
	struct SInputData {
		XMFLOAT2 v1; // this field is for adding
		XMFLOAT3 v2; // this field is for dot.
		f32		 _pad[3];
	};

	struct SOutputData {
		XMFLOAT2 v1;
		f32		 v2;
		f32		 _pad;
	};

	const u32 elementNum = 512;
	std::vector<SInputData> A(elementNum * 2);
	std::vector<SInputData> B(elementNum * 2);
	std::vector<SOutputData> C(elementNum);

	// init input buffer
	for (u32 i = 0; i < elementNum; i++)
	{
		A[i].v1 = XMFLOAT2(math::RandomFloat(0, 10.0f), math::RandomFloat(0, 10.0f));
		A[i].v2 = XMFLOAT3(math::RandomFloat(0, 10.0f), math::RandomFloat(0, 10.0f), math::RandomFloat(0, 10.0f));

		B[i].v1 = XMFLOAT2(math::RandomFloat(0, 10.0f), math::RandomFloat(0, 10.0f));
		B[i].v2 = XMFLOAT3(math::RandomFloat(0, 10.0f), math::RandomFloat(0, 10.0f), math::RandomFloat(0, 10.0f));	
	}

	IBuffer* inputBuffer1 = textureManager->createBuffer("testcase3_inputbuffer1", elementNum * 2, 
		ETBT_SHADER_RESOURCE | ETBT_CPU_ACCESS_WRITE, 
		EGF_UNKNOWN, sizeof(SInputData), NULL, EMU_DYNAMIC);
	IBuffer* inputBuffer2 = textureManager->createBuffer("testcase3_inputbuffer2", elementNum * 2, ETBT_SHADER_RESOURCE,
		EGF_UNKNOWN, sizeof(SInputData), &B[0]);

	IBuffer* outputBuffer = textureManager->createBuffer("testcase3_outputbuffer", elementNum,
		ETBT_UNORDERED_ACCESS, EGF_UNKNOWN, sizeof(SOutputData));

	// just for testing dynamic buffer.
	STextureData inputData;
	//inputBuffer1->setData(&A[0], elementNum);
	inputBuffer1->lock(ETLT_WRITE_DISCARD, &inputData, 0);
	memcpy(inputData.Data, &A[0], inputData.RowPitch);
	inputBuffer1->unlock();

	shader->setTexture("gInputA", inputBuffer1);
	shader->setTexture("gInputB", inputBuffer2);
	shader->setTexture("gOutput", outputBuffer);

	driver->runComputeShader(shader, elementNum / 32, 1, 1);
	driver->resetRWTextures();
	driver->resetTextures();

	IBuffer* copyBuffer = textureManager->createBuffer("testcase3_copybuffer", elementNum, ETBT_CPU_ACCESS_READ, EGF_UNKNOWN,
		sizeof(SOutputData));

	outputBuffer->copyDataToAnotherTexture(copyBuffer);
	STextureData outputData;
	copyBuffer->lock(ETLT_READ, &outputData);
	memcpy(&C[0], outputData.Data, sizeof(SOutputData)* elementNum);
	copyBuffer->unlock();

	// check the results
	for (int i = 0; i < elementNum; i++) {
		XMFLOAT2 add_result1 = XMFLOAT2(A[i].v1.x + B[i].v1.x, A[i].v1.y + B[i].v1.y);
		printf("AddResult:[%f,%f] = [%f,%f]\n", add_result1.x, add_result1.y, C[i].v1.x, C[i].v1.y);
		f32 dot_result = A[i].v2.x * B[i].v2.x + A[i].v2.y * B[i].v2.y + A[i].v2.z * B[i].v2.z;
		printf("DotResult:%f = %f\n", dot_result, C[i].v2);

		assert(math::FloatEqual(add_result1.x, C[i].v1.x));
		assert(math::FloatEqual(add_result1.y, C[i].v1.y));
		assert(math::FloatEqual(dot_result, C[i].v2));
	}

	// destory textures.
	if (!textureManager->destroy(inputBuffer1))
		printf("Destory texture failed!");

	if (!textureManager->destroy(inputBuffer2))
		printf("Destory texture failed!");

	if (!textureManager->destroy(outputBuffer))
		printf("Destory texture failed!");

	if (!textureManager->destroy(copyBuffer))
		printf("Destory texture failed!");
}

// Add two matrices
void TestCase4()
{
	ITextureManager* textureManager = ITextureManager::getInstance();
	IVideoDriver* driver = IVideoDriver::getInstance();
	IShaderManager* shaderMgr = driver->getShaderManager();
	IShader* shader = shaderMgr->load(EST_COMPUTE_SHADER, "test_cs4.hlsl", "cs_main");

	const u32 dimension = 500;
	const u32 sq_dimension = dimension * dimension;
	std::vector<f32> A(sq_dimension);
	std::vector<f32> B(sq_dimension);
	std::vector<f32> C(sq_dimension);

	// init data
	for (u32 i = 0; i < sq_dimension; i++)
	{
		A[i] = math::RandomFloat(0, 10.0f);
		B[i] = math::RandomFloat(0, 10.0f);
	}

	ITexture* inputTexture1 = textureManager->createTexture2D("testcase4_input1", dimension, dimension,
		ETBT_SHADER_RESOURCE, &A[0], 1, EGF_R32_FLOAT, 0);

	ITexture* inputTexture2 = textureManager->createTexture2D("testcase4_input2", dimension, dimension,
		ETBT_SHADER_RESOURCE, &B[0], 1, EGF_R32_FLOAT, 0);

	ITexture* outputTexture = textureManager->createTexture2D("testcase4_output", dimension, dimension,
		ETBT_UNORDERED_ACCESS, nullptr, 1, EGF_R32_FLOAT, 0);

	ITexture* copyTexture = textureManager->createTexture2D("testcase4_copy", dimension, dimension,
		ETBT_CPU_ACCESS_READ, nullptr, 1, EGF_R32_FLOAT, 0);
	
	shader->setTexture("gInputA", inputTexture1);
	shader->setTexture("gInputB", inputTexture2);
	shader->setTexture("gOutput", outputTexture);

	u32 blockNum = dimension / 32;
	if (blockNum * 32 != dimension)
		blockNum++;

	driver->runComputeShader(shader, blockNum, blockNum, 1);
	driver->resetRWTextures();
	driver->resetTextures();

	outputTexture->copyDataToAnotherTexture(copyTexture);
	STextureData outputData;
	copyTexture->lock(ETLT_READ, &outputData);
	u8* data = (u8*)outputData.Data;
	for (u32 i = 0; i < dimension; i++)
	{
		// copy each row.
		memcpy(&C[i * dimension], data + outputData.RowPitch * i, dimension * sizeof(f32));
	}

	//memcpy(&C[0], outputData.Data, dimension * outputData.RowPitch);
	copyTexture->unlock();

	for (u32 i = 0; i < 1000; i++)
	{
		assert(math::FloatEqual(A[i] + B[i], C[i]));
	}

	printf("Test Case 4: Passed.\n");

	// destory textures.
	if (!textureManager->destroy(inputTexture1))
		printf("Destory texture failed!");

	if (!textureManager->destroy(inputTexture2))
		printf("Destory texture failed!");

	if (!textureManager->destroy(outputTexture))
		printf("Destory texture failed!");

	if (!textureManager->destroy(copyTexture))
		printf("Destory texture failed!");
}

// matrix multiplication.
void TestCase5()
{
	ITextureManager* textureManager = ITextureManager::getInstance();
	IVideoDriver* driver = IVideoDriver::getInstance();
	IShaderManager* shaderMgr = driver->getShaderManager();
	IShader* shader = shaderMgr->load(EST_COMPUTE_SHADER, "test_cs4.hlsl", "matmul_cs_main");
	ITimer* timer = IDevice::getInstance()->getTimer();


	const u32 dimension = 512;
	const u32 sq_dimension = dimension * dimension;
	std::vector<f32> A(sq_dimension);
	std::vector<f32> B(sq_dimension);
	std::vector<f32> C(sq_dimension);
	std::vector<f32> D(sq_dimension);
		
	// init data
	for (u32 i = 0; i < sq_dimension; i++)
	{
		A[i] = math::RandomFloat(0, 10.0f);
		B[i] = math::RandomFloat(0, 10.0f);
	}

	// store the right answers to D
	for (u32 i = 0; i < dimension; i++)
	{
		for (u32 j = 0; j < dimension; j++)
		{
			f32 sum = 0;
			for (u32 k = 0; k < dimension; k++)
			{
				sum += A[i * dimension + k] * B[k * dimension + j];
			}
			D[i * dimension + j] = sum;
		}
	}

	ITexture* inputTexture1 = textureManager->createTexture2D("testcase5_input1", dimension, dimension,
		ETBT_SHADER_RESOURCE, &A[0], 1, EGF_R32_FLOAT, 0);

	ITexture* inputTexture2 = textureManager->createTexture2D("testcase5_input2", dimension, dimension,
		ETBT_SHADER_RESOURCE, &B[0], 1, EGF_R32_FLOAT, 0);

	ITexture* outputTexture = textureManager->createTexture2D("testcase5_output", dimension, dimension,
		ETBT_UNORDERED_ACCESS, nullptr, 1, EGF_R32_FLOAT, 0);

	ITexture* copyTexture = textureManager->createTexture2D("testcase5_copy", dimension, dimension,
		ETBT_CPU_ACCESS_READ, nullptr, 1, EGF_R32_FLOAT, 0);

	shader->setTexture("gInputA", inputTexture1);
	shader->setTexture("gInputB", inputTexture2);
	shader->setTexture("gOutput", outputTexture);

	u32 blockNum = dimension / 8;
	if (blockNum * 8 != dimension)
		blockNum++;

	f32 start_time = timer->getTime();

	driver->runComputeShader(shader, blockNum, blockNum, 1);

	f32 end_time = timer->getTime();

	driver->resetRWTextures();
	driver->resetTextures();

	outputTexture->copyDataToAnotherTexture(copyTexture);
	STextureData outputData;
	copyTexture->lock(ETLT_READ, &outputData);
	u8* data = (u8*)outputData.Data;
	for (u32 i = 0; i < dimension; i++)
	{
		// copy each row.
		memcpy(&C[i * dimension], data + outputData.RowPitch * i, dimension * sizeof(f32));
	}

	copyTexture->unlock();

	for (u32 i = 0; i < sq_dimension; i++)
	{
		//printf("%f=%f\n", C[i], D[i]);
		assert(math::FloatEqual(C[i], D[i]));
	}

	printf("Test Case 5: Passed. Time:%f(s)\n", end_time - start_time);

	// destory textures.
	if (!textureManager->destroy(inputTexture1))
		printf("Destory texture failed!");

	if (!textureManager->destroy(inputTexture2))
		printf("Destory texture failed!");

	if (!textureManager->destroy(outputTexture))
		printf("Destory texture failed!");

	if (!textureManager->destroy(copyTexture))
		printf("Destory texture failed!");
}
