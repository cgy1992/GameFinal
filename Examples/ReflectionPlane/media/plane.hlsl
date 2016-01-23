cbuffer cbPerFrame
{
	SDirectionalLight gDirLight;
	SPointLight gPointLight;
	
};

cbuffer cbPerObject
{
	float4x4 gReflectionMatrix;
};


Texture2D gReflectionMap;

#ifdef TEXTURE_ON
	SamplerState gSampleState;
#endif

SamplerState gRelectionSampleState;

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
	float4 PosRH    : POSITION1;
#ifdef TEXTURE_ON
	float2 Tex      : TEXCOORD;
#endif
};

VertexOut vs_main(VertexIn vin)
{
	VertexOut vout;
	float4 PosW = mul(float4(vin.PosL, 1.0f), GF_WORLD);
	vout.Normal = mul(float4(vin.Normal, 0), GF_WORLD).xyz;
	vout.PosH = mul(PosW, GF_VIEW_PROJ);
	vout.PosW = PosW.xyz;
#ifdef TEXTURE_ON
	vout.Tex = vin.Tex;
	//vout.PosRH = mul(PosW, gReflectionMatrix);
#endif

	vout.PosRH = mul(PosW, gReflectionMatrix);

	return vout;
}

float4 point_light_ps_main(VertexOut pin) : SV_TARGET
{
	float3 normal = normalize(pin.Normal);
	float4 diffuse;
	float4 specular;

#ifdef TEXTURE_ON
	float4 mtrlColor = GF_TEXTURE.Sample(gSampleState, pin.Tex);
	//float4 texColor = gReflectionMap.Sample(gSampleState, pin.Tex);
	//texColor.g = texColor.b = 0;
#else
	float4 mtrlColor = float4(1.0, 1.0, 1.0, 1.0);
#endif
	float2 reflectTexCoord;
	reflectTexCoord.x = pin.PosRH.x / pin.PosRH.w / 2.0f + 0.5f;
    reflectTexCoord.y = -pin.PosRH.y / pin.PosRH.w / 2.0f + 0.5f; 

	float4 refColor = gReflectionMap.Sample(gRelectionSampleState, reflectTexCoord);
	float4 texColor = lerp(mtrlColor, refColor, 1);


	float3 lightDir;
	ComputeIrradianceOfPointLight(pin.PosW, gPointLight, lightDir, diffuse, specular);

	PhoneShading(pin.PosW, lightDir, normal, diffuse, specular, GF_MTRL_SPECULAR.w);

#ifdef SHADOW_ON
	float shadowFactor = CalcPointLightShadowFactor(1, gPointLight.Position, GF_SHADOW_SOFTNESS);
	return GF_AMBIENT * GF_MTRL_AMBIENT * texColor + GF_MTRL_EMISSIVE + 
		diffuse * GF_MTRL_DIFFUSE * texColor * shadowFactor + specular * GF_MTRL_SPECULAR * shadowFactor;
#else
	return GF_AMBIENT * GF_MTRL_AMBIENT * texColor + GF_MTRL_EMISSIVE + 
		diffuse * GF_MTRL_DIFFUSE * texColor + specular * GF_MTRL_SPECULAR;
#endif
}
