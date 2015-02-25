
struct VertexIn
{
	float3 PosL		: POSITION;
	float3 Normal	: NORMAL;
	float3 Tangent	: TANGENT;
	float2 Tex		: TEXCOORD;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
};

VertexOut vs_main(VertexIn vin)
{
	VertexOut vout;
	vout.PosH = mul(float4(vin.PosL, 1.0f), GF_WVP);
	return vout;
}

float4 ps_main(VertexOut vout) : SV_TARGET
{
	return GF_MTRL_DIFFUSE;
}
