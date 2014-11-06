#include "../built-in-resources/GameFinal.hlsl"

Texture2D gTexture1;
SamplerState sampleType;

cbuffer cbPerObject
{
	float4x4 gWorld;
};

cbuffer cbPerFrame
{
	float4x4		gViewProj;
	//SPointLight		gLight;
	SMaterial		gMaterial;
	float3			gEyePos;
};

struct VertexOut
{
	float4 PosH		: SV_POSITION;
	float3 PosW		: POSITION;
	float3 Normal	: NORMAL;
	float2 Tex		: TEXCOORD;
};

float4 ps_main(VertexOut pin) : SV_TARGET
{
	float3 PosW = pin.PosW;
	float3 Normal = normalize(pin.Normal);
	float4 Color = GF_AMBIENT * gMaterial.Ambient + gMaterial.Emissive;
	float4 Specular = 0;
	float4 Diffuse = 0;

	for(uint i = 0; i < GF_POINT_LIGHTS_NUM; i++)
	{
		SPointLight light = GF_POINT_LIGHTS[i];
		float3 lightDir = PosW - light.Position;
		float dist = length(lightDir);
		lightDir /= dist;

		float diffAtten = dot(-lightDir, Normal);
		
		[flatten]
		if (diffAtten > 0)
		{
			float3 toEye = normalize(gEyePos - PosW);
			float3 refLightDir = reflect(lightDir, Normal);

			float specAtten = dot(refLightDir, toEye);
			float distAtten = 1.0 / dot(light.Attenuations, float3(1, dist, dist * dist));

			Diffuse = light.Diffuse * diffAtten * distAtten * gMaterial.Diffuse;
			Specular = light.Specular * pow(saturate(specAtten), 32) * distAtten * gMaterial.Specular;
		}
	}	

	float4 texColor = gTexture1.Sample(sampleType, pin.Tex);
	//float width, height, NumberOfSamples;
	//gTexture1.GetDimensions(width, height, NumberOfSamples);
	//float4 texColor = gTexture1.Load(pin.Tex * float2(width, height), 1);
	Color = texColor * (Color + Diffuse) + Specular;
	Color.a = 0.5f;
	//float4 color = gTexture1.Sample(sampleType, pin.Tex) * Color;
	return Color;
}

float4 border_ps_main(VertexOut pin) : SV_TARGET
{
	return float4(1.0f, 1.0f, 0.0f, 1.0f);
}