cbuffer cbPerFrame
{
	SDirectionalLight gLight;
};

struct VertexIn
{
	float3 PosL		: POSITION;
	float3 Normal	: NORMAL;
	float3 Tangent  : TANGENT;
	float2 Tex		: TEXCOORD;

#ifdef INSTANCES_ON
	GF_DECLARE_INTANCES_VERTEX
#endif
};

#ifdef INSTANCES_ON 
#define WORLD_TRANSROM (vin.World)
#else
#define WORLD_TRANSROM (GF_WORLD)
#endif

SamplerState gSampleState;

struct VertexOut
{
	float4 PosH		: SV_POSITION;
	float3 PosW		: POSITION;
	float3 Normal	: NORMAL;
	float3 Tangent  : TANGENT;
	float2 Tex      : TEXCOORD;
};

VertexOut vs_main(VertexIn vin)
{
	VertexOut vout;
	float4 PosW = mul(float4(vin.PosL, 1.0f), WORLD_TRANSROM);
	vout.Normal = mul(vin.Normal, (float3x3)WORLD_TRANSROM);
	vout.Tangent = mul(vin.Tangent, (float3x3)WORLD_TRANSROM);
	vout.PosH = mul(PosW, GF_VIEW_PROJ);
	vout.PosW = PosW.xyz;
	vout.Tex = vin.Tex;
	return vout;
}

void shadow_vs_main(VertexIn vin,
	out float4 PosH: SV_POSITION,
	out float2 Tex : TEXCOORD)
{
	float4 PosW = mul(float4(vin.PosL, 1.0f), WORLD_TRANSROM);
	PosH = mul(PosW, GF_VIEW_PROJ);
	Tex = vin.Tex;
}

float4 ps_main(VertexOut pin) : SV_TARGET
{
	float4 texColor = GF_TEXTURE.Sample(gSampleState, pin.Tex);

#ifdef ALPHA_TEST_ON
	clip(texColor.a - 0.2f);
#endif

	float3 unitNormalW = normalize(pin.Normal);
	float3 tangentW = pin.Tangent;
	float3 normalMapSample = GF_TEXTURE_1.Sample(gSampleState, pin.Tex); 
	float3 normal = NormalSampleToWorldSpace(normalMapSample, unitNormalW, tangentW);

	float4 diffuse = gLight.Diffuse;
	float4 specular = gLight.Specular;

	PhoneShading(pin.PosW, -gLight.Direction.xyz, normal,
		diffuse, specular, gLight.Specular.w);

#ifdef SHADOW_ON
	float shadowFactor = CalcShadowFactor(1, 1.0f);

	return GF_AMBIENT * GF_MTRL_AMBIENT * texColor + GF_MTRL_EMISSIVE + 
		diffuse * GF_MTRL_DIFFUSE * shadowFactor * texColor + specular * GF_MTRL_SPECULAR * shadowFactor;
#else
	return GF_AMBIENT * GF_MTRL_AMBIENT * texColor + GF_MTRL_EMISSIVE + 
		diffuse * GF_MTRL_DIFFUSE * texColor + specular * GF_MTRL_SPECULAR;
#endif
}


SReturnGBuffers defer_ps_main(VertexOut pin)
{
	float4 texColor = GF_TEXTURE_0.Sample(gSampleState, pin.Tex);

#ifdef ALPHA_TEST_ON
	clip(texColor.a - 0.2f);
#endif

	float3 unitNormalW = normalize(pin.Normal);
	float3 tangentW = pin.Tangent;
	float3 normalMapSample = GF_TEXTURE_1.Sample(gSampleState, pin.Tex); 
	float3 normal = NormalSampleToWorldSpace(normalMapSample, unitNormalW, tangentW);

#ifdef SHADOW_ON
	float shadowFactor = CalcShadowFactor(1, 1.0f);
#else
	float shadowFactor = 1.0f;
#endif

	normal = normal * 0.5f + 0.5f;
	SReturnGBuffers pout;
	pout.GBuffer0 = float4(normal, GF_MTRL_SPECULAR.w);
	pout.GBuffer1 = GF_MTRL_DIFFUSE * texColor;
	pout.GBuffer2.x = GF_MTRL_SPECULAR.x;
	pout.GBuffer2.y = GF_MTRL_AMBIENT.x;
	pout.GBuffer2.z = shadowFactor;

	return pout;
}

void shadow_ps_main(
	in float4 PosH : SV_POSITION,
	in float2 Tex : TEXCOORD
	)
{
	float4 texColor = GF_TEXTURE.Sample(gSampleState, Tex);

#ifdef ALPHA_TEST_ON
	clip(texColor.a - 0.2f);
#endif
}

