#ifndef __SCENE_LOADER_H__
#define __SCENE_LOADER_H__

#include "../SceneEditor/SceneFileStructs.h"
#include "PhysicsLoader.h"

class RaceScene;
class GrassLand;

struct SPhysicalTerrainLoadingInfo
{
	hkpSampledHeightFieldBaseCinfo			Cinfo;
	ITerrainMesh*							TerrainMesh;
};

struct SPhysicalBoxLoadingInfo
{
	XMFLOAT3				HalfSize;
	XMFLOAT3				Position;
	XMFLOAT4				Rotation;
	bool					Static;
	f32						Mass;
	f32						Friction;
};

struct SPhysicalLoadingInfo
{
	ITerrainMesh*								TerrainMesh;
	std::vector<SPhysicalBoxLoadingInfo>		Boxes;
};

class CSceneLoader
{
public:
	static bool load(const char* filename, RaceScene* scene, SPhysicalLoadingInfo& physicalInfos);
	static void ReadMeshNodes(ISceneManager* smgr, FILE* fp, u32 meshCount, SPhysicalLoadingInfo& physicalInfo);
	static void ReadCollectionNodes(ISceneManager* smgr, FILE* fp, u32 count, SPhysicalLoadingInfo& physicalInfo);
	static void ReadCollectionNode(ISceneManager* smgr, FILE* fp, SPhysicalLoadingInfo& physicalInfo);
	static void ReadLightNodes(ISceneManager* smgr, FILE* fp, u32 count);
	static GrassLand* buildGrassLand(ISceneManager* smgr, ITerrainNode* terrainNode);
	static IInstanceCollectionNode* CreateCollectionNode(ISceneManager* smgr, IModelMesh* mesh, int maxNum);
	static bool BuildLandscape(RaceScene* scene, const SSceneFileHeader& header, SPhysicalLoadingInfo& physicalInfos);

	static void AddPhysicalBoundings(const std::string& meshName, bool bStatic,
		const XMFLOAT3& position, const XMFLOAT3& rotation, const XMFLOAT3& scaling, 
		SPhysicalLoadingInfo& physicalInfo,
		ISceneManager* smgr);
	
	static void AddBoxPhysicalBounding(const SPhysicalBounding* bounding, bool bStatic,
		const XMFLOAT3& position, const XMFLOAT3& rotation, const XMFLOAT3& scaling,
		SPhysicalLoadingInfo& physicalInfo,
		ISceneManager* smgr);

	static void LoadPhysics(SPhysicalLoadingInfo& physicalInfo);
};


#endif
