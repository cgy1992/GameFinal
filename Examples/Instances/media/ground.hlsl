//#include "../../../Media/built-in-resources/GameFinal.hlsl"

SamplerState gSampleState;

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
};

struct VertexOut
{
	float4 PosH		: SV_POSITION;
	float3 PosW		: POSITION;
	float3 Normal	: NORMAL; 
	float3 Tangent  : TANGENT;
	float3 Tex		: TEXCOORD;
};

VertexOut vs_main(VertexIn vin)
{
	VertexOut vout;
	float4 PosW = mul(float4(vin.PosL, 1.0f), GF_WORLD);
	vout.PosH = mul(PosW, GF_VIEW_PROJ);
	vout.PosW = PosW.xyz;
	vout.Normal = mul(vin.Normal, (float3x3)GF_WORLD);
	vout.Tangent = mul(vin.Tangent, (float3x3)GF_WORLD);
	vout.Tex = vin.PosL;
	return vout;
}


float4 ps_main(VertexOut pin) : SV_TARGET
{	
	float4 diffuse = gLight.Diffuse;
	float4 specular = gLight.Specular;

	float3 normal = normalize(pin.Normal);

	float NdotL = dot(normal, -gLight.Direction.xyz);

	if(NdotL < 0)
	{
		return GF_AMBIENT * GF_MTRL_AMBIENT + GF_MTRL_EMISSIVE;
	}
	
	PhoneShading(pin.PosW, -gLight.Direction.xyz, normal,
		diffuse, specular, gLight.Specular.w);

#ifdef SHADOW_ON
	//float shadowFactor = CalcShadowFactor(1, pin.ShadowPosH, 2.0f);
	//float shadowFactor = CalcPointLightShadowFactor(GF_PL_SHADOW_MAP_2, pin.PosW, gPointLight.Position);
	float shadowFactor = CalcShadowFactor(1, 2.0f);

	return GF_AMBIENT * GF_MTRL_AMBIENT 
		+ GF_MTRL_EMISSIVE + diffuse * GF_MTRL_DIFFUSE * shadowFactor
		+ specular * GF_MTRL_SPECULAR * shadowFactor;
#else
	return GF_AMBIENT * GF_MTRL_AMBIENT 
		+ GF_MTRL_EMISSIVE + diffuse * GF_MTRL_DIFFUSE
		+ specular * GF_MTRL_SPECULAR;
#endif
	//float shadowFactor = CalcShadowFactor_9x9(GF_SHADOW_MAP_1, GF_SHADOW_MAP_SIZE_1, pin.ShadowPosH);
	//float4 texColor = GF_TEXTURE.Sample(gSampleState, pin.Tex);
	//float CalcShadowFactor(Texture2D shadowMap, float4 shadowMapSize, float4 shadowPosH)
}

float4 point_light_ps_main(VertexOut pin) : SV_TARGET
{
	float3 normal = normalize(pin.Normal);
	float4 diffuse;
	float4 specular;

	float3 lightDir;
	ComputeIrradianceOfPointLight(pin.PosW, gPointLight, lightDir, diffuse, specular);

	PhoneShading(pin.PosW, lightDir, normal, diffuse, specular, gPointLight.Specular.w);

#ifdef SHADOW_ON
	float shadowFactor = CalcPointLightShadowFactor(2, gPointLight.Position, 10.0f);
	return GF_AMBIENT * GF_MTRL_AMBIENT + GF_MTRL_EMISSIVE + 
		diffuse * GF_MTRL_DIFFUSE * shadowFactor + specular * GF_MTRL_SPECULAR * shadowFactor;
#else
	return GF_AMBIENT * GF_MTRL_AMBIENT + GF_MTRL_EMISSIVE + 
		diffuse * GF_MTRL_DIFFUSE + specular * GF_MTRL_SPECULAR;
#endif
}
