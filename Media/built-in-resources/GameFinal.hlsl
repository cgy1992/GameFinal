#ifndef __GAME_FINAL_SHADER_HEADER__
#define __GAME_FINAL_SHADER_HEADER__

#ifndef GF_TEXTURE
#define GF_TEXTURE GF_TEXTURE_0
#endif

struct SDirectionalLight
{
	float4	Diffuse;
	float4	Specular;
	float4	Direction;
};

struct SPointLight
{
	float4	Diffuse;
	float4	Specular;
	float3	Position;
	float	Range;
	float3  Attenuations;
	float   _pad;
};

struct SSpotLight
{
	float4	Diffuse;
	float4	Specular;
	float4	Direction;
	float3	Position;
	float	Range;
	float3	Attenuations;
	float	Falloff;
	float	InnerCone;
	float	OuterCone;
	float2  _pad;
};

struct SMaterial
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float4 Emissive;
};

cbuffer gf_cb_world
{
	float4x4 GF_WORLD;
	float4x4 GF_INV_WORLD;
	float4x4 GF_TRAN_WORLD;
	float4x4 GF_INV_TRAN_WORLD;
};

cbuffer gf_cb_view
{
	float4x4 GF_VIEW;
	float4x4 GF_INV_VIEW;
	float4x4 GF_TRAN_VIEW;
	float4x4 GF_INV_TRAN_VIEW;
};

cbuffer gf_cb_world_view
{
	float4x4 GF_WORLD_VIEW;
	float4x4 GF_INV_WORLD_VIEW;
	float4x4 GF_TRAN_WORLD_VIEW;
	float4x4 GF_INV_TRAN_WORLD_VIEW;
};

cbuffer gf_cb_wvp
{
	float4x4 GF_WVP;
	float4x4 GF_INV_WVP;
	float4x4 GF_TRAN_WVP;
	float4x4 GF_INV_TRAN_WVP;
};

cbuffer gf_cb_proj
{
	float4x4 GF_PROJ;
	float4x4 GF_INV_PROJ;
	float4x4 GF_TRAN_PROJ;
	float4x4 GF_INV_TRAN_PROJ;
};

cbuffer gf_cb_view_proj
{
	float4x4 GF_VIEW_PROJ;
	float4x4 GF_INV_VIEW_PROJ;
	float4x4 GF_TRAN_VIEW_PROJ;
	float4x4 GF_INV_TRAN_VIEW_PROJ;
};

cbuffer gf_cb_camera_pos
{
	float3 GF_CAMERA_POS;
};

cbuffer gf_cb_material
{
	float4 GF_MTRL_AMBIENT;
	float4 GF_MTRL_DIFFUSE;
	float4 GF_MTRL_SPECULAR;
	float4 GF_MTRL_EMISSIVE;
};

cbuffer gf_cb_screen_size
{
	float4  GF_SCREEN_SIZE;
};

cbuffer gf_cb_scene_info
{
	float4 GF_AMBIENT;
};

cbuffer gf_cb_point_lights
{
	SPointLight		GF_POINT_LIGHTS[32];
	uint			GF_POINT_LIGHTS_NUM;
};

cbuffer gf_cb_directional_lights
{
	SDirectionalLight		GF_DIR_LIGHTS[8];
	uint					GF_DIR_LIGHTS_NUM;
};

Texture2D GF_TEXTURE_0;
Texture2D GF_TEXTURE_1;
Texture2D GF_TEXTURE_2;
Texture2D GF_TEXTURE_3;
Texture2D GF_TEXTURE_4;
Texture2D GF_TEXTURE_5;
Texture2D GF_TEXTURE_6;
Texture2D GF_TEXTURE_7;


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

// shadow jitter, for blurring shadow
Texture3D GF_SHADOW_MAP_JITTER_TEXTURE;
Texture3D GF_PL_SHADOW_MAP_JITTER_TEXTURE;

cbuffer gf_cb_shadow_map_0
{
	float4x4 GF_SHADOW_MAP_TRANSFORM_0;
	float4 	 GF_SHADOW_MAP_SIZE_0;
};
cbuffer gf_cb_shadow_map_1
{
	float4x4 GF_SHADOW_MAP_TRANSFORM_1;
	float4 	 GF_SHADOW_MAP_SIZE_1;
};
cbuffer gf_cb_shadow_map_2
{
	float4x4 GF_SHADOW_MAP_TRANSFORM_2;
	float4 	 GF_SHADOW_MAP_SIZE_2;
};
cbuffer gf_cb_shadow_map_3
{
	float4x4 GF_SHADOW_MAP_TRANSFORM_3;
	float4 	 GF_SHADOW_MAP_SIZE_3;
};
cbuffer gf_cb_shadow_map_4
{
	float4x4 GF_SHADOW_MAP_TRANSFORM_4;
	float4 	 GF_SHADOW_MAP_SIZE_4;
};
cbuffer gf_cb_shadow_map_5
{
	float4x4 GF_SHADOW_MAP_TRANSFORM_5;
	float4 	 GF_SHADOW_MAP_SIZE_5;
};
cbuffer gf_cb_shadow_map_6
{
	float4x4 GF_SHADOW_MAP_TRANSFORM_6;
	float4 	 GF_SHADOW_MAP_SIZE_6;
};
cbuffer gf_cb_shadow_map_7
{
	float4x4 GF_SHADOW_MAP_TRANSFORM_7;
	float4 	 GF_SHADOW_MAP_SIZE_7;
};


/* built-in texture samplers. */
SamplerState GF_WRAP_LINER_SAMPLER;
SamplerState GF_WRAP_POINT_SAMPLER;
SamplerComparisonState GF_SHADOW_MAP_SAMPLER;

/* @lightDir: from object to light.
   @return: if light can reach the object. */
bool ComputeIrradianceOfPointLight(float3 pos, SPointLight light, out float3 lightDir,
	out float4 diffuse, out float4 specular)
{
	lightDir = light.Position - pos;
	float dist = length(lightDir);
	if(dist > light.Range)
		return false;

	lightDir /= dist;
	float distAtten = 1.0 / dot(light.Attenuations, float3(1, dist, dist * dist));

	diffuse = light.Diffuse * distAtten;
	specular = light.Specular * distAtten;

	return true;
}


void PhoneShading(float3 pos, float3 lightDir, float3 normal, 
	inout float4 diffuse, inout float4 specular, float power)
{
	float3 reflectDir = reflect(-lightDir, normal);
	float3 viewDir = normalize(GF_CAMERA_POS - pos);

	float atten = saturate(dot(normal, lightDir));
	float specAtten = saturate(dot(viewDir, reflectDir));

	diffuse = diffuse * atten;
	specular = specular * atten * pow(specAtten, power);
}



float3 NormalSampleToWorldSpace(float3 normalMapSample, float3 unitNormalW, float3 tangentW)
{
	// Uncompress each component from [0,1] to [-1,1].
	float3 normalT = 2.0f * normalMapSample - 1.0f;
	// Build orthonormal basis.
	float3 N = unitNormalW;
	float3 T = normalize(tangentW - dot(tangentW, N) * N);
	float3 B = cross(N, T);
	float3x3 TBN = float3x3(T, B, N);
	// Transform from tangent space to world space.
	float3 bumpedNormalW = mul(normalT, TBN);
	return bumpedNormalW;
}

float4 CalcShadowPosH(float3 posW, float4x4 shadowTransform)
{
	return mul(float4(posW, 1.0f), shadowTransform);
}

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

float CalcShadowFactor_Jitter(Texture2D shadowMap, float4 shadowMapSize,
	float2 screenPos, float4 shadowPosH, float blurRadius)
{
#define JITTER_SAMPLES_COUNT 64
#define JITTER_SAMPLES_COUNT_DIV_2 32
#define INV_JITTER_SAMPLES_COUNT (1.0f / JITTER_SAMPLES_COUNT)
#define INV_JITTER_SAMPLES_COUNT_DIV_2 (1.0f / JITTER_SAMPLES_COUNT_DIV_2)

	// Complete projection by doing division by w.
	shadowPosH.xyz /= shadowPosH.w;

	// Depth in NDC space.
	float depth = shadowPosH.z;

	// Texel size.
	float2 fsize = float2(shadowMapSize.z, shadowMapSize.w) * blurRadius;

	float percentLit = 0.0f;
	float3 jcoord = float3(screenPos, 0);

		[unroll]
	for (int i = 0; i < 4; i++)
	{
		float4 offset = GF_SHADOW_MAP_JITTER_TEXTURE.Sample(GF_WRAP_POINT_SAMPLER, jcoord);

			float2 smcoord = fsize * offset.xy + shadowPosH.xy;
			percentLit += shadowMap.SampleCmpLevelZero(GF_SHADOW_MAP_SAMPLER, smcoord, depth).r;

		smcoord = fsize * offset.zw + shadowPosH.xy;
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

			float2 smcoord = fsize * offset.xy + shadowPosH.xy;
			percentLit += shadowMap.SampleCmpLevelZero(GF_SHADOW_MAP_SAMPLER, smcoord, depth).r;

		smcoord = fsize * offset.zw + shadowPosH.xy;
		percentLit += shadowMap.SampleCmpLevelZero(GF_SHADOW_MAP_SAMPLER, smcoord, depth).r;

		jcoord.z += INV_JITTER_SAMPLES_COUNT_DIV_2;
	}

	return percentLit * INV_JITTER_SAMPLES_COUNT;
}



#ifndef CalcShadowFactor
#define CalcShadowFactor(ShadowLightID, ShadowPosH, BlurRadius) \
	CalcShadowFactor_Jitter(GF_SHADOW_MAP_##ShadowLightID, GF_SHADOW_MAP_SIZE_##ShadowLightID, \
		pin.PosH.xy/pin.PosH.w, ShadowPosH, BlurRadius)
#endif

#ifndef PS_ShadowPosH
#define PS_ShadowPosH(ShadowLightID) \
	mul(float4(pin.PosW.xyz, 1.0f), GF_SHADOW_MAP_TRANSFORM_##ShadowLightID)
#endif

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

#ifndef CalcPointLightShadowFactor
#define CalcPointLightShadowFactor(ShadowLightID, LightPos, BlurRadius) \
	CalcPointLightShadowFactor_Jitter(GF_PL_SHADOW_MAP_##ShadowLightID, pin.PosW, LightPos, \
		pin.PosH.xy/pin.PosH.w, GF_SHADOW_MAP_SIZE_##ShadowLightID, BlurRadius)
#endif
