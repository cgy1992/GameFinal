#include "../built-in-resources/GameFinal.hlsl"

cbuffer cbPerObject
{
	float4x4 	gWorld;
	SMaterial 	gMaterial;
};

cbuffer cbPerFrame
{
	float4x4	gViewProj;
	SDirectionalLight gLight;
	float3		gEyePos;
	float4   	gFrustumPlanes[6];
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
	float4 PosH : SV_POSITION;
	float3 Normal : NORMAL;
	float2 Tex  : TEXCOORD;	
};

VertexOut vs_main(VertexIn vin)
{
	VertexOut vout;
	float4 PosW = mul(float4(vin.PosL, 1.0f), gWorld);
	vout.PosH = mul(PosW, gViewProj);
	vout.Normal = mul(vin.NormalL, (float3x3)gWorld);
	vout.Tex = vin.Tex;
	return vout;
} 

float4 ps_main(VertexOut pin) : SV_TARGET
{
	float4 texColor;

	float4 AmbientColor = GF_AMBIENT * gMaterial.Ambient * 3.0f;

	float3 Normal = normalize(pin.Normal);

	float4 DiffuseColor = gLight.Diffuse * gMaterial.Diffuse * dot(Normal, -gLight.Direction);

	//texColor.r = abs(gFrustumPlanes[1].x);


	//float4 texColor;
	float4 ratio = GF_TEXTURE_3.Sample(gSampleState, pin.Tex);
	float4 c1 = GF_TEXTURE_0.Sample(gSampleState, pin.Tex * 20.0f);
	float4 c2 = GF_TEXTURE_1.Sample(gSampleState, pin.Tex * 10.0f);
	float4 c3 = GF_TEXTURE_2.Sample(gSampleState, pin.Tex * 12.0f);

	texColor = c1 * ratio.r + c2 * ratio.g + c3 * ratio.b;

	return texColor * (AmbientColor + DiffuseColor);
}

