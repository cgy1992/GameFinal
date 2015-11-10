cbuffer cbPerFrame
{
	SPointLight gPointLight;
};

SamplerState gSampleState;

Texture2D gTexture;
Texture2D gNormalMap;

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
	return vout;
}

float4 ground_ps_main(VertexOut pin) : SV_TARGET
{	
	float3 normal = normalize(pin.Normal);
	// sample the normal map and create the new normal vector
	float3 normalMapSample = gNormalMap.Sample(gSampleState, pin.Tex);
	normal = NormalSampleToWorldSpace(normalMapSample, normal, pin.Tangent);

	float4 diffuse;
	float4 specular;
	float4 texColor = gTexture.Sample(gSampleState, pin.Tex);

	float3 lightDir;
	ComputeIrradianceOfPointLight(pin.PosW, gPointLight, lightDir, diffuse, specular);

	PhoneShading(pin.PosW, lightDir, normal, diffuse, specular, GF_MTRL_SPECULAR.w);

	float shadowFactor = CalcPointLightShadowFactor(1, gPointLight.Position, GF_SHADOW_SOFTNESS);
	return GF_AMBIENT * GF_MTRL_AMBIENT * texColor + GF_MTRL_EMISSIVE + 
		diffuse * GF_MTRL_DIFFUSE * texColor * shadowFactor + 
		specular * GF_MTRL_SPECULAR * shadowFactor;
}

