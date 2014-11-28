cbuffer cbPerObject
{
	float4x4 gWorld;
};

cbuffer cbPerFrame
{
	float4x4 gViewProj;
};

Texture2D gTexture;
SamplerState gSampleState;

struct VertexIn
{
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
	float3 TangentL : TANGENT;
	float2 Tex : TEXCOORD;
};

struct VertexOut
{
	float4 PosH		: SV_POSITION;
	float3 PosW		: POSITION;
	float3 Normal	: NORMAL; 
	float3 Tangent  : TANGENT;
	float2 Tex		: TEXCOORD;
};

VertexOut vs_main(VertexIn vin)
{
	VertexOut vout;
	float4 PosW = mul(float4(vin.PosL, 1.0f), GF_WORLD);
	vout.PosH = mul(PosW, GF_VIEW_PROJ);
	vout.PosW = PosW.xyz;
	vout.Normal = mul(vin.NormalL, (float3x3)GF_WORLD);
	vout.Tangent = mul(vin.TangentL, (float3x3)GF_WORLD);
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

	float3 unitNormalW = normalize(pin.Normal);
	//float3 tangentW = pin.Tangent;
	//float3 normalMapSample = GF_TEXTURE_1.Sample(gSampleState, pin.Tex); 
	//float3 normal = NormalSampleToWorldSpace(normalMapSample, unitNormalW, tangentW);
	float3 normal = unitNormalW;
	for(uint i = 0; i < GF_POINT_LIGHTS_NUM; i++)
	{
		if(ComputeIrradianceOfPointLight(pin.PosW, GF_POINT_LIGHTS[i], lightDir, diffuse, specular))
		{
			PhoneShading(pin.PosW, lightDir, normal, diffuse, specular, 32);
			sumDiffuse += diffuse;
			sumSpecular += specular;
		}
	}

	float4 texColor = GF_TEXTURE.Sample(gSampleState, pin.Tex);

	return (GF_AMBIENT * GF_MTRL_AMBIENT + GF_MTRL_EMISSIVE + 
		sumDiffuse * GF_MTRL_DIFFUSE) * texColor + sumSpecular * GF_MTRL_SPECULAR;
}

