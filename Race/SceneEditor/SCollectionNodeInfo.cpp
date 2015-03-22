#include "stdafx.h"
#include "SCollectionNodeInfo.h"
#include "EditorScene.h"

SNodeInfo* SCollectionNodeInfo::GetNodeInfo(ISceneNode* node)
{
	auto it = InstanceIdMap.find(node);
	if (it == InstanceIdMap.end())
		return nullptr;

	int id = it->second;
	return &InstanceNodeInfos[id];
}

int SCollectionNodeInfo::AddNodeInfo(SNodeInfo& info)
{
	int id = info.Id;
	info.CollectionId = Id;
	info.Category = INSTANCE_CATEGORY;
	info.Name = "";
	InstanceNodeInfos.insert(std::make_pair(id, info));
	InstanceIdMap.insert(std::make_pair(info.Node, id));
	return id;
}

SNodeInfo* SCollectionNodeInfo::GetNodeInfoById(u32 id)
{
	auto it = InstanceNodeInfos.find(id);
	if (it == InstanceNodeInfos.end())
		return nullptr;

	return &(it->second);
}

std::vector<int> SCollectionNodeInfo::GetAllInstancesIds()
{
	std::vector<int> ids;
	for (auto it = InstanceNodeInfos.begin(); it != InstanceNodeInfos.end(); it++)
	{
		ids.push_back(it->first);
	}
	
	std::sort(ids.begin(), ids.end());
	return ids;
}

void SCollectionNodeInfo::DeleteInstances()
{
	for (auto it = InstanceNodeInfos.begin(); it != InstanceNodeInfos.end(); it++)
	{
		it->second.Node->destroy();
	}
}

void SCollectionNodeInfo::DeleteInstance(u32 id)
{
	auto it = InstanceNodeInfos.find(id);

	if (it == InstanceNodeInfos.end())
		return;

	ISceneNode* node = it->second.Node;
	auto it2 = InstanceIdMap.find(node);

	InstanceNodeInfos.erase(it);
	InstanceIdMap.erase(it2);
	
	node->destroy();
}

