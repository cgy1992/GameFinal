//Texture2DMS<float4, 4> gTexture;
Texture2D gTexture;

cbuffer cbPerFrame
{
	float gInverseWindowWidth;
	float gInverseWindowHeight;
};

SamplerState gSamplerState;

static const float4 gSamples[4] = 
{
	{-1.0f, 0.0f, 0.0f, 0.25f},
	{1.0f, 0.0f, 0.0f, 0.25f},
	{0.0f, -1.0f, 0.0f, 0.25f},
	{0.0f, 1.0f, 0.0f, 0.25f}
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float2 Tex : TEXCOORD;
};

VertexOut vs_main(float3 Pos : POSITION)
{
	VertexOut vout;
	vout.PosH = float4(Pos.xy, 0.0f, 1.0f);
	vout.Tex.x = 0.5f * (1.0f + Pos.x);
	vout.Tex.y = 0.5f * (1.0f - Pos.y);
	return vout;
}



float4 ps_main(VertexOut pin) : SV_TARGET
{
//	float4 Color = gTexture.Sample(gSamplerState, pin.Tex);
//	float Intensity = dot(Color, float4(0.299,0.587,0.184,0));
//	Color = float4(Intensity, Intensity, Intensity, 1.0f);
	float4 Color = 0;

	for(int i = 0; i < 4; i++)
	{
		Color += gSamples[i].w * gTexture.Sample(gSamplerState, pin.Tex + 
			float2(gSamples[i].x * gInverseWindowWidth, gSamples[i].y * gInverseWindowHeight));
	}

	return Color;
}



/*
float4 ps_main(VertexOut pin) : SV_TARGET
{
	uint Width, Height, NumberOfSamples;
	gTexture.GetDimensions(Width, Height, NumberOfSamples);

	Width = 1.0f / gInverseWindowWidth;
	Height = 1.0f / gInverseWindowHeight;

	int2 tex = int2(pin.Tex.x * Width, pin.Tex.y * Height);
	float4 Color = computeSamplingColor(tex, NumberOfSamples);

	float Intensity = dot(Color, float4(0.299,0.587,0.184,0));

	//Color = float4(1.0f, 1.0f, 1.0f, 1.0f) - Color;
	//float4 c1 = gTexture.Load(pin.Tex * float2(Width, Height), 0);

	Color = float4(Intensity, Intensity, Intensity, 1.0f);


	return Color;
}
*/