struct SInputData
{
	float2 v1;
	float3 v2;
	float3 _pad;
};

struct SOutputData
{
	float2 v1;
	float  v2;
	float _pad;
};

StructuredBuffer<SInputData> gInputA;
StructuredBuffer<SInputData> gInputB;
RWStructuredBuffer<SOutputData> gOutput;

[numthreads(32, 1, 1)]
void cs_main(int3 dispatchThreadID : SV_DispatchThreadID,
	int3  groundThreadID : SV_GroupThreadID)
{
// Sum the xyth texels and store the result in the xyth texel of
// gOutput.
//	gOutput[dispatchThreadID.x] = gInputA[dispatchThreadID.x] + gInputB[dispatchThreadID.x];
	
	SOutputData sout = (SOutputData)0;

	sout.v1 = gInputA[dispatchThreadID.x].v1 + gInputB[dispatchThreadID.x].v1;
	sout.v2 = dot(gInputA[dispatchThreadID.x].v2, gInputB[dispatchThreadID.x].v2);

	gOutput[dispatchThreadID.x] = sout;

}