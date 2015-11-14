#include<GameFinal.h>
#include <assert.h>

#pragma comment(lib, "GameFinal.lib")
using namespace gf;

int main()
{
	IDevice* device = gf::createDevice(EDT_DIRECT3D11, 800, 600);
	IVideoDriver* driver = device->getVideoDriver();
	ISceneManager* smgr = device->createSceneManager();

	IResourceGroupManager* resourceGroupManager = driver->getResourceGroupManager();
	resourceGroupManager->init("Resources.cfg");
	
	ITimer* timer = device->getTimer();
	timer->reset();

	ITextureManager* textureManager = ITextureManager::getInstance();
	IShaderManager* shaderMgr = driver->getShaderManager();
	IShader* shader = shaderMgr->load(EST_COMPUTE_SHADER, "matmul.hlsl", "cs_main");

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

	f32 start_time, end_time;

	start_time = timer->getTime();
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
	end_time = timer->getTime();
	printf("The computation time by CPU: %fs\n", end_time - start_time);


	start_time = timer->getTime();
	ITexture* inputTexture1 = textureManager->createTexture2D("input1", dimension, dimension,
		ETBT_SHADER_RESOURCE, &A[0], 1, EGF_R32_FLOAT, 0);

	ITexture* inputTexture2 = textureManager->createTexture2D("input2", dimension, dimension,
		ETBT_SHADER_RESOURCE, &B[0], 1, EGF_R32_FLOAT, 0);

	ITexture* outputTexture = textureManager->createTexture2D("output", dimension, dimension,
		ETBT_UNORDERED_ACCESS, nullptr, 1, EGF_R32_FLOAT, 0);

	ITexture* copyTexture = textureManager->createTexture2D("copy", dimension, dimension,
		ETBT_CPU_ACCESS_READ, nullptr, 1, EGF_R32_FLOAT, 0);

	shader->setTexture("gInputA", inputTexture1);
	shader->setTexture("gInputB", inputTexture2);
	shader->setTexture("gOutput", outputTexture);

	u32 blockNum = dimension / 8;
	if (blockNum * 8 != dimension)
		blockNum++;

	driver->runComputeShader(shader, blockNum, blockNum, 1);

	//driver->resetRWTextures();
	//driver->resetTextures();

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
	end_time = timer->getTime();
	printf("The computation time by GPU: %fs\n", end_time - start_time);

	for (u32 i = 0; i < sq_dimension; i++)
	{
		assert(math::FloatEqual(C[i], D[i]));
	}

	// destory textures.
	if (!textureManager->destroy(inputTexture1))
		printf("Destory texture failed!");

	if (!textureManager->destroy(inputTexture2))
		printf("Destory texture failed!");

	if (!textureManager->destroy(outputTexture))
		printf("Destory texture failed!");

	if (!textureManager->destroy(copyTexture))
		printf("Destory texture failed!");

	//sphereMaterial.drop();
	smgr->destroy();
	device->drop();

	system("pause");

	return 0;
}