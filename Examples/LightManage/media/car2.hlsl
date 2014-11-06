#include "../../../Media/built-in-resources/GameFinal.hlsl"

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
	float2 Tex : TEXCOORD;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD;
};

SamplerState gSampleState;

VertexOut vs_main(VertexIn vin)
{
	VertexOut vout;
	float4 PosW = mul(float4(vin.PosL, 1.0f), gWorld);
		vout.PosH = mul(PosW, gViewProj);
	vout.PosW = PosW.xyz;
	vout.Normal = mul(vin.NormalL, (float3x3)gWorld);
	vout.Tex = vin.Tex;
	return vout;
}

float4 ps_main(VertexOut pin) : SV_TARGET
{
	float4 sumDiffuse = 0;
	float4 sumSpecular = 0;

	float4 diffuse = 0;
	float4 specular = 0;
	float3 lightDir;

	float3 normal = normalize(pin.Normal);

	for (uint i = 0; i < GF_POINT_LIGHTS_NUM; i++)
	{
		if (ComputeIrradianceOfPointLight(pin.PosW, GF_POINT_LIGHTS[i], lightDir, diffuse, specular))
		{
			PhoneShading(pin.PosW, lightDir, normal, diffuse, specular, 32);
			sumDiffuse += diffuse;
			sumSpecular += specular;
		}
	}

	float4 texColor = GF_TEXTURE.Sample(gSampleState, pin.Tex);

	return (GF_AMBIENT * GF_MTRL_AMBIENT +
		sumDiffuse * GF_MTRL_DIFFUSE) * texColor + sumSpecular * GF_MTRL_SPECULAR * 0.4f;
}

