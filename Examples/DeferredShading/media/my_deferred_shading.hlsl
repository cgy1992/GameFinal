#ifndef __MY_DEFERRED_SHADING_SHADER_HEADER__
#define __MY_DEFERRED_SHADING_SHADER_HEADER__ 

Texture2D gDepthBuffer;
Texture2D gGBuffer0;
Texture2D gGBuffer1;
Texture2D gGBuffer2;
Texture2D gGBuffer3;
SamplerState gSamplerState;

cbuffer cbPerFrame
{
	SPointLight gPointLight;
	SDirectionalLight gLight;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float2 Tex : TEXCOORD;
};

/*
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


float4 point_light_ps_main(VertexOut pin) : SV_TARGET
{
	float3 normal = normalize(pin.Normal);
	float4 diffuse;
	float4 specular;

//	bool ComputeIrradianceOfPointLight(float3 pos, SPointLight light, out float3 lightDir,
//	out float4 diffuse, out float4 specular)
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
*/

float4 point_light_ps_main(VertexOut pin) : SV_TARGET
{
	//ComputeWorldPosFromDepthBuffer(Texture2D zbuffer, SamplerState samplerState, float2 Tex)
	float4 posH;
	//posH.xy = pin.PosH.xy / pin.PosH.w;
	posH.xy = pin.Tex * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);
	posH.z = gDepthBuffer.Sample(gSamplerState, pin.Tex).r; 
	posH.w = 1.0f;

	clip(1.0f - posH.z);

	float4 posW = mul(posH, GF_INV_VIEW_PROJ);
	posW = posW / posW.w;
	float3 PosW = posW.xyz;

	//PosW = ComputeWorldPosFromDepthBuffer(gDepthBuffer, gSamplerState, pin.Tex);
	float3 normal = gGBuffer0.Sample(gSamplerState, pin.Tex).xyz * 2.0f - 1.0f;
	float4 diffuseMtrl = gGBuffer1.Sample(gSamplerState, pin.Tex);
	float4 specularMtrl = gGBuffer2.Sample(gSamplerState, pin.Tex);
	float4 ambientMtrl = gGBuffer3.Sample(gSamplerState, pin.Tex);

	float4 diffuse;
	float4 specular;
	float3 lightDir;
	ComputeIrradianceOfPointLight(PosW, gPointLight, lightDir, diffuse, specular);

	PhoneShading(PosW, lightDir, normal, diffuse, specular, gPointLight.Specular.w);

	//return gGBuffer1.Sample(gSamplerState, pin.Tex);

#ifdef SHADOW_ON
	float2 screenPos = pin.Tex * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);
	float shadowFactor = CalcPointLightShadowFactor_Jitter(GF_PL_SHADOW_MAP_2, PosW, gPointLight.Position, 
			pin.PosH.xy / pin.PosH.w * 29.83f, GF_SHADOW_MAP_SIZE_2, 10.0f);
//CalcPointLightShadowFactor_Jitter(GF_PL_SHADOW_MAP_##ShadowLightID, pin.PosW, LightPos, \
//		pin.PosH.xy/pin.PosH.w, GF_SHADOW_MAP_SIZE_##ShadowLightID, BlurRadius)

	return GF_AMBIENT * ambientMtrl + 
		diffuse * diffuseMtrl * shadowFactor + specular * specularMtrl * shadowFactor;
#else
	return GF_AMBIENT * ambientMtrl + 
		diffuse * diffuseMtrl + specular * specularMtrl;
#endif
}


float4 ps_main(VertexOut pin) : SV_TARGET
{
	float4 posH;
	posH.xy = pin.Tex * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);
	posH.z = gDepthBuffer.Sample(gSamplerState, pin.Tex).r; 
	posH.w = 1.0f;

	clip(1.0f - posH.z);

	float4 posW = mul(posH, GF_INV_VIEW_PROJ);
	posW = posW / posW.w;
	float3 PosW = posW.xyz;

	//PosW = ComputeWorldPosFromDepthBuffer(gDepthBuffer, gSamplerState, pin.Tex);
	float3 normal = gGBuffer0.Sample(gSamplerState, pin.Tex).xyz * 2.0f - 1.0f;
	float4 diffuseMtrl = gGBuffer1.Sample(gSamplerState, pin.Tex);
	float4 specularMtrl = gGBuffer2.Sample(gSamplerState, pin.Tex);
	float4 ambientMtrl = gGBuffer3.Sample(gSamplerState, pin.Tex);

	float4 diffuse = gLight.Diffuse;
	float4 specular = gLight.Specular;

	float NdotL = dot(normal, -gLight.Direction.xyz);

	if(NdotL < 0)
	{
		return GF_AMBIENT * ambientMtrl;
	}

	PhoneShading(PosW, -gLight.Direction.xyz, normal,
		diffuse, specular, gLight.Specular.w);

#ifdef SHADOW_ON

	float shadowFactor = CalcShadowFactor_Jitter(GF_DIR_SHADOW_MAP_1, GF_SHADOW_MAP_SIZE_1, 
		GF_DIR_SHADOW_MAP_TRANSFORMS_1, pin.PosH.xy/pin.PosH.w * 29.454f, PosW, 10.0f);

	return GF_AMBIENT * ambientMtrl + 
		diffuse * diffuseMtrl * shadowFactor + specular * specularMtrl * shadowFactor;
#else
	return GF_AMBIENT * ambientMtrl + 
		diffuse * diffuseMtrl + specular * specularMtrl;
#endif
	
}


#endif