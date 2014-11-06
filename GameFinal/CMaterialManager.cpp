#include "stdafx.h"
#include "CMaterialManager.h"
#include "IResourceGroupManager.h"

namespace gf
{
	CMaterialManager::CMaterialManager()
	{

	}


	CMaterialManager::~CMaterialManager()
	{
	}

	bool CMaterialManager::add(IMaterial& material)
	{
		const std::string name = material.getName();
		if (mMaterialsMap.find(name) != mMaterialsMap.end())
		{
			GF_PRINT_CONSOLE_INFO("load the material with the same name (%s).", name.c_str());
			return false;
		}

		u32 sortcode = mCodeAllocator.allocate();
		mMaterialsMap.insert(std::make_pair(name, material));
		return true;
	}

	IMaterial* CMaterialManager::get(const std::string& name, bool bLoadIfNotExist)
	{
		auto it = mMaterialsMap.find(name);
		if (it != mMaterialsMap.end())
			return &(it->second);

		if (bLoadIfNotExist)
		{
			return IResourceGroupManager::getInstance()->loadMaterial(name);
		}

		return nullptr;
	}

	IMaterial* CMaterialManager::create(const std::string& name,
		IPipeline* pipelines[])
	{
		auto it = mMaterialsMap.find(name);
		if (it != mMaterialsMap.end())
		{
			GF_PRINT_CONSOLE_INFO("The material named '%s' has already been created. \
								  							  you can't create the materials with the same name twice.",
															  name.c_str());
			return &it->second;
		}

		u32 sortcode = mCodeAllocator.allocate();
		SMaterial material(name, sortcode, pipelines);

		mMaterialsMap.insert(std::make_pair(name, material));
		it = mMaterialsMap.find(name);
		return &it->second;
	}

	IMaterial* CMaterialManager::create(const std::string& name, IPipeline* pipeline)
	{
		IPipeline* pipelines[EPU_COUNT] = { 0 };
		pipelines[EPU_FORWARD] = pipeline;
		return create(name, pipelines);
	}


	bool CMaterialManager::destroy(const std::string& name)
	{
		auto it = mMaterialsMap.find(name);
		if (it == mMaterialsMap.end())
			return false;

		if (it->second.getReferenceCount() == 1)
		{
			it->second.drop();
			mMaterialsMap.erase(it);
			return true;
		}
		else
			return false;
	}

	bool CMaterialManager::destroy(IMaterial* material)
	{
		const std::string& name = material->getName();
		auto it = mMaterialsMap.find(name);
		if (it == mMaterialsMap.end())
			return false;

		if (it->second.getReferenceCount() == 1)
		{
			it->second.drop();
			mMaterialsMap.erase(it);
			return true;
		}
		else
			return false;
	}
}