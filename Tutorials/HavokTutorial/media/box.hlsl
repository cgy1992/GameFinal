#include "../../../Media/built-in-resources/GameFinal.hlsl"

cbuffer cbPerFrame
{
	SDirectionalLight gLight;
	SPointLight gPointLight;
};

struct VertexIn
{
	float3 PosL		: POSITION;
	float3 Normal	: NORMAL;
	float3 Tangent	: TANGENT;
	float2 Tex		: TEXCOORD;

#ifdef INSTANCES_ON
	GF_DECLARE_INTANCES_VERTEX
	float4 Color    : COLOR;
#endif
};

#ifdef INSTANCES_ON 
#define WORLD_TRANSROM (vin.World)
#else
#define WORLD_TRANSROM (GF_WORLD)
#endif

struct VertexOut
{
	float4 PosH		: SV_POSITION;
	float3 PosW		: POSITION;
	float3 Normal	: NORMAL;
#ifdef INSTANCES_ON
	float4 Color    : COLOR;
#endif
};

VertexOut vs_main(VertexIn vin)
{
	VertexOut vout;
	float4 PosW = mul(float4(vin.PosL, 1.0f), WORLD_TRANSROM);
	vout.Normal = mul(vin.Normal, (float3x3)WORLD_TRANSROM);
	vout.PosH = mul(PosW, GF_VIEW_PROJ);
	vout.PosW = PosW.xyz;
#ifdef INSTANCES_ON
	vout.Color = vin.Color;
#endif
	return vout;
}

void shadow_vs_main(VertexIn vin,
	out float4 PosH: SV_POSITION)
{
	float4 PosW = mul(float4(vin.PosL, 1.0f), WORLD_TRANSROM);
	PosH = mul(PosW, GF_VIEW_PROJ);
}

float4 ps_main(VertexOut pin) : SV_TARGET
{
	float3 normal = normalize(pin.Normal);
	float4 diffuse = gLight.Diffuse;
	float4 specular = gLight.Specular;

	PhoneShading(pin.PosW, -gLight.Direction.xyz, normal,
		diffuse, specular, gLight.Specular.w);

#ifdef INSTANCES_ON
	float4 color = pin.Color;
#else
	float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);
#endif

#ifdef SHADOW_ON
	float shadowFactor = CalcShadowFactor(1, 2.0f);
	
	//float shadowFactor = CalcPointLightShadowFactor(GF_PL_SHADOW_MAP_2, pin.PosW, gPointLight.Position);
	return GF_AMBIENT * GF_MTRL_AMBIENT * color + GF_MTRL_EMISSIVE + 
		diffuse * GF_MTRL_DIFFUSE * shadowFactor * color + 
		specular * GF_MTRL_SPECULAR * shadowFactor;
#else
	return GF_AMBIENT * GF_MTRL_AMBIENT * color + GF_MTRL_EMISSIVE + 
		diffuse * GF_MTRL_DIFFUSE * color + specular * GF_MTRL_SPECULAR;
#endif
}
