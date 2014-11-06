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
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 Normal : NORMAL;
};

VertexOut vs_main(VertexIn vin)
{
	VertexOut vout;
	float4 PosW = mul(float4(vin.PosL, 1.0f), gWorld);
	vout.PosW = PosW.xyz;
	vout.PosH = mul(PosW, gViewProj);
	vout.Normal = mul(vin.NormalL, (float3x3)gWorld);

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

	for(uint i = 0; i < GF_POINT_LIGHTS_NUM; i++)
	{
		if(ComputeIrradianceOfPointLight(pin.PosW, GF_POINT_LIGHTS[i], lightDir, diffuse, specular))
		{
			PhoneShading(pin.PosW, lightDir, normal, diffuse, specular, 32);
			sumDiffuse += diffuse;
			sumSpecular += specular;
		}
	}

//	float4 texColor = GF_TEXTURE.Sample(gSampleState, pin.Tex);

	return (GF_AMBIENT * GF_MTRL_AMBIENT + GF_MTRL_EMISSIVE) * 0.1f + sumDiffuse * GF_MTRL_DIFFUSE + sumSpecular * GF_MTRL_SPECULAR * 20;
}

