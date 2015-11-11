cbuffer cbPerFrame
{
	SDirectionalLight gLight;
};

SamplerState gSampleState;

struct VertexIn
{
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
	float2 Tex : TEXCOORD;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 Normal : NORMAL;
	float2 Tex  : TEXCOORD;	
};

VertexOut vs_main(VertexIn vin)
{
	VertexOut vout;
	float4 PosW = mul(float4(vin.PosL, 1.0f), GF_WORLD);
	vout.PosH = mul(PosW, GF_VIEW_PROJ);
	vout.Normal = mul(vin.NormalL, (float3x3)GF_WORLD);
	vout.Tex = vin.Tex;
	vout.PosW = PosW.xyz;
	return vout;
} 

float4 ps_main(VertexOut pin) : SV_TARGET
{
	float3 Normal = normalize(pin.Normal);
	float NdotL = saturate(dot(Normal, -gLight.Direction));

	float4 texColor = GF_TEXTURE_0.Sample(gSampleState, pin.Tex * GF_TERRAIN_TEX_SCALE);

#ifdef SHADOW_ON
	float shadowFactor = CalcShadowFactor(1, GF_SHADOW_SOFTNESS);
	float4 lightColor = GF_AMBIENT * GF_MTRL_AMBIENT  + NdotL * GF_MTRL_DIFFUSE * shadowFactor;
#else
	float4 lightColor = GF_AMBIENT * GF_MTRL_AMBIENT  + NdotL * GF_MTRL_DIFFUSE;
#endif
	return texColor * lightColor;
}

