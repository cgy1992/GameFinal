struct VertexIn
{
	float3 PosL : POSITION;
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

float4 ps_main(VertexOut pin) : SV_TARGET
{
	return float4(0.7f, 0.7f, 0.7f, 1.0f);
} 

