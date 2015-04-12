cbuffer cbPerObject
{
	float4x4 	gWorld;
};

cbuffer cbPerFrame
{
	float4x4	gViewProj;
	SDirectionalLight gLight;
	float3		gEyePos;
	float4   	gFrustumPlanes[6];
};

Texture2D gTexture;
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
	float4 PosW = mul(float4(vin.PosL, 1.0f), gWorld);
	vout.PosH = mul(PosW, gViewProj);
	vout.Normal = mul(vin.NormalL, (float3x3)gWorld);
	vout.Tex = vin.Tex;
	vout.PosW = PosW.xyz;
	return vout;
} 

float4 ps_main(VertexOut pin) : SV_TARGET
{
	float3 Normal = normalize(pin.Normal);
	float NdotL = saturate(dot(Normal, -gLight.Direction));

	float4 texColor;
	float4 ratio = GF_TEXTURE_3.Sample(gSampleState, pin.Tex);
	float4 c1 = GF_TEXTURE_0.Sample(gSampleState, pin.Tex * 100.0f);
	float4 c2 = GF_TEXTURE_1.Sample(gSampleState, pin.Tex * 100.0f);
	float4 c3 = GF_TEXTURE_2.Sample(gSampleState, pin.Tex * 120.0f);

	texColor = c1 * ratio.r + c2 * ratio.g + c3 * ratio.b;
	
#ifdef SHADOW_ON

	float shadowFactor = CalcShadowFactor(1, 1.0f);
	float4 lightColor = GF_AMBIENT * GF_MTRL_AMBIENT  + NdotL * GF_MTRL_DIFFUSE * shadowFactor;
#else
	float4 lightColor = GF_AMBIENT * GF_MTRL_AMBIENT  + NdotL * GF_MTRL_DIFFUSE;
#endif
	//return texColor;
	return texColor * lightColor;
}

SReturnGBuffers defer_ps_main(VertexOut pin)
{
	float3 normal = normalize(pin.Normal);
	float4 texColor;
	float4 ratio = GF_TEXTURE_3.Sample(gSampleState, pin.Tex);
	float4 c1 = GF_TEXTURE_0.Sample(gSampleState, pin.Tex * 100.0f);
	float4 c2 = GF_TEXTURE_1.Sample(gSampleState, pin.Tex * 100.0f);
	float4 c3 = GF_TEXTURE_2.Sample(gSampleState, pin.Tex * 120.0f);

	texColor = c1 * ratio.r + c2 * ratio.g + c3 * ratio.b;
#ifdef SHADOW_ON
	float shadowFactor = CalcShadowFactor(1, 5.0f);
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

