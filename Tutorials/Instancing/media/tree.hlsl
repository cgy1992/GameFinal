cbuffer cbPerFrame
{
	SDirectionalLight gLight;
};

struct VertexIn
{
	float3 PosL		: POSITION;
	float3 Normal	: NORMAL;
	float2 Tex		: TEXCOORD;
	GF_DECLARE_INTANCES_VERTEX   // must add this when using instancing
};

SamplerState gSampleState;

struct VertexOut
{
	float4 PosH		: SV_POSITION;
	float3 PosW		: POSITION;
	float3 Normal	: NORMAL;
	float2 Tex      : TEXCOORD;
};

VertexOut vs_main(VertexIn vin)
{
	VertexOut vout;
	float4 PosW = mul(float4(vin.PosL, 1.0f), INSTANCE_WORLD);
	vout.Normal = mul(vin.Normal, (float3x3)INSTANCE_WORLD);
	vout.PosH = mul(PosW, GF_VIEW_PROJ);
	vout.PosW = PosW.xyz;
	vout.Tex = vin.Tex;
	return vout;
}


float4 ps_main(VertexOut pin) : SV_TARGET
{
	float4 texColor = GF_TEXTURE.Sample(gSampleState, pin.Tex);

//#ifdef ALPHA_TEST_ON
	clip(texColor.a - 0.2f);
//#endif

	float3 normal = normalize(pin.Normal);
	float4 diffuse = gLight.Diffuse;
	float4 specular = gLight.Specular;

	PhoneShading(pin.PosW, -gLight.Direction.xyz, normal,
		diffuse, specular, gLight.Specular.w);

	float shadowFactor = CalcShadowFactor(1, 2.0f);	
	return GF_AMBIENT * GF_MTRL_AMBIENT * texColor + GF_MTRL_EMISSIVE + 
		diffuse * GF_MTRL_DIFFUSE * shadowFactor * texColor 
		+ specular * GF_MTRL_SPECULAR * shadowFactor;

}


void shadow_vs_main(VertexIn vin,
	out float4 PosH: SV_POSITION,
	out float2 Tex : TEXCOORD)
{
	float4 PosW = mul(float4(vin.PosL, 1.0f), INSTANCE_WORLD);
	PosH = mul(PosW, GF_VIEW_PROJ);
	Tex = vin.Tex;
}

void shadow_ps_main(
	in float4 PosH : SV_POSITION,
	in float2 Tex : TEXCOORD
	)
{
	float4 texColor = GF_TEXTURE.Sample(gSampleState, Tex);

//#ifdef ALPHA_TEST_ON
	clip(texColor.a - 0.2f);
//#endif
}
