SamplerState gSampleState;

cbuffer cbPerFrame
{
	SDirectionalLight gDirLight;
};

#ifdef INSTANCES_ON 
#define WORLD_TRANSROM (vin.World)
#else
#define WORLD_TRANSROM (GF_WORLD)
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
};

VertexOut vs_main(VertexIn vin)
{
	VertexOut vout;
	float4 PosW = mul(float4(vin.PosL, 1.0f), WORLD_TRANSROM);
	vout.Normal = mul(float4(vin.Normal, 0), GF_INV_TRAN_WORLD).xyz;
	vout.PosH = mul(PosW, GF_VIEW_PROJ);
	vout.PosW = PosW.xyz;
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

	PhoneShading(pin.PosW, -gDirLight.Direction.xyz, normal,
		diffuse, specular, GF_MTRL_SPECULAR.w);


#ifdef SHADOW_ON
	float shadowFactor = CalcShadowFactor(1, 1.0f);
	
	return GF_AMBIENT * GF_MTRL_AMBIENT + GF_MTRL_EMISSIVE + 
		diffuse * GF_MTRL_DIFFUSE * shadowFactor + specular * GF_MTRL_SPECULAR * shadowFactor;
#else
	return GF_AMBIENT * GF_MTRL_AMBIENT + GF_MTRL_EMISSIVE + 
		diffuse * GF_MTRL_DIFFUSE + specular * GF_MTRL_SPECULAR;
#endif
}

