struct Material
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float4 Emissive;
};

struct Light
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

cbuffer cbPerObject
{
	float4x4 	gWorld;
	Material	gMaterial;
};

cbuffer cbPerFrame
{
	float4x4	gViewProj;
	Light		gLight;
	float3		gEyePos;
	float4   	gFrustumPlanes[6];
};

Texture2D gTexture;
SamplerState gSampleState;

struct VertexIn
{
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
	float2 Tex : TEXCOORD;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float3 Normal : NORMAL;
	float2 Tex  : TEXCOORD;	
};

VertexOut vs_main(VertexIn vin)
{
	VertexOut vout;
	float4 PosW = mul(float4(vin.PosL, 1.0f), gWorld);
	vout.PosH = mul(PosW, gViewProj);
	vout.Normal = mul(vin.NormalL, (float3x3)gWorld);
	vout.Tex = vin.Tex;
	return vout;
} 

float4 ps_main(VertexOut pin) : SV_TARGET
{
	float4 texColor = gTexture.Sample(gSampleState, pin.Tex);

	float4 AmbientColor = gLight.Ambient * gMaterial.Ambient;

	float3 Normal = normalize(pin.Normal);

	float4 DiffuseColor = gLight.Diffuse * gMaterial.Diffuse * dot(Normal, -gLight.Direction);

	//texColor.r = abs(gFrustumPlanes[1].x);

	return texColor * (AmbientColor + DiffuseColor);
}