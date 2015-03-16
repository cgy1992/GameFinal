
#ifdef INSTANCES_ON 
#define WORLD_TRANSROM (vin.World)
#else
#define WORLD_TRANSROM (GF_WORLD)
#endif

struct VertexIn
{
	float3 PosL		: POSITION;
	float3 Normal	: NORMAL;
	float3 Tangent	: TANGENT;
	float2 Tex		: TEXCOORD;
#ifdef INSTANCES_ON
	GF_DECLARE_INTANCES_VERTEX
	float4 Color	: COLOR;
#endif
};


struct VertexOut
{
	float4 PosH : SV_POSITION;
#ifdef INSTANCES_ON
	float4 Color    : COLOR;
#endif
};

VertexOut vs_main(VertexIn vin)
{
	VertexOut vout;
	float4 PosW = mul(float4(vin.PosL, 1.0f), WORLD_TRANSROM);
	vout.PosH = mul(PosW, GF_VIEW_PROJ);
#ifdef INSTANCES_ON
	vout.Color = vin.Color;
#endif
	return vout;
}

float4 ps_main(VertexOut pin) : SV_TARGET
{
#ifdef INSTANCES_ON
	return pin.Color;
#else
	return GF_MTRL_DIFFUSE;
#endif
}
