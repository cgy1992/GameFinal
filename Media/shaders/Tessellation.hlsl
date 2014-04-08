
cbuffer cbPerObject
{
	float4x4 gWVP;
};

cbuffer cbPerFrame
{
	float3 gEyePos;
};

struct VertexIn
{
	float3 PosL : POSITION;
};

struct VertexOut
{
	float3 PosL : POSITION;
};

struct HullOut
{
	float3 PosL : POSITION;
};

struct DomainOut
{
	float4 PosH : SV_POSITION;
	
};

struct PatchTess
{
    float Edges[4]        : SV_TessFactor;
    float Inside[2]       : SV_InsideTessFactor;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	vout.PosL = vin.PosL;
	return vout;
}

PatchTess ConstantHS(InputPatch<VertexOut, 4> patch,
    uint PatchID : SV_PrimitiveID )
{
	PatchTess pt;

	float3 center = 0.25f * (patch[0].PosL + patch[1].PosL + patch[2].PosL + patch[3].PosL);
	float d = distance(center, gEyePos);

	const float d0 = 20.0f;
	const float d1 = 100.0f;

	float tess = 64.0f * saturate((d1 - d) / (d1 - d0));

	pt.Edges[0] = tess;
	pt.Edges[1] = tess;
	pt.Edges[2] = tess;
	pt.Edges[3] = tess;

	pt.Inside[0] = tess;
	pt.Inside[1] = tess;

	return pt;
}


[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("ConstantHS")]
HullOut HS( 
    InputPatch<VertexOut, 4> patch, 
    uint i : SV_OutputControlPointID,
    uint PatchID : SV_PrimitiveID )
{
	HullOut hout;
	hout.PosL = patch[i].PosL;
	return hout;
}

/* 0 - 1
   2 - 3
*/

[domain("quad")]
DomainOut DS( PatchTess patchTess, 
    float2 uv : SV_DomainLocation,
    const OutputPatch<HullOut, 4> quad)
{
    DomainOut dout;
    float3 v1 = lerp(quad[0].PosL, quad[1].PosL, uv.x);
    float3 v2 = lerp(quad[2].PosL, quad[3].PosL, uv.x);

    float3 v = lerp(v1, v2, uv.y);
    v.y = 0.1f * (v.z * sin(v.x) + v.x * cos(v.z));
    dout.PosH = mul(float4(v, 1.0f), gWVP);
    return dout;
}

float4 PS(DomainOut pin) : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}

/*
RasterizerState WireframeRS
{
	FillMode = Wireframe;
	CullMode = Back;
	FrontCounterClockwise = false;
};
*/