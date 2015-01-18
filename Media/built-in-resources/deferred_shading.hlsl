#ifndef __DEFERRED_SHADING_SHADER_HEADER__
#define __DEFERRED_SHADING_SHADER_HEADER__ 

Texture2D gDepthBuffer;
Texture2D gGBuffer0;
Texture2D gGBuffer1;
Texture2D gGBuffer2;
Texture2D gGBuffer3;
SamplerState gSamplerState;

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float2 Tex : TEXCOORD;
};

float4 default_ps_main(VertexOut vout) : SV_TARGET
{
	return gGBuffer0.Sample(gSamplerState, vout.Tex);
	//return float4(1.0f, 0,0,0);
}

#endif
