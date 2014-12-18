SamplerState gSampleState;
SamplerState gRandomValueSampler;
SamplerState gHeightMapSampler;
Texture1D gRadiansTexture;
Texture2D gHeightMapTexture;

cbuffer cb_grass
{
	float gGrassWidth;
	float gGrassHeight;
	float gBladeFluctuation;
	float gTerrainWidth;
	float gInvTerrainWidth;
	float3 gWindDirection;
	SDirectionalLight gLight;
};

struct VertexIn
{
	float3 PosL : POSITION;
};

struct VertexOut
{
	float3 PosL : POSITION;
};

struct GeoOut
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float2 Tex  : TEXCOORD;

#ifndef SHADOW_MAP_PASS
	float3 Normal : NORMAL;
#endif

};

VertexOut vs_main(VertexIn vin)
{
	VertexOut vout;
	vout.PosL = vin.PosL;
	return vout;
}

[maxvertexcount(4)]
void gs_main(point VertexOut gin[1], 
	in uint id : SV_PrimitiveID,
	inout TriangleStream<GeoOut> triStream)
{
	float2 sin_cos_value = gRadiansTexture.SampleLevel(gRandomValueSampler, id / 256.0f, 0).xy;
	float3 extends = float3(gGrassWidth * 0.5f * sin_cos_value.x, gGrassHeight, gGrassWidth * 0.5f * sin_cos_value.y);
	float3 PosW = mul(float4(gin[0].PosL, 1.0f), GF_WORLD).xyz;

	float2 uv;
	uv.x = (PosW.x + gTerrainWidth * 0.5f) * gInvTerrainWidth;
	uv.y = (gTerrainWidth * 0.5f - PosW.z) * gInvTerrainWidth;

	float4 height = gHeightMapTexture.SampleLevel(gHeightMapSampler, uv, 0);
	PosW.y = height;

	float3 v[4];
	GeoOut gout[4];
	float shift = gWindDirection * gBladeFluctuation;

	v[0] = PosW + float3(-extends.x, extends.y, -extends.z) + shift;
	v[1] = PosW + float3(extends.x, extends.y, extends.z) + shift;
	v[2] = PosW + float3(-extends.x, 0, -extends.z);
	v[3] = PosW + float3(extends.x, 0, extends.z);

	gout[0].Tex = float2(0, 0.0f);
	gout[1].Tex = float2(1.0f, 0.0f);
	gout[2].Tex = float2(0, 1.0f);
	gout[3].Tex = float2(1.0f, 1.0f);

#ifndef SHADOW_MAP_PASS
	float3 normal = normalize(v[0] - v[2]);
	gout[0].Normal = normal;
	gout[2].Normal = normal;

	normal = normalize(v[1] - v[3]);
	gout[1].Normal = normal;
	gout[3].Normal = normal;
#endif

	[unroll]
	for(int i = 0; i < 4; i++)
	{
		gout[i].PosW = v[i];
		gout[i].PosH = mul(float4(v[i], 1.0f), GF_VIEW_PROJ);
		triStream.Append(gout[i]);
	}
}

float4 ps_main(GeoOut pin) : SV_TARGET
{
	float4 texColor = GF_TEXTURE.Sample(gSampleState, pin.Tex);
	clip(texColor.a - 0.3f);


#ifndef SHADOW_MAP_PASS
	float3 normal = normalize(pin.Normal);

	//float3 normal = float3(1, 0, 0);
	float4 diffuse = gLight.Diffuse;
	float4 specular = gLight.Specular;

	PhoneShading(pin.PosW, -gLight.Direction.xyz, normal,
		diffuse, specular, gLight.Specular.w);

#ifdef SHADOW_ON
	float shadowFactor = CalcShadowFactor(1, 1.0f);
	//float shadowFactor = 1.0f;
	return GF_AMBIENT * GF_MTRL_AMBIENT * texColor + 
		diffuse * texColor * GF_MTRL_DIFFUSE * shadowFactor + specular * GF_MTRL_SPECULAR * shadowFactor;
#else
	return GF_AMBIENT * GF_MTRL_AMBIENT * texColor + 
		diffuse * texColor * GF_MTRL_DIFFUSE + specular * GF_MTRL_SPECULAR;
#endif

#else
	return texColor;
#endif 

}

void shadow_ps_main(GeoOut pin)
{
	float4 texColor = GF_TEXTURE.Sample(gSampleState, pin.Tex);
	clip(texColor.a - 0.8f);
}

