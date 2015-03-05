Texture2D gDepthBuffer;
Texture2D gGBuffer0;
Texture2D gGBuffer1;
Texture2D gGBuffer2;
Texture2D gGBuffer3;
RWTexture2D<float4> gOutputTexture;

cbuffer gf_ds_buffer
{
	uint   gPointLightsNum;
	float4 gTilesNum;
};

StructuredBuffer<SDirectionalLight> 	gDirLights;
StructuredBuffer<SPointLight> 			gPointLights;

groupshared uint uiMinDepth; 
groupshared uint uiMaxDepth;
groupshared uint visiblePointLightsNum;
groupshared uint visiblePointLights[1024];


static inline SFrustum computeFrustum(float4 RegionH, float minDepth, float maxDepth)
{
	float4x4 M = transpose(GF_VIEW_PROJ);	
	SFrustum frustum;
	frustum.Planes[GF_LEFT_PLANE] = M[0] - M[3] * RegionH.x;
	frustum.Planes[GF_RIGHT_PLANE] = M[3] * RegionH.y - M[0];	
	frustum.Planes[GF_TOP_PLANE] = M[3] * RegionH.z - M[1];
	frustum.Planes[GF_BOTTOM_PLANE] = M[1] - M[3] * RegionH.w;
	frustum.Planes[GF_NEAR_PLANE] = M[2] - M[3] * minDepth;
	frustum.Planes[GF_FAR_PLANE] = M[3] * maxDepth - M[2];

	[unroll]
	for(int i = 0; i < 6; i++)
	{
		float len = length(frustum.Planes[i].xyz);
		frustum.Planes[i] /= len;
	}

	return frustum;
}

// return false: sphere clipped.
static inline bool SphereFrustumTest(float3 center, float radius, SFrustum frustum)
{
	//return true;

	//[unroll]
	for(int i = 0; i < 6; i++)
	{
		float dist = dot(float4(center, 1.0f), frustum.Planes[i]);
		if(dist < -radius)
			return false;

		//if(dist < radius)
		//	return true;
	}

	return true;
}

#define BLOCK_SIZE (16)

[numthreads(BLOCK_SIZE, BLOCK_SIZE, 1)]
void cs_main(int3 dispatchThreadID : SV_DispatchThreadID,
	int3 groupThreadID : SV_GroupThreadID,
	uint3 groupID : SV_GroupID,
	int groupIndex: SV_GroupIndex)
{
	// Step 1 : Compute the minimum and maximum depth of this tile.
	// initial minDepth and maxDepth;
	visiblePointLightsNum = 0;
	uiMaxDepth = 0;
	uiMinDepth = 0xFFFFFFFF;
	float depth = gDepthBuffer[dispatchThreadID.xy].x;
	uint depthInt = asuint(depth);

	GroupMemoryBarrierWithGroupSync();

	InterlockedMin(uiMinDepth, depthInt);
	InterlockedMax(uiMaxDepth, depthInt);

	GroupMemoryBarrierWithGroupSync();

	float minDepth = asfloat(uiMinDepth);
	float maxDepth = asfloat(uiMaxDepth);

	// Step 2: Compute the frustum of current tile.
	//float4 RegionH = float4(groupID.x, groupID.x + 1, -groupID.y, -groupID.y-1)
	//				* gTilesNum.zzww * 2.0f + float4(-1.0f, -1.0f, 1.0f, 1.0f);
	
	float left = groupID.x * gTilesNum.z * 2.0f - 1.0f;
	float right = (groupID.x + 1) * gTilesNum.z * 2.0f - 1.0f;
	float top = 1.0f - groupID.y * gTilesNum.w * 2.0f;
	float bottom = 1.0f - (groupID.y + 1) * gTilesNum.w * 2.0f;
	float4 RegionH = float4(left, right, top, bottom);

	SFrustum frustum = computeFrustum(RegionH, minDepth, maxDepth);

	// Step 3: clip the point lights.
	uint threadNumPerGroup = BLOCK_SIZE * BLOCK_SIZE;
	//uint pointLightCountPerThread = (gPointLights + groundThreadNum - 1) / groundThreadNum;

	for(int i = groupIndex; i < gPointLightsNum; i += threadNumPerGroup)
	{
		SPointLight pointLight = gPointLights[i];
		if(SphereFrustumTest(pointLight.Position, pointLight.Range, frustum))
		{
			uint index;
			InterlockedAdd(visiblePointLightsNum, 1, index);
			visiblePointLights[index] = i;	
		}
	}

	GroupMemoryBarrierWithGroupSync();


	// step 4: calculate the actual lighting.
	//  compute the coordinates in clip space.
	float4 posH;
	posH.x = GF_SCREEN_SIZE.z * dispatchThreadID.x * 2.0f - 1.0f;
	posH.y = 1.0f - GF_SCREEN_SIZE.w * dispatchThreadID.y * 2.0f;
	posH.z = gDepthBuffer[dispatchThreadID.xy].x;
	posH.w = 1.0f;
	float4 PosW = mul(posH, GF_INV_VIEW_PROJ);
	PosW = PosW / PosW.w;

	float3 normal = gGBuffer0[dispatchThreadID.xy].xyz * 2.0f - 1.0f;
	float4 diffuseMtrl = gGBuffer1[dispatchThreadID.xy];
	float4 specularMtrl = gGBuffer2[dispatchThreadID.xy];
	float4 ambientMtrl = gGBuffer3[dispatchThreadID.xy];

	
	float4 diffuseSum = float4(0, 0, 0, 0);
	float4 specularSum = float4(0, 0, 0, 0);
	float4 diffuse;
	float4 specular;
	float3 lightDir;
	float4 Color = GF_AMBIENT * ambientMtrl;
	
	for(int i = 0; i < visiblePointLightsNum; i++)
	{
		uint lightIndex = visiblePointLights[i];
		SPointLight pointLight = gPointLights[lightIndex];
		if(ComputeIrradianceOfPointLight(PosW.xyz, pointLight, lightDir, diffuse, specular))
		{
			BlinnPhoneShading(PosW.xyz, lightDir, normal, diffuse, specular, pointLight.Specular.w);
			//PhoneShading(PosW, lightDir, normal, diffuse, specular, pointLight.Specular.w);

			diffuseSum += diffuse;
			specularSum += specular;
		}
	}

	Color = Color + diffuseSum * diffuseMtrl + specularSum * specularMtrl;
	float k = (float)visiblePointLightsNum / 500;
	float4 testColor = float4(k, 0, 0, 0);
	
	gOutputTexture[dispatchThreadID.xy] = Color;
	//gOutputTexture[dispatchThreadID.xy] = testColor;
	//gOutputTexture[dispatchThreadID.xy] = float4(RegionH.ww, 0,0);
	//gOutputTexture[dispatchThreadID.xy] = float4(posH.z, 0, 0, 0);
	//gOutputTexture[dispatchThreadID.xy] = float4(0,0,0,0);
}



SamplerState gSamplerState;

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float2 Tex : TEXCOORD;
};

float4 ps_main(VertexOut pin) : SV_TARGET
{
	return GF_TEXTURE.Sample(gSamplerState, pin.Tex);
}
