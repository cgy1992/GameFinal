Buffer<float> gInputA;
Buffer<float> gInputB;
RWBuffer<float> gOutput;

[numthreads(32, 1, 1)]
void cs_main(int3 dispatchThreadID : SV_DispatchThreadID)
{
// Sum the xyth texels and store the result in the xyth texel of
// gOutput.
	gOutput[dispatchThreadID.x] = gInputA[dispatchThreadID.x] + gInputB[dispatchThreadID.x];
}