SamplerState gSampleState;

cbuffer cb_per_object
{
	float4 gSourceRect;
	float4 gDestRect;
};

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

	float2 xy = vin.PosL.xy * float2(0.5f, -0.5f) + float2(0.5f, 0.5f);
	vout.Tex.x = gSourceRect.x + xy.x * (gSourceRect.z - gSourceRect.x);
	vout.Tex.y = gSourceRect.y + xy.y * (gSourceRect.w - gSourceRect.y);

	float2 pos;
	pos.x = gDestRect.x + xy.x * (gDestRect.z - gDestRect.x);
	pos.y = gDestRect.y + xy.y * (gDestRect.w - gDestRect.y);

	pos = pos * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);
	vout.PosH = float4(pos, 0.0f, 1.0f);

	return vout;
}

float4 ps_main(VertexOut pin) : SV_TARGET
{
	return GF_TEXTURE.Sample(gSampleState, pin.Tex);
}


