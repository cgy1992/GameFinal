#include "../built-in-resources/GameFinal.hlsl"

cbuffer cbPerObject
{
	float4x4 	gWorld;
	SMaterial	gMaterial;
};

cbuffer cbPerFrame
{
	float4x4		gViewProj;
	SPointLight		gLight;
	float3			gEyePos;
};

Texture2D gTexture;
SamplerState gSampleState;

struct VertexIn
{
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
	float2 Tex : TEXCOORD;
};

struct VertexOut
{
	float4 PosH : 	SV_POSITION;
	float3 PosW		: POSITION;
	float3 Normal	: NORMAL; 
	float2 Tex		: TEXCOORD;
};

VertexOut vs_main(VertexIn vin)
{
	VertexOut vout;
	float4 PosW = mul(float4(vin.PosL, 1.0f), gWorld);
	vout.PosW = PosW;
	vout.PosH = mul(PosW, gViewProj);
	vout.Normal = mul(vin.NormalL, (float3x3)gWorld);
	vout.Tex = vin.Tex;
	return vout;
}

float4 ps_main(VertexOut pin) : SV_TARGET
{
	float3 PosW = pin.PosW;
	float3 Normal = normalize(pin.Normal);

	float4 Color = GF_AMBIENT * gMaterial.Ambient + gMaterial.Emissive;
	float3 lightDir = PosW - gLight.Position;
	float dist = length(lightDir);
	lightDir /= dist;

	float diffAtten = dot(-lightDir, Normal);
	float4 Specular = 0;
	float4 Diffuse = 0;

	[flatten]
	if (diffAtten > 0)
	{
		float3 toEye = normalize(gEyePos - PosW);
		float3 refLightDir = reflect(lightDir, Normal);

		float specAtten = dot(refLightDir, toEye);
		float distAtten = 1.0 / dot(gLight.Attenuations, float3(1, dist, dist * dist));

		Diffuse = gLight.Diffuse * diffAtten * distAtten * gMaterial.Diffuse;
		Specular = gLight.Specular * pow(saturate(specAtten), 32) * distAtten * gMaterial.Specular;
	}

	float4 texColor = gTexture.Sample(gSampleState, pin.Tex);
	Color = (Color + Diffuse) * texColor + Specular;
	Color.a = 0.5f;
	//float4 color = gTexture1.Sample(sampleType, pin.Tex) * Color;
	return Color;

	//return gTexture.Sample(gSampleState, pin.Tex);
}

