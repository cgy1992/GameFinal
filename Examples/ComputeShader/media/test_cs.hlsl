//Texture2D gInputA;
Texture2D gInputA;
Texture2D gInputB;
RWTexture2D<float4> gOutput;

[numthreads(16, 16, 1)]
void cs_main(int3 dispatchThreadID : SV_DispatchThreadID)
{
// Sum the xyth texels and store the result in the xyth texel of
// gOutput.
	gOutput[dispatchThreadID.xy] = gInputA[dispatchThreadID.xy] * 0.5f + gInputB[dispatchThreadID.xy] * 0.5f;
}