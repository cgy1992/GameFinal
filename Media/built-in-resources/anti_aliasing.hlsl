SamplerState gPointSamplerState;
SamplerState gLinearSamplerState;

Texture2D gPixelsBuffer;
Texture2D gDepthBuffer;
Texture2D gNormalBuffer;

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float2 Tex : TEXCOORD;
};

static float2 gBarrier = float2(0.8f, 0.5f);
static float2 gWeights = float2(0.5f, 0.5f);


// This algorithm comes from GPU Gem 2
float4 ps_main(VertexOut pin) : SV_TARGET
{
	static float2 offsets[8] = {
		float2(-1.0f, -1.0f), // left top
		float2(1.0f, 1.0f), // right bottom
		float2(1.0f, -1.0f), // right bottom
		float2(-1.0f, 1.0f), // left bottom
		float2(-1.0f, 0), // left
		float2(1.0f, 0), // right
		float2(0, -1.0f), // top
		float2(0, 1.0f) // bottom
	};

	float2 texes[8];
	[unroll]
	for(int i = 0; i < 8; i++)
	{
		texes[i] = pin.Tex + offsets[i] * GF_SCREEN_SIZE.zw;
	}

	float3 nc = gNormalBuffer.Sample(gPointSamplerState, pin.Tex).xyz * 2.0f - 1.0f;
	float4 nd;
	nd.x = dot(nc, gNormalBuffer.Sample(gPointSamplerState, texes[0]).xyz * 2.0f - 1.0f);
	nd.y = dot(nc, gNormalBuffer.Sample(gPointSamplerState, texes[1]).xyz * 2.0f - 1.0f);
	nd.z = dot(nc, gNormalBuffer.Sample(gPointSamplerState, texes[2]).xyz * 2.0f - 1.0f);
	nd.w = dot(nc, gNormalBuffer.Sample(gPointSamplerState, texes[3]).xyz * 2.0f - 1.0f);

	nd -= gBarrier.x;
	nd = step(0, nd);
	float ne = saturate(dot(nd, gWeights.x));

	float3 dc = ComputeViewPosFromDepthBuffer(gDepthBuffer, gPointSamplerState, pin.Tex);

	float4 dd;
	[unroll]
	for(int i = 0; i < 4; i++)
	{
		dd[i] = ComputeViewPosFromDepthBuffer(gDepthBuffer, gPointSamplerState, texes[2 * i]).z +
				ComputeViewPosFromDepthBuffer(gDepthBuffer, gPointSamplerState, texes[2 * i + 1]).z;
	}

	dd = abs(2 * dc.z - dd) - gBarrier.y;
	dd = step(dd, 0);

	float de = saturate(dot(dd, gWeights.y));
	float w = 1.0f - de * ne;

	float2 offset = pin.Tex * (1.0f - w);
	float4 s0 = gPixelsBuffer.Sample(gLinearSamplerState, offset + texes[0] * w);
	float4 s1 = gPixelsBuffer.Sample(gLinearSamplerState, offset + texes[1] * w);
	float4 s2 = gPixelsBuffer.Sample(gLinearSamplerState, offset + texes[2] * w);
	float4 s3 = gPixelsBuffer.Sample(gLinearSamplerState, offset + texes[3] * w);

	return (s0 + s1 + s2 + s3) / 4.0f;
}

