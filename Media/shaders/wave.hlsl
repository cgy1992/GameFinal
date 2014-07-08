struct PointLight
{
	float4 Diffuse;
	float4 Specular;
	float4 Ambient;
	float3 Position;
	float  Range;
	float3 Direction;
	float  Falloff;
	float3 Attenuations;
	float  Theta;
};

struct Material
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float4 Emissive;
};

Texture2D gWaterTexture1;
Texture2D gNormalTexture1;
Texture2D gDispTexture1;

Texture2D gWaterTexture2;
Texture2D gNormalTexture2;
Texture2D gDispTexture2;

SamplerState samLinear;

cbuffer cbPerFrame
{
	float4x4 gViewProj;
	float4x4 gWVP;
	PointLight gPointLight;
	float3 gEyePos;
};

cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gTangentWorldMatrix;
	float4x4 gTexTransform1;
	float4x4 gTexTransform2;
	Material gMaterial;
};

static float3 gSurfaceNormal = {0, 1.0f, 0};
	static float gMaxTessDistance = 1.0f;
	static float gMinTessDistance = 200.0f;
	static float gMaxTessFactor = 16;
	static float gMinTessFactor = 1;
	static float gDispScale1 = 1.0f;
	static float gDispScale2 = 1.0f;

/*
cbuffer cbFixed
{
	static float3 gSurfaceNormal = {0, 1.0f, 0};
	static float gMaxTessDistance = 1.0f;
	static float gMinTessDistance = 200.0f;
	static float gMaxTessFactor = 16;
	static float gMinTessFactor = 1;
	static float gDispScale1 = 1.0f;
	static float gDispScale2 = 3.0f;
};
*/

struct VertexIn
{
	float3 PosL : POSITION;
	float2 Tex : TEXCOORD;
};

struct VertexOut
{
//	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float2 Tex1 : TEXCOORD0;
	float2 Tex2 : TEXCOORD1;
	float TessFactor : TESS;
};

struct HullOut
{
	float3 PosW : POSITION;
	float2 Tex1 : TEXCOORD0;
	float2 Tex2 : TEXCOORD1;
};


struct DomainOut
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float2 Tex1 : TEXCOORD0;
	float2 Tex2 : TEXCOORD1;
};

struct PatchTess
{
    float Edges[4]        : SV_TessFactor;
    float Inside[2]       : SV_InsideTessFactor;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	vout.PosW = mul(vin.PosL, (float3x3)gWorld);
	vout.Tex1 = mul(float4(vin.Tex, 0, 1), gTexTransform1).xy;
	vout.Tex2 = mul(float4(vin.Tex, 0, 1), gTexTransform2).xy;

	float d = distance(vout.PosW, gEyePos);
	float t = saturate((gMinTessDistance - d) / (gMinTessDistance - gMaxTessDistance));

	vout.TessFactor = gMinTessFactor + t * (gMaxTessFactor - gMinTessFactor);
	return vout;
}

// Patch Constant Function
PatchTess ConstantsHS( 
    InputPatch<VertexOut, 4> patch,
    uint PatchID : SV_PrimitiveID )
{	
    PatchTess pt;
    pt.Edges[0] = 0.5f * (patch[0].TessFactor + patch[2].TessFactor);
    pt.Edges[1] = 0.5f * (patch[0].TessFactor + patch[1].TessFactor);
    pt.Edges[2] = 0.5f * (patch[1].TessFactor + patch[3].TessFactor);
    pt.Edges[3] = 0.5f * (patch[2].TessFactor + patch[3].TessFactor);

    pt.Inside[0] = patch[0].TessFactor;
    pt.Inside[1] = patch[0].TessFactor;
    
    return pt;
}


[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("ConstantsHS")]
HullOut HS( 
    InputPatch<VertexOut, 4> patch, 
    uint i : SV_OutputControlPointID,
    uint PatchID : SV_PrimitiveID )
{
    HullOut hout;

    hout.PosW = patch[i].PosW;
    hout.Tex1  = patch[i].Tex1;
    hout.Tex2  = patch[i].Tex2;

    return hout;
}

[domain("quad")]
DomainOut DS( PatchTess input, 
    float2 uv : SV_DomainLocation,
    const OutputPatch<HullOut, 4> patch )
{
    DomainOut dout;

    float2 t1 = patch[0].Tex1 * (1.0f - uv.x) + patch[1].Tex1 * uv.x;
   	float2 t2 = patch[2].Tex1 * (1.0f - uv.x) + patch[3].Tex1 * uv.x;
    dout.Tex1 = t1 * (1.0f - uv.y) + t2 * uv.y;

    t1 = patch[0].Tex2 * (1.0f - uv.x) + patch[1].Tex2 * uv.x;
   	t2 = patch[2].Tex2 * (1.0f - uv.x) + patch[3].Tex2 * uv.x;
    dout.Tex2 = t1 * (1.0f - uv.y) + t2 * uv.y;

    float3 p1 = patch[0].PosW * (1.0f - uv.x) + patch[1].PosW * uv.x;
    float3 p2 = patch[2].PosW * (1.0f - uv.x) + patch[3].PosW * uv.x;

    dout.PosW = p1 * (1.0f - uv.y) + p2 * uv.y;
    float h1 = gDispTexture1.SampleLevel(samLinear, dout.Tex1, 0).r;
    float h2 = gDispTexture2.SampleLevel(samLinear, dout.Tex2, 0).r;

	float3 Normal1 = gNormalTexture1.SampleLevel(samLinear, dout.Tex1, 0).xyz;
	float3 Normal2 = gNormalTexture2.SampleLevel(samLinear, dout.Tex2, 0).xyz;
	
	Normal1 = normalize(2 * Normal1 - 1.0f);
	Normal2 = normalize(2 * Normal2 - 1.0f);
	float3 Normal = normalize(mul(Normal1 + Normal2, (float3x3)gTangentWorldMatrix));

    dout.PosW += (h1 * gDispScale1 + h2 * gDispScale2) * Normal;
    dout.PosH = mul(float4(dout.PosW, 1.0f), gViewProj);
   	
    return dout;    
}


float4 PS(DomainOut pin) : SV_TARGET
{
	float4 color = 0.0f;

	color += gPointLight.Ambient * gMaterial.Ambient + gMaterial.Emissive;
	float3 toPointLight = gPointLight.Position - pin.PosW;
	float3 toEye = normalize(gEyePos - pin.PosW);
	float3 Normal1 = gNormalTexture1.Sample(samLinear, pin.Tex1).xyz;
	float3 Normal2 = gNormalTexture2.Sample(samLinear, pin.Tex2).xyz;

	Normal1 = normalize(2 * Normal1 - 1.0f);
	Normal2 = normalize(2 * Normal2 - 1.0f);
	float3 Normal = normalize(mul(Normal1 + Normal2, (float3x3)gTangentWorldMatrix));

	float dist = length(toPointLight);

	float4 SpecColor = 0;
	if(dist < gPointLight.Range)
	{
		toPointLight = toPointLight / dist;

		float diffAtten = dot(toPointLight, Normal);
		if(diffAtten > 0)
		{
			float3 reflectDir = reflect(-toPointLight, Normal);

			float specAtten = saturate(dot(toEye, reflectDir));
			float distAtten = 1.0 / dot(gPointLight.Attenuations, float3(1, dist, dist * dist));

			color += (gPointLight.Diffuse * gMaterial.Diffuse * diffAtten) * distAtten;
			SpecColor = gPointLight.Specular * gMaterial.Specular * pow(specAtten, gMaterial.Specular.w) * distAtten;
		}
	}

	float4 texColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
	if(true)
	{
		float4 c1 = gWaterTexture1.Sample(samLinear, pin.Tex1);
		float4 c2 = gWaterTexture2.Sample(samLinear, pin.Tex2);
		texColor = (c1 + c2) * 0.5f;
	}
	return color * texColor + SpecColor;

	//return gWaterTexture.Sample(samLinear, pin.Tex);
}
