#ifndef __SCENE_FILE_STRUCTS_H__
#define __SCENE_FILE_STRUCTS_H__

using namespace gf;

enum E_NODE_CATEGORY
{
	MESH_CATEGORY,
	COLLECTION_CATEGORY,
	INSTANCE_CATEGORY
};


struct SSceneFileHeader
{
	u32						LastSequenceNum;
	XMFLOAT4				Ambient;
	XMFLOAT3				GlobalLightDirection;
	XMFLOAT4				GlobalLightDiffuse;
	XMFLOAT4				GlobalLightSpecular;
	XMFLOAT3				CameraPosition;
	XMFLOAT3				CameraRight;
	XMFLOAT3				CameraUp;
	XMFLOAT3				CameraLook;
	bool					DeferredShading;
	u32						MeshNodeCount;
	u32						CollectionNodeCount;
	u32						LightNodeCount;
	char					TerrainHeightFile[64];
	char					TerrainMaterial[64];
	f32						TerrainVertexSpace;
	f32						TerrainHeightScale;
	int						Reserved[64];
};

struct SSceneFileMeshNodeInfo
{
	int					Id;
	char				Name[64];
	char				MeshName[64];
	bool				Static;
	bool				BoundingPhysics;
	bool				ShadowCasting;
	XMFLOAT3			Position;
	XMFLOAT3			Rotation;
	XMFLOAT3			Scaling;
	E_NODE_CATEGORY		Category;
	u32					Tag;
};

struct SSceneFileInstanceNodeInfo
{
	int					Id;
	bool				Static;
	bool				BoundingPhysics;
	bool				ShadowCasting;
	XMFLOAT3			Position;
	XMFLOAT3			Rotation;
	XMFLOAT3			Scaling;
	u32					Tag;
};

struct SSceneFilePointLightInfo
{
	int					Id;
	char				Name[64];
	bool				Static;
	u32					Tag;
	XMFLOAT3			Position;
	XMFLOAT4			Diffuse;
	XMFLOAT4			Specular;
	f32					Range;
	XMFLOAT3			Attenuations;
};

#endif

