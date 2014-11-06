cbuffer cbPerObject
{
	float4x4 gWorld;
};

cbuffer cbPerFrame
{
	float4x4 gViewProj;
};

struct VertexIn
{
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
};

VertexOut vs_main(VertexIn vin)
{
	VertexOut vout;
	float4 PosW = mul(float4(vin.PosL, 1.0f), gWorld);
	vout.PosH = mul(PosW, gViewProj);
	return vout;
}

float4 ps_main(VertexOut pin) : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}

