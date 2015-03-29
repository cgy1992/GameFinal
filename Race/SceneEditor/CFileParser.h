#ifndef __FILE_PARSER_H__
#define __FILE_PARSER_H__

#include "SNodeInfo.h"
#include "SCollectionNodeInfo.h"
#include "SceneFileStructs.h"


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
