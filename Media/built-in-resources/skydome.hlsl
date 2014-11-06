#include "GameFinal.hlsl"

cbuffer cbPerFrame
{
	float4x4 gWorldViewProj;
};

TextureCube gCubeTexture;
SamplerState gCubeTextureState;

struct VertexIn
{
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
	float3 TangentL : TANGENT;
	float2 Tex : TEXCOORD;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float3 PosL : POSITION;
};

VertexOut vs_main(VertexIn vin)
{
	VertexOut vout;
	vout.PosL = vin.PosL;
	vout.PosH = mul(float4(vin.PosL, 1.0f), GF_WVP).xyww;

	return vout;
}


float4 ps_main(VertexOut pin) : SV_TARGET
{
	return gCubeTexture.Sample(gCubeTextureState, pin.PosL);
}
