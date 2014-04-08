cbuffer cbPerObject
{
	float4x4 gWVP;
};

struct VertexIn
{
	float3 Pos : POSITION;
	float4 Color : COLOR;
	float SizeW : SIZE;
};

struct VertexOut
{
	float3 Pos : POSITION;
	float4 Color : COLOR;
	float SizeW : SIZE;
};

struct GeoOut
{
	float4 PosH : SV_POSITION;
	float4 Color : COLOR;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	vout.Pos = vin.Pos;
	vout.Color = vin.Color;
	vout.SizeW = vin.SizeW;
	return vout;
}


[maxvertexcount(4)]
void GS(point VertexOut gin[1], inout TriangleStream<GeoOut> triStream)
{
	float halfSize = gin[0].SizeW;

	float3 v[4];
	v[0] = gin[0].Pos + float3(-halfSize, halfSize, 0.0f);
	v[1] = gin[0].Pos + float3(halfSize, halfSize, 0.0f);
	v[2] = gin[0].Pos + float3(-halfSize, -halfSize, 0.0f);
	v[3] = gin[0].Pos + float3(halfSize, -halfSize, 0.0f);
	
	GeoOut gout[4];

	[unroll]
	for(int i = 0; i < 4; i++)
	{
		gout[i].PosH = mul(float4(v[i], 1.0f), gWVP);
		gout[i].Color = gin[0].Color;
		triStream.Append(gout[i]);
	}
}


float4 PS(GeoOut pin) : SV_TARGET
{
	return pin.Color;
}
