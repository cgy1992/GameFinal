# Compute Shader #

GPGPU is a new trend in software development, especially those systems that has high parallelism but requires high performance. GameFinal provides a series of convenient interfaces for developers to use the DirectX11's compute shader. This tutorial will show you how to use compute shader to muiltiply two matrices.

Firstly, we create a HLSL file, "matmul.hlsl", and define a compute shader in it:

	Texture2D<float> gInputA;
	Texture2D<float> gInputB;
	RWTexture2D<float> gOutput;
	
	#define MAX_DIMENSIION 512
	#define BLOCK_SIZE 8
	#define BLOCK_NUM (MAX_DIMENSIION/BLOCK_SIZE)
	
	groupshared float cacheInputA[MAX_DIMENSIION][BLOCK_SIZE];
	groupshared float cacheInputB[BLOCK_SIZE][MAX_DIMENSIION];
	
	[numthreads(BLOCK_SIZE, BLOCK_SIZE, 1)]
	void cs_main(int3 dispatchThreadID : SV_DispatchThreadID,
		int3  groundThreadID : SV_GroupThreadID)
	{
		// fill row cache
		for(int i = 0; i < BLOCK_NUM; i++)
		{
			cacheInputA[groundThreadID.x + i * BLOCK_SIZE][groundThreadID.y] =
				gInputA[int2(groundThreadID.x + i * BLOCK_SIZE, dispatchThreadID.y)];
		}
	
		// fill col cache
		for(int i = 0; i < BLOCK_NUM; i++)
		{
			cacheInputB[groundThreadID.x][groundThreadID.y + i * BLOCK_SIZE] = 
				gInputB[int2(dispatchThreadID.x, groundThreadID.y + i * BLOCK_SIZE)];
		}
	
		GroupMemoryBarrierWithGroupSync();
	
		float sum = 0;
		for(int k = 0; k < MAX_DIMENSIION; k++)
		{
			sum += cacheInputA[k][groundThreadID.y] * cacheInputB[groundThreadID.x][k];
		}
	
		gOutput[dispatchThreadID.xy] = sum;
	}

The 3-dimension vector after keyword `numthreads` specifies the size of each block. Here we set (8, 8, 1), which means 8\*8\*1 threads in each block. We also use groupshared memory to improve performance. The threads within the same block could visit the groupshared memory. The two input matrices are stored in two textures `gInputA` and `gInputB`. The output of this compute shader will be written into `gOutput`.

In order to load this compute shader in C++ code, we need to call `IShaderManager::load` method:

	IShaderManager* shaderMgr = driver->getShaderManager();
	IShader* shader = shaderMgr->load(EST_COMPUTE_SHADER, "matmul.hlsl", "cs_main");

The first parameter specifies what's kind of shader would be loaded. Here `EST_COMPUTE_SHADER` means it's a compute shader. The second and the third parameters receive the name of the shader file and the shader's entry point.

Then let's initialize two input matrices:

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

We create 4 vector with the same size. The first two, A and B, will store the initial values in the two input matrices. The C vector receives the results of multiplication calculated by the compute shader. The D vector saves the results calculated by CPU. We can check the correctness by comparing C and D.

The code that calculates the values in D is quite simple:

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

Before calling the compute shader, we need to put the values A and B into two textures; so we create the following four textures:

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

Texture `inputTexture1` and `inputTexture2` are the input textures, which correpond the `gInputA` and `gInputB` in the shader respectively. Notice that we pass the address of the first element in A and B as a parameter of `createTexture2D` method, which means the textures will be initialized with the values in A and B respectively. The `outputTexture` texture is used to store the output of the compute shader. Another texture, `copyTexture` is used to copy the values from `outputTexture` to main memory. Because our program cannot read values from a texture in GPU. Then we call `setTexture` on shader to associate these textures with the variables in shader.

	u32 blockNum = dimension / 8;
	driver->runComputeShader(shader, blockNum, blockNum, 1);

Then we call `IVideoDriver::runComputeShader` method to actually run a compute shader. It has four parameters. The first gives the shader's interface. The other three parameters specify how many blocks we are going to dispatch in x, y, z directons respectively.

After that, we start to copy the results from GPU to main memory:

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

We are calling the `copyDataToAnotherTexture` method on `outputTexture` to copy all the data from `outputTexture` to `copyTexture`. Then we lock the `copyTexture` and begin to copy each row in this texture into the C vector. When finishing copy, we call the `unlock` method.

	for (u32 i = 0; i < sq_dimension; i++)
	{
		assert(math::FloatEqual(C[i], D[i]));
	}

Finally, we can check the correctness by comparing each element in C and D. Be aware that we use `math::FloatEqual` here instead of `==`. Because this method could tolerate a liitle error.

Run the program, from the information printed in the console, we can clearly see the computing by GPU is much faster than CPU: 

![](https://raw.githubusercontent.com/woyaofacai/GameFinal/master/Tutorials/img/11-01.png)






