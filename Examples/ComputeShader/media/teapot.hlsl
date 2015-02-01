#include "../../../Media/built-in-resources/GameFinal.hlsl"

struct VertexIn
{
	float3 PosL		: POSITION;
	float3 Normal	: NORMAL;
	float2 Tex		: TEXCOORD;
};

struct VertexOut
{
	float4 PosH		: SV_POSITION;
	float3 PosW		: POSITION;
	float3 Normal	: NORMAL;
#ifdef SHADOW_ON
	float4 ShadowPosH : TEXCOORD1;
#endif
};

VertexOut vs_main(VertexIn vin)
{
	VertexOut vout;
	float4 PosW = mul(float4(vin.PosL, 1.0f), GF_WORLD);
	vout.PosH = mul(PosW, GF_VIEW_PROJ);
	vout.PosW = PosW.xyz;
	vout.Normal = mul(vin.Normal, (float3x3)GF_WORLD);

#ifdef SHADOW_ON
	vout.ShadowPosH = CalcShadowPosH(vout.PosW, GF_SHADOW_MAP_TRANSFORM_1);
#endif
	return vout;
}
