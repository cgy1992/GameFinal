#include "../../../Media/built-in-resources/GameFinal.hlsl"

SamplerState gSampleState;

cbuffer cbPerFrame
{
	SDirectionalLight		 gLight;
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
	float3 Tangent  : TANGENT;
	float2 Tex		: TEXCOORD;
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
	vout.Tangent = mul(vin.Tangent, (float3x3)GF_WORLD);
	vout.Tex = vin.Tex;

#ifdef SHADOW_ON
	vout.ShadowPosH = CalcShadowPosH(vout.PosW, GF_SHADOW_MAP_TRANSFORM_1);
#endif
	return vout;
}


float4 ps_main(VertexOut pin) : SV_TARGET
{	
	float4 diffuse = gLight.Diffuse;
	float4 specular = gLight.Specular;

	float3 unitNormalW = normalize(pin.Normal);
	float3 tangentW = pin.Tangent;
	float3 normalMapSample = GF_TEXTURE_1.Sample(gSampleState, pin.Tex); 
	float3 normal = NormalSampleToWorldSpace(normalMapSample, unitNormalW, tangentW);
	float4 texColor = float4(1.0f, 1.0f, 1.0f, 1.0f);

	unitNormalW = -unitNormalW;
	float NdotL = dot(unitNormalW, -gLight.Direction.xyz);

	if(NdotL < 0)
	{
		return GF_AMBIENT * GF_MTRL_AMBIENT * texColor + GF_MTRL_EMISSIVE;
	}
	
	PhoneShading(pin.PosW, -gLight.Direction.xyz, unitNormalW,
		diffuse, specular, gLight.Specular.w);

#ifdef SHADOW_ON
	float shadowFactor = CalcShadowFactor(1, pin.ShadowPosH, 20);
	return GF_AMBIENT * GF_MTRL_AMBIENT * texColor 
		+ GF_MTRL_EMISSIVE + diffuse * GF_MTRL_DIFFUSE * texColor * shadowFactor
		+ specular * GF_MTRL_SPECULAR * shadowFactor;
#else
	return GF_AMBIENT * GF_MTRL_AMBIENT * texColor 
		+ GF_MTRL_EMISSIVE + diffuse * GF_MTRL_DIFFUSE * texColor
		+ specular * GF_MTRL_SPECULAR;
#endif
	//float shadowFactor = CalcShadowFactor_9x9(GF_SHADOW_MAP_1, GF_SHADOW_MAP_SIZE_1, pin.ShadowPosH);
	//float4 texColor = GF_TEXTURE.Sample(gSampleState, pin.Tex);
	//float CalcShadowFactor(Texture2D shadowMap, float4 shadowMapSize, float4 shadowPosH)
	

}

