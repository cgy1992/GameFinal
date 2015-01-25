#ifndef __DEFERRED_SHADING_SHADER_HEADER__
#define __DEFERRED_SHADING_SHADER_HEADER__ 

Texture2D gDepthBuffer;
Texture2D gGBuffer0;
Texture2D gGBuffer1;
Texture2D gGBuffer2;
Texture2D gGBuffer3;
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

cbuffer cb_deferred_shading
{
	float2 m_TileNum;
	float2 m_TileColRow;
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

VertexOut default_vs_main(VertexIn vin)
{
	VertexOut vout;

	float currentCol = m_TileColRow.x;
	float currentRow = m_TileColRow.y;

	float2 tileSpace = float2(2.0f / m_TileNum.x, 2.0f / m_TileNum.y);
	float2 pos = vin.PosL.xy * 0.5f + 0.5f;
	float2 PosH;
	PosH.x = -1.0f + tileSpace.x * (currentCol + pos.x);
	PosH.y = -1.0f + tileSpace.y * (m_TileNum.y - currentRow - 1 + pos.y);
	vout.PosH = float4(PosH, 0, 1.0f);

	vout.Tex.x = 0.5f * (1 + PosH.x - GF_SCREEN_SIZE.z);
	vout.Tex.y = 0.5f * (1 - PosH.y - GF_SCREEN_SIZE.w);

	return vout;
}

float4 default_ps_main(VertexOut vout) : SV_TARGET
{
	return gGBuffer0.Sample(gSamplerState, vout.Tex);
	//return float4(1.0f, 0,0,0);
}

#endif
