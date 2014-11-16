#ifndef __SHADOW_SHADER_HEADER__
#define __SHADOW_SHADER_HEADER__

// shadow maps
Texture2D GF_SHADOW_MAP_0;
Texture2D GF_SHADOW_MAP_1;
Texture2D GF_SHADOW_MAP_2;
Texture2D GF_SHADOW_MAP_3;
Texture2D GF_SHADOW_MAP_4;
Texture2D GF_SHADOW_MAP_5;
Texture2D GF_SHADOW_MAP_6;
Texture2D GF_SHADOW_MAP_7;

// point light shadow map
TextureCube GF_PL_SHADOW_MAP_0;
TextureCube GF_PL_SHADOW_MAP_1;
TextureCube GF_PL_SHADOW_MAP_2;
TextureCube GF_PL_SHADOW_MAP_3;
TextureCube GF_PL_SHADOW_MAP_4;
TextureCube GF_PL_SHADOW_MAP_5;
TextureCube GF_PL_SHADOW_MAP_6;
TextureCube GF_PL_SHADOW_MAP_7;
TextureCube GF_PL_SHADOW_MAP_8;
TextureCube GF_PL_SHADOW_MAP_9;
TextureCube GF_PL_SHADOW_MAP_10;
TextureCube GF_PL_SHADOW_MAP_11;
TextureCube GF_PL_SHADOW_MAP_12;
TextureCube GF_PL_SHADOW_MAP_13;
TextureCube GF_PL_SHADOW_MAP_14;
TextureCube GF_PL_SHADOW_MAP_15;

Texture2DArray GF_DIR_SHADOW_MAP_0;
Texture2DArray GF_DIR_SHADOW_MAP_1;
Texture2DArray GF_DIR_SHADOW_MAP_2;
Texture2DArray GF_DIR_SHADOW_MAP_3;
Texture2DArray GF_DIR_SHADOW_MAP_4;
Texture2DArray GF_DIR_SHADOW_MAP_5;
Texture2DArray GF_DIR_SHADOW_MAP_6;
Texture2DArray GF_DIR_SHADOW_MAP_7;

// shadow jitter, for blurring shadow
Texture3D GF_SHADOW_MAP_JITTER_TEXTURE;
Texture3D GF_PL_SHADOW_MAP_JITTER_TEXTURE;


cbuffer gf_cb_shadow_map_size_0_3
{
	float4 GF_SHADOW_MAP_SIZE_0;
	float4 GF_SHADOW_MAP_SIZE_1;
	float4 GF_SHADOW_MAP_SIZE_2;
	float4 GF_SHADOW_MAP_SIZE_3;
};

cbuffer gf_cb_shadow_map_size_4_7
{
	float4 GF_SHADOW_MAP_SIZE_4;
	float4 GF_SHADOW_MAP_SIZE_5;
	float4 GF_SHADOW_MAP_SIZE_6;
	float4 GF_SHADOW_MAP_SIZE_7;
};


#define CBUFFER_SHADOW_MAP_TRANSFORM(shadow_map_id) \
cbuffer gf_cb_shadow_map_transform_##shadow_map_id {float4x4 GF_SHADOW_MAP_TRANSFORM_##shadow_map_id;}

#define CBUFFER_DIR_SHADOW_MAP_TRANSFORM(shadow_map_id) \
cbuffer gf_cb_dir_shadow_map_transform_##shadow_map_id {float4x4 GF_DIR_SHADOW_MAP_TRANSFORMS_##shadow_map_id[4];}

#define CBUFFER_DIR_SHADOW_CROP_TRANSFORM(shadow_map_id) \
cbuffer gf_cb_dir_shadow_crop_transform_##shadow_map_id {float4x4 GF_DIR_SHADOW_CROP_TRANSFORMS_##shadow_map_id[4];}

CBUFFER_SHADOW_MAP_TRANSFORM(0);
CBUFFER_SHADOW_MAP_TRANSFORM(1);
CBUFFER_SHADOW_MAP_TRANSFORM(2);
CBUFFER_SHADOW_MAP_TRANSFORM(3);
CBUFFER_SHADOW_MAP_TRANSFORM(4);
CBUFFER_SHADOW_MAP_TRANSFORM(5);
CBUFFER_SHADOW_MAP_TRANSFORM(6);
CBUFFER_SHADOW_MAP_TRANSFORM(7);

CBUFFER_DIR_SHADOW_MAP_TRANSFORM(0);
CBUFFER_DIR_SHADOW_MAP_TRANSFORM(1);
CBUFFER_DIR_SHADOW_MAP_TRANSFORM(2);
CBUFFER_DIR_SHADOW_MAP_TRANSFORM(3);
CBUFFER_DIR_SHADOW_MAP_TRANSFORM(4);
CBUFFER_DIR_SHADOW_MAP_TRANSFORM(5);
CBUFFER_DIR_SHADOW_MAP_TRANSFORM(6);
CBUFFER_DIR_SHADOW_MAP_TRANSFORM(7);

CBUFFER_DIR_SHADOW_CROP_TRANSFORM(0);
CBUFFER_DIR_SHADOW_CROP_TRANSFORM(1);
CBUFFER_DIR_SHADOW_CROP_TRANSFORM(2);
CBUFFER_DIR_SHADOW_CROP_TRANSFORM(3);
CBUFFER_DIR_SHADOW_CROP_TRANSFORM(4);
CBUFFER_DIR_SHADOW_CROP_TRANSFORM(5);
CBUFFER_DIR_SHADOW_CROP_TRANSFORM(6);
CBUFFER_DIR_SHADOW_CROP_TRANSFORM(7);

/* built-in texture samplers. */
SamplerState GF_WRAP_LINER_SAMPLER;
SamplerState GF_WRAP_POINT_SAMPLER;
SamplerComparisonState GF_SHADOW_MAP_SAMPLER;




float CalcShadowFactor_9x9(Texture2D shadowMap, float4 shadowMapSize, float4 shadowPosH)
{
	// Complete projection by doing division by w.
	shadowPosH.xyz /= shadowPosH.w;

	// Depth in NDC space.
	float depth = shadowPosH.z;

	// Texel size.
	const float dx = shadowMapSize.z;
	const float dy = shadowMapSize.w;
	const float offsets_x[9] = { -dx * 4, -dx * 3, -dx * 2, -dx, 0, dx, dx * 2, dx * 3, dx * 4 };
	const float offsets_y[9] = { -dy * 4, -dy * 3, -dy * 2, -dy, 0, dy, dy * 2, dy * 3, dy * 4 };
	const float blurDistance = 3.0f;
	float percentLit = 0.0f;

	[unroll]
	for (int i = 0; i < 9; i++)
	{
		[unroll]
		for (int j = 0; j < 9; j++)
		{
			percentLit += shadowMap.SampleCmpLevelZero(GF_SHADOW_MAP_SAMPLER,
				shadowPosH.xy + float2(offsets_x[i], offsets_y[j])*blurDistance, depth).r;
		}
	}

	return percentLit /= 81.0f;
}

float CalcShadowFactor_3x3(Texture2D shadowMap, float4 shadowMapSize, float4 shadowPosH)
{
	// Complete projection by doing division by w.
	shadowPosH.xyz /= shadowPosH.w;

	// Depth in NDC space.
	float depth = shadowPosH.z;

	// Texel size.
	const float dx = shadowMapSize.z;
	const float dy = shadowMapSize.w;

	float percentLit = 0.0f;
	const float2 offsets[9] =
	{
		float2(-dx, -dy), float2(0.0f, -dy), float2(dx, -dy),
		float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
		float2(-dx, +dy), float2(0.0f, +dy), float2(dx, +dy)
	};

	[unroll]
	for (int i = 0; i < 9; ++i)
	{
		percentLit += shadowMap.SampleCmpLevelZero(GF_SHADOW_MAP_SAMPLER,
			shadowPosH.xy + offsets[i], depth).r;
	}

	return percentLit /= 9.0f;
}




float CalcShadowFactor_Jitter(Texture2DArray shadowMap, float4 shadowMapSize,
	float4x4 shadowTransforms[4],
	float2 screenPos, float3 posW, float blurRadius)
{
#define JITTER_SAMPLES_COUNT 64
#define JITTER_SAMPLES_COUNT_DIV_2 32
#define INV_JITTER_SAMPLES_COUNT (1.0f / JITTER_SAMPLES_COUNT)
#define INV_JITTER_SAMPLES_COUNT_DIV_2 (1.0f / JITTER_SAMPLES_COUNT_DIV_2)

	// Complete projection by doing division by w.

	// position in view space
	float4 posV = mul(float4(posW, 1.0f), GF_VIEW);
	float distToCamera = posV.z;
	uint segmentid = 0;
	while(segmentid < 4)
	{
		if(distToCamera < GF_CAMERA_SEGMENTS[segmentid])
			break;

		segmentid = segmentid + 1;
	}

	if(segmentid >= 4)
		return 1.0f;

	float4 shadowPosH = mul(float4(posW, 1.0f), shadowTransforms[segmentid]);

	shadowPosH.xyz /= shadowPosH.w;

	// Depth in NDC space.
	float depth = shadowPosH.z;
	if(depth > 1.0f)
		return 1.0f;


	// Texel size.
	float2 fsize = float2(shadowMapSize.z, shadowMapSize.w) * blurRadius;

	float percentLit = 0.0f;
	float3 jcoord = float3(screenPos, 0);
	float3 smcoord = float3(0, 0, segmentid);

		[unroll]
	for (int i = 0; i < 4; i++)
	{
		float4 offset = GF_SHADOW_MAP_JITTER_TEXTURE.Sample(GF_WRAP_POINT_SAMPLER, jcoord);

		smcoord.xy = fsize * offset.xy + shadowPosH.xy;
		percentLit += shadowMap.SampleCmpLevelZero(GF_SHADOW_MAP_SAMPLER, smcoord, depth).r;

		smcoord.xy = fsize * offset.zw + shadowPosH.xy;
		percentLit += shadowMap.SampleCmpLevelZero(GF_SHADOW_MAP_SAMPLER, smcoord, depth).r;

		jcoord.z += INV_JITTER_SAMPLES_COUNT_DIV_2;
	}


	if (percentLit * (percentLit - 8.0f) == 0)
	{
		return percentLit / 8.0f;
	}

	[unroll]
	for (int i = 4; i < JITTER_SAMPLES_COUNT_DIV_2; i++)
	{
		float4 offset = GF_SHADOW_MAP_JITTER_TEXTURE.Sample(GF_WRAP_POINT_SAMPLER, jcoord);

		smcoord.xy = fsize * offset.xy + shadowPosH.xy;
		percentLit += shadowMap.SampleCmpLevelZero(GF_SHADOW_MAP_SAMPLER, smcoord, depth).r;

		smcoord.xy = fsize * offset.zw + shadowPosH.xy;
		percentLit += shadowMap.SampleCmpLevelZero(GF_SHADOW_MAP_SAMPLER, smcoord, depth).r;

		jcoord.z += INV_JITTER_SAMPLES_COUNT_DIV_2;
	}

	return percentLit * INV_JITTER_SAMPLES_COUNT;
}

#ifndef CalcShadowFactor
#define CalcShadowFactor(ShadowLightID, BlurRadius) \
	CalcShadowFactor_Jitter(GF_DIR_SHADOW_MAP_##ShadowLightID, GF_SHADOW_MAP_SIZE_##ShadowLightID, \
		GF_DIR_SHADOW_MAP_TRANSFORMS_##ShadowLightID,	\
		pin.PosH.xy/pin.PosH.w, pin.PosW.xyz, BlurRadius)
#endif


float CalcPointLightShadowFactor_3x3x3(TextureCube shadowMap, float3 PosW, float3 LightPos,
		float4 shadowMapSize, float blurDistance)
{
	float3 vLight = PosW - LightPos;
	float distSqrt = dot(vLight, vLight);
	
	vLight = normalize(vLight);

	float dx = shadowMapSize.z;
	float offsets[3] = {-dx, 0, dx};
	float percentLit = 0.0f;

	[unroll]
	for (int i = 0; i < 3; i++)
	{
		[unroll]
		for(int j = 0; j < 3; j++)
		{
			[unroll]
			for(int k = 0; k < 3; k++)
			{
				percentLit += shadowMap.SampleCmpLevelZero(GF_SHADOW_MAP_SAMPLER,
					vLight + float3(offsets[i], offsets[j], offsets[k])*blurDistance, distSqrt).r;
			}
		}
	}
	return percentLit / 27;
}

float CalcPointLightShadowFactor_Jitter(TextureCube shadowMap, float3 PosW, float3 LightPos, 
	float2 screenPos, float4 shadowMapSize, float blurDistance)
{
#define PL_JITTER_SAMPLES_COUNT 16
#define INV_PL_JITTER_SAMPLES_COUNT (1.0f / PL_JITTER_SAMPLES_COUNT)

	float3 vLight = PosW - LightPos;
	float distSqrt = dot(vLight, vLight);
	vLight = normalize(vLight);

	float fsize = shadowMapSize.z * blurDistance;
	float3 jcoord = float3(screenPos, 0);
	float percentLit = 0.0f;

	[unroll]
	for(int i = 0; i < 4; i++)
	{
		float3 offset = GF_SHADOW_MAP_JITTER_TEXTURE.Sample(GF_WRAP_POINT_SAMPLER, jcoord).xyz;
		percentLit += shadowMap.SampleCmpLevelZero(GF_SHADOW_MAP_SAMPLER, vLight + offset * fsize, distSqrt).r;
		jcoord.z += INV_PL_JITTER_SAMPLES_COUNT;
	}

	
	if(percentLit * (percentLit - 4.0f) == 0)
	{
		return percentLit * 0.25f;
	}

	[unroll]
	for(int i = 4; i < PL_JITTER_SAMPLES_COUNT; i++)
	{
		float3 offset = GF_SHADOW_MAP_JITTER_TEXTURE.Sample(GF_WRAP_POINT_SAMPLER, jcoord).xyz;
		percentLit += shadowMap.SampleCmpLevelZero(GF_SHADOW_MAP_SAMPLER, vLight + offset * fsize, distSqrt).r;
		jcoord.z += INV_PL_JITTER_SAMPLES_COUNT;
	}

	return percentLit * INV_PL_JITTER_SAMPLES_COUNT;
}

#ifndef PS_ShadowPosH
#define PS_ShadowPosH(ShadowLightID) \
	mul(float4(pin.PosW.xyz, 1.0f), GF_SHADOW_MAP_TRANSFORM_##ShadowLightID)
#endif



#ifndef CalcPointLightShadowFactor
#define CalcPointLightShadowFactor(ShadowLightID, LightPos, BlurRadius) \
	CalcPointLightShadowFactor_Jitter(GF_PL_SHADOW_MAP_##ShadowLightID, pin.PosW, LightPos, \
		pin.PosH.xy/pin.PosH.w, GF_SHADOW_MAP_SIZE_##ShadowLightID, BlurRadius)
#endif


#endif
