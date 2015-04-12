TextureCube gCubeTexture;
SamplerState gCubeTextureState;

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float3 PosL : POSITION;
};

SReturnGBuffers defer_ps_main(VertexOut pin)
{
	float4 texColor = gCubeTexture.Sample(gCubeTextureState, pin.PosL);

	SReturnGBuffers pout;
	//pout.GBuffer0 = float4(0, 1.0, 0, 0);
	pout.GBuffer1 = GF_MTRL_DIFFUSE * texColor;
	pout.GBuffer2.x = GF_MTRL_SPECULAR.x;
	pout.GBuffer2.y = GF_MTRL_AMBIENT.x;
	pout.GBuffer2.z = 1.0f;

	return pout;
}

