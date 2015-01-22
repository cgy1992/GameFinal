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
	float2 Tex		: TEXCOORD;
#ifdef SHADOW_ON
	//float4 ShadowPosH : TEXCOORD1;
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
	//vout.ShadowPosH = CalcShadowPosH(vout.PosW, GF_SHADOW_MAP_TRANSFORM_1);
#endif
	return vout;
}


float4 ps_main(VertexOut pin) : SV_TARGET
{	
	float4 diffuse = gLight.Diffuse;
	float4 specular = gLight.Specular;

	float3 normal = -normalize(pin.Normal);

	float NdotL = dot(normal, -gLight.Direction.xyz);

	if(NdotL < 0)
	{
		return GF_AMBIENT * GF_MTRL_AMBIENT + GF_MTRL_EMISSIVE;
	}
	
	PhoneShading(pin.PosW, -gLight.Direction.xyz, normal,
		diffuse, specular, gLight.Specular.w);

#ifdef SHADOW_ON
	//float shadowFactor = CalcShadowFactor(1, pin.ShadowPosH, 20);
	float shadowFactor = CalcPointLightShadowFactor(2, gPointLight.Position, 2.0f);

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
	float3 normal = -normalize(pin.Normal);
	float3 normalMapSample = GF_TEXTURE_1.Sample(gSampleState, pin.Tex * 3);
	normal = NormalSampleToWorldSpace(normalMapSample, normal, pin.Tangent);
	float4 texColor = GF_TEXTURE_0.Sample(gSampleState, pin.Tex * 3);

	float4 diffuseSum = float4(0, 0, 0, 0);
	float4 specularSum = float4(0, 0, 0, 0);
	float4 diffuse;
	float4 specular;
	float3 lightDir;
	
	float4 Color = GF_AMBIENT * GF_MTRL_AMBIENT * texColor + GF_MTRL_EMISSIVE;
	for(int i = 0; i < GF_POINT_LIGHTS_NUM; i++)
	{
		SPointLight pointLight = GF_POINT_LIGHTS[i];
		if(ComputeIrradianceOfPointLight(pin.PosW, pointLight, lightDir, diffuse, specular))
		{
			BlinnPhoneShading(pin.PosW, lightDir, normal, diffuse, specular, pointLight.Specular.w);
			
			diffuseSum += diffuse;
			specularSum += specular;
		}
	}

	return Color + diffuseSum * texColor * GF_MTRL_DIFFUSE + specularSum * GF_MTRL_SPECULAR;
}

SReturnGBuffers defer_ps_main(VertexOut pin)
{
	float3 normal = -normalize(pin.Normal);
	float3 normalMapSample = GF_TEXTURE_1.Sample(gSampleState, pin.Tex * 3);
	normal = NormalSampleToWorldSpace(normalMapSample, normal, pin.Tangent);

	normal = normal * 0.5f + 0.5f;

	float4 texColor = GF_TEXTURE_0.Sample(gSampleState, pin.Tex * 3);

	SReturnGBuffers pout;
	pout.GBuffer0 = float4(normal, 0);
	pout.GBuffer1 = GF_MTRL_DIFFUSE * texColor;
	pout.GBuffer2 = GF_MTRL_SPECULAR;
	pout.GBuffer3 = GF_MTRL_AMBIENT * texColor;

	return pout;
}
