#include "stdafx.h"
#include "CShaderManager.h"
#include "CD3D11Shader.h"
#include "CD3D11VertexShader.h"
#include "CD3D11PixelShader.h"

namespace gf
{

	CShaderManager::CShaderManager(IResourceFactory* pResourceFactory)
		:mResourceFactory(pResourceFactory)
	{

	}


	IShader* CShaderManager::load(E_SHADER_TYPE shaderType,
		const std::string& filename,
		const std::string& functionName,
		const SShaderMacroSet& macros)
	{
		std::string shaderName = makeUpShaderName(filename, functionName);
		auto it = mShadersMap.find(shaderName);
		if (it != mShadersMap.end())
		{
			const std::list<IShader*> shaderList = it->second;
			auto shader = std::find(shaderList.begin(), shaderList.end(), macros);
			if (shader != shaderList.end())
			{
				if ((*shader)->getType() == shaderType)
					return *shader;  
				else
					return nullptr;
			}
		}
		else
		{
			std::list<IShader*> shaderList;
			mShadersMap.insert(std::make_pair(shaderName, shaderList));
		}
		
		it = mShadersMap.find(shaderName);
		u32 sortCode = mCodeAllocator[shaderType].allocate();

		// get shader file's full path
		std::string filepath;
		if (!IResourceGroupManager::getInstance()->getFullPath(filename, filepath, ERFT_SHADER))
		{
			GF_PRINT_CONSOLE_INFO("The shader file named '%s' has not been found.\n", filename.c_str());
			return false;
		}

		IShader* shader = mResourceFactory->createShaderFromFile(shaderType, filepath.c_str(), functionName.c_str(), sortCode, shaderName, macros);
		if (!shader)
		{
			mCodeAllocator[shaderType].release(sortCode);
			return nullptr;
		}

		it->second.push_back(shader);
		return shader;
	}

	IShader* CShaderManager::get(const std::string& name,
		const SShaderMacroSet& macros,
		bool bLoadIfNotExist,
		E_SHADER_TYPE shaderType)
	{
		auto it = mShadersMap.find(name);
		if (it != mShadersMap.end())
		{
			const std::list<IShader*> shaderList = it->second;
			auto shader = std::find(shaderList.begin(), shaderList.end(), macros);
			if (shader != shaderList.end())
			{
				if (shaderType == EST_UNDEFINED_SHADER || (*shader)->getType() == shaderType)
					return *shader;
				else
					return nullptr;
			}
		}

		if (!bLoadIfNotExist || shaderType == EST_UNDEFINED_SHADER)
			return nullptr;

		std::string filename;
		std::string functionName;

		if (!splitShaderName(name, filename, functionName))
			return false;

		return this->load(shaderType, filename, functionName, macros);

	}

	IShader* CShaderManager::get(const std::string& name, bool bLoadIfNotExist, E_SHADER_TYPE shaderType)
	{
		SShaderMacroSet macros;
		return get(name, macros, bLoadIfNotExist, shaderType);
	}


	std::string CShaderManager::makeUpShaderName(const std::string& filename,
		const std::string& functionName) const
	{
		return filename + std::string("|") + functionName;
	}

	bool CShaderManager::splitShaderName(const std::string& shaderName, std::string& filename, std::string& functionName) const
	{
		size_t splitter = shaderName.find_first_of('|');
		if (splitter == std::string::npos)
			return false;

		filename = shaderName.substr(0, splitter);
		functionName = shaderName.substr(splitter + 1);
		return true;
	}


}