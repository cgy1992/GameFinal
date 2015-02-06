Texture2D<float> gInputA;
Texture2D<float> gInputB;
RWTexture2D<float> gOutput;

[numthreads(32, 32, 1)]
void cs_main(int3 dispatchThreadID : SV_DispatchThreadID)
{
	gOutput[dispatchThreadID.xy] = gInputA[dispatchThreadID.xy] + gInputB[dispatchThreadID.xy];
}

#define MAX_DIMENSIION 512
#define BLOCK_SIZE 8
#define BLOCK_NUM (MAX_DIMENSIION/BLOCK_SIZE)

groupshared float cacheInputA[MAX_DIMENSIION][BLOCK_SIZE];
groupshared float cacheInputB[BLOCK_SIZE][MAX_DIMENSIION];

[numthreads(BLOCK_SIZE, BLOCK_SIZE, 1)]
void matmul_cs_main(int3 dispatchThreadID : SV_DispatchThreadID,
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
