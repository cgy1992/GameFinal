#include "../../../Media/built-in-resources/GameFinal.hlsl"

cbuffer cbPerFrame
{
	SDirectionalLight gLight;
};

struct VertexIn
{
	float3 PosL		: POSITION;
	float3 Normal	: NORMAL;
	float3 Tangent	: TANGENT;
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

void shadow_vs_main(VertexIn vin,
	out float4 PosH: SV_POSITION)
{
	PosH = mul(float4(vin.PosL, 1.0f), GF_WVP);
}


float4 ps_main(VertexOut pin) : SV_TARGET
{
	float3 normal = normalize(pin.Normal);
	float4 diffuse = gLight.Diffuse;
	float4 specular = gLight.Specular;
	
	PhoneShading(pin.PosW, -gLight.Direction.xyz, normal,
		diffuse, specular, gLight.Specular.w);

#ifdef SHADOW_ON
	float shadowFactor = CalcShadowFactor(1,pin.ShadowPosH,5);
	return GF_AMBIENT * GF_MTRL_AMBIENT + GF_MTRL_EMISSIVE + 
		diffuse * GF_MTRL_DIFFUSE * shadowFactor + specular * GF_MTRL_SPECULAR * shadowFactor;
#else
	return GF_AMBIENT * GF_MTRL_AMBIENT + GF_MTRL_EMISSIVE + 
		diffuse * GF_MTRL_DIFFUSE + specular * GF_MTRL_SPECULAR;
#endif
}
