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

#ifdef SHADOW_ON
	//vout.ShadowPosH = CalcShadowPosH(vout.PosW, GF_SHADOW_MAP_TRANSFORM_1);
#endif
	return vout;
}

void shadow_vs_main(VertexIn vin,
	out float4 PosH: SV_POSITION)
{
	PosH = mul(float4(vin.PosL, 1.0f), GF_WVP);
}

float4 ps_main(VertexOut pin) : SV_TARGET
{
	float3 normal = normalize(pin.Normal);
	float4 diffuse = gLight.Diffuse;
	float4 specular = gLight.Specular;

	PhoneShading(pin.PosW, -gLight.Direction.xyz, normal,
		diffuse, specular, gLight.Specular.w);

#ifdef SHADOW_ON
	//float shadowFactor = CalcShadowFactor(1,pin.ShadowPosH,5);
	
	float shadowFactor = CalcPointLightShadowFactor(2, gPointLight.Position, 5.0f);
	return GF_AMBIENT * GF_MTRL_AMBIENT + GF_MTRL_EMISSIVE + 
		diffuse * GF_MTRL_DIFFUSE * shadowFactor + specular * GF_MTRL_SPECULAR * shadowFactor;
#else
	return GF_AMBIENT * GF_MTRL_AMBIENT + GF_MTRL_EMISSIVE + 
		diffuse * GF_MTRL_DIFFUSE + specular * GF_MTRL_SPECULAR;
#endif
}

float4 point_light_ps_main(VertexOut pin) : SV_TARGET
{
	float3 normal = normalize(pin.Normal);
	float4 diffuseSum = float4(0, 0, 0, 0);
	float4 specularSum = float4(0, 0, 0, 0);
	float3 lightDir;
	float4 diffuse;
	float4 specular;

	float4 Color = GF_AMBIENT * GF_MTRL_AMBIENT + GF_MTRL_EMISSIVE;
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

	return Color + diffuseSum * GF_MTRL_DIFFUSE + specularSum * GF_MTRL_SPECULAR;
}


struct PointVertexOut
{
	float4 PosH		: SV_POSITION;
	float3 PosW		: POSITION;
};

PointVertexOut point_shadow_vs_main(VertexIn vin)
{
	PointVertexOut vout;
	vout.PosH = mul(float4(vin.PosL, 1.0f), GF_WVP);
	vout.PosW = mul(float4(vin.PosL, 1.0f), GF_WORLD);
	return vout;
}

float4 point_shadow_ps_main(PointVertexOut pin) : SV_TARGET
{
	float3 vLight = pin.PosW - GF_CAMERA_POS;
	float distSqrt = dot(vLight, vLight);

	return float4(distSqrt * 1.2f, 0, 0, 1.0f);
}

SReturnGBuffers defer_ps_main(VertexOut pin)
{
	float3 normal = normalize(pin.Normal) * 0.5f + 0.5f;

	SReturnGBuffers pout;
	pout.GBuffer0 = float4(normal, 0);
	pout.GBuffer1 = GF_MTRL_DIFFUSE;
	pout.GBuffer2 = GF_MTRL_SPECULAR;
	pout.GBuffer3 = GF_MTRL_AMBIENT;

	return pout;
}
