#ifndef __COLLECTION_NODE_INFO_STRUCT_H__
#define __COLLECTION_NODE_INFO_STRUCT_H__

#include "SNodeInfo.h"

struct SCollectionNodeInfo
{
public:
	int								Id;
	std::string						MeshName;
	IInstanceCollectionNode*		CollectionNode;
	std::map<int, SNodeInfo>		InstanceNodeInfos;
	std::map<ISceneNode*, int>		InstanceIdMap;

	int AddNodeInfo(SNodeInfo& info);
	SNodeInfo* GetNodeInfo(ISceneNode* node);
	SNodeInfo* GetNodeInfoById(u32 id);
	std::vector<int> GetAllInstancesIds();
	void DeleteInstances();
	void DeleteInstance(u32 id);
};

#endif