#ifndef __SCENE_LOADER_H__
#define __SCENE_LOADER_H__

#include "../SceneEditor/SceneFileStructs.h"

class RaceScene;
class GrassLand;

class CSceneLoader
{
public:
	static bool load(const char* filename, RaceScene* scene);
	static void ReadMeshNodes(ISceneManager* smgr, FILE* fp, u32 meshCount);
	static void ReadCollectionNodes(ISceneManager* smgr, FILE* fp, u32 count);
	static void ReadCollectionNode(ISceneManager* smgr, FILE* fp);
	static void ReadLightNodes(ISceneManager* smgr, FILE* fp, u32 count);
	static GrassLand* buildGrassLand(ISceneManager* smgr, ITerrainNode* terrainNode);
	static IInstanceCollectionNode* CreateCollectionNode(ISceneManager* smgr, IModelMesh* mesh, int maxNum);
	static bool BuildLandscape(RaceScene* scene, const SSceneFileHeader& header);
};


#endif
