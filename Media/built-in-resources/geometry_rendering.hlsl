cbuffer cbPerFrame
{
	SDirectionalLight gDirLight;
	SPointLight gPointLight;
};

#ifdef INSTANCES_ON 
#define WORLD_TRANSROM (vin.World)
#else
#define WORLD_TRANSROM (GF_WORLD)
#endif

#ifdef TEXTURE_ON
	SamplerState gSampleState;
#endif

struct VertexIn
{
	float3 PosL		: POSITION;
	float3 Normal	: NORMAL;
	float3 Tangent	: TANGENT;
	float2 Tex		: TEXCOORD;
#ifdef INSTANCES_ON
	GF_DECLARE_INTANCES_VERTEX
#endif
};

struct VertexOut
{
	float4 PosH		: SV_POSITION;
	float3 PosW		: POSITION;
	float3 Normal	: NORMAL;
#ifdef TEXTURE_ON
	float2 Tex      : TEXCOORD;
#endif
};

VertexOut vs_main(VertexIn vin)
{
	VertexOut vout;
	float4 PosW = mul(float4(vin.PosL, 1.0f), WORLD_TRANSROM);
	vout.Normal = mul(float4(vin.Normal, 0), WORLD_TRANSROM).xyz;
	vout.PosH = mul(PosW, GF_VIEW_PROJ);
	vout.PosW = PosW.xyz;
#ifdef TEXTURE_ON
	vout.Tex = vin.Tex;
#endif

	return vout;
}

void shadow_vs_main(VertexIn vin,
	out float4 PosH: SV_POSITION)
{
	float4 PosW = mul(float4(vin.PosL, 1.0f), WORLD_TRANSROM);
	PosH = mul(PosW, GF_VIEW_PROJ);
}

float4 dir_light_ps_main(VertexOut pin) : SV_TARGET
{
	float3 normal = normalize(pin.Normal);
	float4 diffuse = gDirLight.Diffuse;
	float4 specular = gDirLight.Specular;

#ifdef TEXTURE_ON
	float4 texColor = GF_TEXTURE.Sample(gSampleState, pin.Tex);
#else
	float4 texColor = float4(1.0, 1.0, 1.0, 1.0);
#endif

	BlinnPhoneShading(pin.PosW, -gDirLight.Direction.xyz, normal,
		diffuse, specular, GF_MTRL_SPECULAR.w);


#ifdef SHADOW_ON
	float shadowFactor = CalcShadowFactor(1, GF_SHADOW_SOFTNESS);
	
	return GF_AMBIENT * GF_MTRL_AMBIENT + GF_MTRL_EMISSIVE + 
		diffuse * GF_MTRL_DIFFUSE * texColor * shadowFactor + specular * GF_MTRL_SPECULAR * shadowFactor;
#else
	return GF_AMBIENT * GF_MTRL_AMBIENT + GF_MTRL_EMISSIVE + 
		diffuse * GF_MTRL_DIFFUSE * texColor + specular * GF_MTRL_SPECULAR;
#endif
}

float4 point_light_ps_main(VertexOut pin) : SV_TARGET
{
	float3 normal = normalize(pin.Normal);
	float4 diffuse;
	float4 specular;

#ifdef TEXTURE_ON
	float4 texColor = GF_TEXTURE.Sample(gSampleState, pin.Tex);
#else
	float4 texColor = float4(1.0, 1.0, 1.0, 1.0);
#endif

	float3 lightDir;
	ComputeIrradianceOfPointLight(pin.PosW, gPointLight, lightDir, diffuse, specular);

	PhoneShading(pin.PosW, lightDir, normal, diffuse, specular, GF_MTRL_SPECULAR.w);

#ifdef SHADOW_ON
	float shadowFactor = CalcPointLightShadowFactor(1, gPointLight.Position, GF_SHADOW_SOFTNESS);
	return GF_AMBIENT * GF_MTRL_AMBIENT + GF_MTRL_EMISSIVE + 
		diffuse * GF_MTRL_DIFFUSE * texColor * shadowFactor + specular * GF_MTRL_SPECULAR * shadowFactor;
#else
	return GF_AMBIENT * GF_MTRL_AMBIENT + GF_MTRL_EMISSIVE + 
		diffuse * GF_MTRL_DIFFUSE * texColor + specular * GF_MTRL_SPECULAR;
#endif
}

// FOR rendering point light's shadow map
struct PointVertexOut
{
	float4 PosH		: SV_POSITION;
	float3 PosW		: POSITION;
};

PointVertexOut point_shadow_vs_main(VertexIn vin)
{
	PointVertexOut vout;
	//vout.PosH = mul(float4(vin.PosL, 1.0f), GF_WVP);
	//vout.PosW = mul(float4(vin.PosL, 1.0f), GF_WORLD);
	vout.PosW = mul(float4(vin.PosL, 1.0f), GF_WORLD);
	vout.PosH = mul(float4(vout.PosW, 1.0f), GF_VIEW_PROJ);
	return vout;
}

float4 point_shadow_ps_main(PointVertexOut pin) : SV_TARGET
{
	float3 vLight = pin.PosW - GF_CAMERA_POS;
	float distSqrt = dot(vLight, vLight);

	return float4(distSqrt * 1.2f, 0, 0, 1.0f);
}
