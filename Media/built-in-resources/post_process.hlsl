#include "GameFinal.hlsl"

SamplerState gSamplerState;

struct VertexIn
{
	float3 PosL : POSITION;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float2 Tex : TEXCOORD;
};

VertexOut vs_main(VertexIn vin)
{
	VertexOut vout;
	vout.PosH = float4(vin.PosL.xy, 0.0f, 1.0f);
	//vout.Tex.x = vin.PosL.x * 0.5f + 0.5f;
	//vout.Tex.y = 0.5f - vin.PosL.y * 0.5f;


	vout.Tex.x = 0.5 * (1 + vin.PosL.x - GF_SCREEN_SIZE.z);
	vout.Tex.y = 0.5 * (1 - vin.PosL.y - GF_SCREEN_SIZE.w);

	return vout;
}

cbuffer gf_cb_blur
{
	float gBlurDistance;
};

cbuffer gf_cb_bloom
{
	float gThreshold;
};

static const float4 blur4_samples[4] = 
{
	{-1.0f, 0.0f, 0.0f, 0.25f},
	{1.0f, 0.0f, 0.0f, 0.25f},
	{0.0f, -1.0f, 0.0f, 0.25f},
	{0.0f, 1.0f, 0.0f, 0.25f}
};

static const float4 blur8_samples[8] = 
{
	{-1.0f, 0.0f, 0.0f, 0.125f},
	{1.0f, 0.0f, 0.0f, 0.125f},
	{0.0f, -1.0f, 0.0f, 0.125f},
	{0.0f, 1.0f, 0.0f, 0.125f},
	{-1.0f, -1.0f, 0.0f, 0.125f},
	{-1.0f, 1.0f, 0.0f, 0.125f},
	{1.0f, -1.0f, 0.0f, 0.125f},
	{1.0f, 1.0f, 0.0f, 0.125f}
};

static const float2 gauss_blur_samples[7] = 
{
	{-3.0f, 0.015625f}, 
	{-2.0f, 0.09375f},
	{-1.0f, 0.234375f},
	{0.0f, 0.3125f},
	{1.0f, 0.234375f},
	{2.0f, 0.09375f},
	{3.0f, 0.015625f}
};


float4 ps_blur4(VertexOut pin) : SV_TARGET
{
	float4 Color = 0;

	float2 inc = float2(GF_SCREEN_SIZE.z * gBlurDistance, GF_SCREEN_SIZE.w * gBlurDistance);

	[unroll]
	for(int i = 0; i < 4; i++)
	{
		Color += blur4_samples[i].w * GF_TEXTURE_0.Sample(gSamplerState, pin.Tex + 
			float2(blur4_samples[i].x * inc.x, blur4_samples[i].y * inc.y));
	}

	return Color;
}


float4 ps_blur8(VertexOut pin) : SV_TARGET
{
	float4 Color = 0;

	float2 inc = float2(GF_SCREEN_SIZE.z * gBlurDistance, GF_SCREEN_SIZE.w * gBlurDistance);

	[unroll]
	for(int i = 0; i < 8; i++)
	{
		Color += blur8_samples[i].w * GF_TEXTURE_0.Sample(gSamplerState, pin.Tex + 
			float2(blur8_samples[i].x * inc.x, blur8_samples[i].y * inc.y));
	}

	return Color;
}

float4 ps_gauss_vertical_blur(VertexOut pin) : SV_TARGET
{
	float4 Color = 0;
	float inc = GF_SCREEN_SIZE.z * gBlurDistance;

	[unroll]
	for(int i = 0; i < 7; i++)
	{
		Color += gauss_blur_samples[i].y * GF_TEXTURE_0.Sample(gSamplerState,
			pin.Tex + float2(0, gauss_blur_samples[i].x * inc)); 
	}

	return Color;
}


float4 ps_gauss_horizontal_blur(VertexOut pin) : SV_TARGET
{
	float4 Color = 0;
	float inc = GF_SCREEN_SIZE.w * gBlurDistance;

	[unroll]
	for(int i = 0; i < 7; i++)
	{
		Color += gauss_blur_samples[i].y * GF_TEXTURE_0.Sample(gSamplerState,
			pin.Tex + float2(gauss_blur_samples[i].x * inc, 0)); 
	}

	return Color;
}

float4 ps_bloom_retain_high_brightness(VertexOut pin) : SV_TARGET
{
	float4 Color = GF_TEXTURE_0.Sample(gSamplerState, pin.Tex);
	float brightness = dot(Color, float4(0.299,0.587,0.184,0));
	clip(brightness - gThreshold);
	return Color;
}

float4 ps_bloom(VertexOut pin) : SV_TARGET
{
	float4 original = GF_TEXTURE_0.Sample(gSamplerState, pin.Tex);
	float4 highBright = GF_TEXTURE_1.Sample(gSamplerState, pin.Tex);

	//return highBright;
	//return original;
	return original + highBright;
	//return pin.PosH;
}


cbuffer gf_cb_motion_blur
{
	float4x4 gPrevViewProj;
	float    gSampleNum;
};

float4 ps_motion_blur(VertexOut pin) : SV_TARGET
{
	float4 posH;
	posH.xy = pin.Tex * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);
	posH.z = GF_TEXTURE_1.Sample(gSamplerState, pin.Tex).r; // GF_TEXTURE_1 is depth buffer
	posH.w = 1.0f;

	float4 posW = mul(posH, GF_INV_VIEW_PROJ);
	posW = posW / posW.w;

	float4 prePosH = mul(posW, gPrevViewProj);
	prePosH = prePosH / prePosH.w;

	float2 velocity = (posH.xy - prePosH.xy) * 0.5f;

	//return float4(velocity, 0.0f, 1.0f);

	float4 color = 0;
	float2 texcoord = pin.Tex;
	for(int i = 0; i < gSampleNum; i++)
	{
		color += GF_TEXTURE_0.Sample(gSamplerState, texcoord);
		texcoord += velocity;
	}

	color = color / gSampleNum;
	return color;
}

