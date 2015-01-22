#ifndef __GF_BILLBOARD_HLSL__
#define __GF_BILLBOARD_HLSL__

SamplerState gSamplerState;

cbuffer cb_billboard
{
	float2 gTexOffset;
};

struct VertexIn
{
	float3 Pos : POSITION;
	float2 Tex : TEXCOORD;
	float2 Size : SIZE;
	float4 Color : COLOR;
};

struct VertexOut
{
	float3 Pos : POSITION;
	float2 Tex : TEXCOORD;
	float2 Size : SIZE;
	float4 Color : COLOR;
};

struct GeoOut
{
	float4 PosH : SV_POSITION;
	float2 Tex : TEXCOORD;
	float4 Color : COLOR;
};

VertexOut default_vs_main(VertexIn vin)
{
	VertexOut vout;
	vout.Pos = mul(float4(vin.Pos, 1.0f), GF_WORLD).xyz;
	vout.Tex = vin.Tex;
	vout.Size = vin.Size;
	vout.Color = vin.Color;
	return vout;
}

[maxvertexcount(4)]
void default_gs_main(point VertexOut gin[1], inout TriangleStream<GeoOut> triStream)
{
	static float2 texOffsets[4] = {
		float2(0, 0), 
		float2(gTexOffset.x, 0),
		float2(0, gTexOffset.y),
		float2(gTexOffset.x, gTexOffset.y)
	};

	float3 up = float3(0, 1.0f, 0);
	float3 toEye = normalize(GF_CAMERA_POS - gin[0].Pos);
	float3 right = normalize(cross(up, toEye));
	up = cross(toEye, right);

	float3 v[4];
	v[0] = gin[0].Pos + gin[0].Size.x * right + gin[0].Size.y * up;
	v[1] = gin[0].Pos - gin[0].Size.x * right + gin[0].Size.y * up;
	v[2] = gin[0].Pos + gin[0].Size.x * right - gin[0].Size.y * up;
	v[3] = gin[0].Pos - gin[0].Size.x * right - gin[0].Size.y * up;

	GeoOut gout[4];
	[unroll]
	for(int i = 0; i < 4; i++)
	{
		gout[i].PosH = mul(float4(v[i], 1.0f), GF_VIEW_PROJ);
		gout[i].Tex = gin[0].Tex + texOffsets[i];
		gout[i].Color = gin[0].Color;
		triStream.Append(gout[i]);
	}
}

float4 default_ps_main(GeoOut pin) : SV_TARGET
{
	return GF_TEXTURE.Sample(gSamplerState, pin.Tex) * pin.Color;
}

#endif
