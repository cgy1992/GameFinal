#ifndef __FILE_PARSER_H__
#define __FILE_PARSER_H__

#include "SNodeInfo.h"
#include "SCollectionNodeInfo.h"

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

class CFileParser
{
public:
	static bool SaveScene(const char* filename);
	static bool ReadScene(const char* filename);

	static SSceneFileHeader ReadFileHeader(FILE* fp);
	static void WriteFileHeader(FILE* fp);

	static void ReadMeshNodes(u32 count, FILE* fp);
	static void WriteMeshNodes(FILE* fp);

	static void ReadCollectionNode(FILE* fp);
	static void WriteCollectionNode(FILE* fp, SCollectionNodeInfo* collectionNodeInfo);

	static void ReadCollections(u32 count, FILE* fp);
	static void WriteCollections(FILE* fp);

	static void ReadLightNodes(u32 count, FILE* fp);
	static void WriteLightNodes(FILE* fp);

	static SSceneFileMeshNodeInfo ConvertNodeInfo(SNodeInfo* info);
	static SSceneFilePointLightInfo ConvertPointLightInfo(SLightNodeInfo* info);

	static void AddMeshNodeToScene(SSceneFileMeshNodeInfo* fileinfo);

};

#endif
