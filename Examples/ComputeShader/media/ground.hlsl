SamplerState gSampleState;

struct VertexIn
{
	float3 PosL		: POSITION;
	float3 Normal	: NORMAL;
	float3 Tangent	: TANGENT;
	float2 Tex		: TEXCOORD;
};

struct VertexOut
{
	float4 PosH		: SV_POSITION;
	float3 PosW		: POSITION;
	float3 Normal	: NORMAL; 
	float3 Tangent  : TANGENT;
	float2 Tex		: TEXCOORD;
};

VertexOut vs_main(VertexIn vin)
{
	VertexOut vout;
	float4 PosW = mul(float4(vin.PosL, 1.0f), GF_WORLD);
	vout.PosH = mul(PosW, GF_VIEW_PROJ);
	vout.PosW = PosW.xyz;
	vout.Normal = mul(vin.Normal, (float3x3)GF_WORLD);
	vout.Tangent = mul(vin.Tangent, (float3x3)GF_WORLD);
	vout.Tex = vin.Tex;
	return vout;
}


float4 ps_main(VertexOut pin) : SV_TARGET
{	
	return GF_TEXTURE.Sample(gSampleState, pin.Tex);
}