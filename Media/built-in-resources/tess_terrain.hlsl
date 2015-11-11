cbuffer cbPerFrame
{
	float4 gFrustumPlanes[6];
	SDirectionalLight gLight;
};

cbuffer cbFixed
{
	float gHeightScale;
	float gTexelCellSpaceU;
	float gTexelCellSpaceV;
	float gWorldCellSpace;
//	float gTexScale;
//	float gMaxTessDistance;
//	float gMinTessDistance;
//	float gMaxTessFactor;
//	float gMinTessFactor;
};

static float gMaxTessDistance = 100.0f;
static float gMinTessDistance = 500.0f;
static float gMaxTessFactor = 6;
static float gMinTessFactor = 0;

Texture2D gHeightTexture;
Texture2D gTexture;
SamplerState gHeightTextureSampleState;
SamplerState gSampleState;


struct VertexIn
{
	float3 PosL : POSITION;
	float2 Tex : TEXCOORD;
	float2 BoundsY : BOUNDS;
};

struct VertexOut
{
	float3 PosW : POSITION;
	float2 Tex : TEXCOORD;
	float2 BoundsY : BOUNDS;
};

struct HullOut
{
	float3 PosW : POSITION;
	float2 Tex : TEXCOORD;
};

struct PatchTess
{
    float Edges[4]        : SV_TessFactor;
    float Inside[2]       : SV_InsideTessFactor;
};

struct DomainOut
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float2 Tex : TEXCOORD;
	float3 Normal : NORMAL;
};

bool AabbBehindPlaneTest(float3 center, float3 extends, float4 plane)
{
	float3 n = abs(plane.xyz);
	float r = dot(extends, n);
	float s = dot(float4(center, 1.0f), plane);
	return (r + s) < 0.0f;
}

bool AabbOutsideFrustumTest(float3 center, float3 extends)
{
	for(int i = 0; i < 6; i++)
	{
		if(AabbBehindPlaneTest(center, extends, gFrustumPlanes[i]))
		{
			return true;
		}
	}
	return false;
}

VertexOut vs_main(VertexIn vin)
{
	VertexOut vout;
	vout.PosW = mul(vin.PosL, (float3x3)GF_WORLD);
	vout.Tex = vin.Tex;
	vout.BoundsY = vin.BoundsY;
	return vout;
}

float calcPatchTess(float3 Pos)
{
	float dist = distance(Pos, GF_CAMERA_POS);
	float t = saturate((gMinTessDistance - dist) / (gMinTessDistance - gMaxTessDistance));
	return pow(2, round(lerp(gMinTessFactor, gMaxTessFactor, t)));
}

PatchTess ConstantsHS( 
    InputPatch<VertexOut, 4> patch,
    uint PatchID : SV_PrimitiveID )
{	
    PatchTess pt;

    float3 vMax = float3(patch[1].PosW.x, patch[0].BoundsY.y, patch[1].PosW.z);
    float3 vMin = float3(patch[2].PosW.x, patch[0].BoundsY.x, patch[2].PosW.z);

    float3 center = 0.5f * (vMin + vMax);
    float3 extends = 0.5f * (vMax - vMin);

    if(AabbOutsideFrustumTest(center, extends))
    {
    	pt.Edges[0] = 0;
    	pt.Edges[1] = 0;
    	pt.Edges[2] = 0;
    	pt.Edges[3] = 0;
    	pt.Inside[0] = 0;
    	pt.Inside[1] = 0;
    }
    else 
    {
    	float3 v[4];
	    v[0] = 0.5f * (patch[0].PosW + patch[2].PosW);
	    v[1] = 0.5f * (patch[0].PosW + patch[1].PosW);
	    v[2] = 0.5f * (patch[1].PosW + patch[3].PosW);
	    v[3] = 0.5f * (patch[2].PosW + patch[3].PosW);

	    pt.Edges[0] = calcPatchTess(v[0]);
	    pt.Edges[1] = calcPatchTess(v[1]);
	    pt.Edges[2] = calcPatchTess(v[2]);
	    pt.Edges[3] = calcPatchTess(v[3]);

	    float3 c = 0.25f * (patch[0].PosW + patch[1].PosW + patch[2].PosW + patch[3].PosW);

	    pt.Inside[0] = calcPatchTess(c);
	    pt.Inside[1] = pt.Inside[0];
    }

    return pt;
}

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("ConstantsHS")]
HullOut hs_main( 
    InputPatch<VertexOut, 4> patch, 
    uint i : SV_OutputControlPointID,
    uint PatchID : SV_PrimitiveID )
{
    HullOut hout;

    hout.PosW = patch[i].PosW;
    hout.Tex  = patch[i].Tex;

    return hout;
}

[domain("quad")]
DomainOut ds_main( PatchTess input, 
    float2 uv : SV_DomainLocation,
    const OutputPatch<HullOut, 4> patch )
{
    DomainOut dout;

    dout.PosW = lerp(
    	lerp(patch[0].PosW, patch[1].PosW, uv.x),
    	lerp(patch[2].PosW, patch[3].PosW, uv.x),
    	uv.y
    );

    dout.Tex = lerp(
		lerp(patch[0].Tex, patch[1].Tex, uv.x),
    	lerp(patch[2].Tex, patch[3].Tex, uv.x),
    	uv.y
    );

    dout.PosW.y = gHeightTexture.SampleLevel(gHeightTextureSampleState, dout.Tex, 0).x;
    dout.PosH = mul(float4(dout.PosW, 1.0f), GF_VIEW_PROJ);

    float2 leftTex  = dout.Tex + float2(-gTexelCellSpaceU, 0.0f);
    float2 rightTex = dout.Tex + float2(gTexelCellSpaceU, 0.0f);
    float2 topTex   = dout.Tex + float2(0.0f, -gTexelCellSpaceV);
    float2 bottomTex = dout.Tex + float2(0.0f, gTexelCellSpaceV);

    float leftY =  gHeightTexture.SampleLevel(gHeightTextureSampleState, leftTex, 0).x;
	float rightY =  gHeightTexture.SampleLevel(gHeightTextureSampleState, rightTex, 0).x;
	float topY =  gHeightTexture.SampleLevel(gHeightTextureSampleState, topTex, 0).x;
	float bottomY = gHeightTexture.SampleLevel(gHeightTextureSampleState, bottomTex, 0).x;

	float3 tangent = normalize(float3(gWorldCellSpace * 2.0f, rightY - leftY, 0.0f));
	float3 bitan   = normalize(float3(0, bottomY - topY, -gWorldCellSpace * 2.0f));

	dout.Normal = cross(tangent, bitan);

    return dout;
}

float4 ps_main(DomainOut pin) : SV_TARGET
{
	float3 Normal = normalize(pin.Normal);
	float NdotL = saturate(dot(Normal, -gLight.Direction));
	float4 texColor = gTexture.Sample(gSampleState, pin.Tex * 50);

#ifdef SHADOW_ON
	float shadowFactor = CalcShadowFactor(1, GF_SHADOW_SOFTNESS);
	float4 lightColor = GF_AMBIENT * GF_MTRL_AMBIENT  + NdotL * GF_MTRL_DIFFUSE * shadowFactor;
#else
	float4 lightColor = GF_AMBIENT * GF_MTRL_AMBIENT  + NdotL * GF_MTRL_DIFFUSE;
#endif
	return texColor * lightColor;
}
