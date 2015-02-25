#ifndef __CD3D11SHADERMANAGER_H__
#define __CD3D11SHADERMANAGER_H__

#include "IShaderManager.h"
#include "IResourceFactory.h"
#include "CSortCodeAllocator.h"
namespace gf
{
	const int MAX_SHADER_SORT_CODE = 255;

	class CShaderManager : public IShaderManager
	{
	public:
		CShaderManager(IResourceFactory* pResourceFactory);

		virtual ~CShaderManager();

		virtual IShader* load(E_SHADER_TYPE shaderType,
			const std::string& filename,
			const std::string& functionName,
			const SShaderMacroSet& macros);

		virtual IShader* load(E_SHADER_TYPE shaderType,
			const std::string& filename,
			const std::string& functionName);

		virtual IShader* get(const std::string& name,
			bool bLoadIfNotExist = true,
			E_SHADER_TYPE shaderType = EST_UNDEFINED_SHADER);

		virtual IShader* get(const std::string& name,
			const SShaderMacroSet& macros,
			bool bLoadIfNotExist = true,
			E_SHADER_TYPE shaderType = EST_UNDEFINED_SHADER);

		virtual IShader* get(const std::string& filename,
			const std::string& functionName,
			bool bLoadIfNotExist = true,
			E_SHADER_TYPE shaderType = EST_UNDEFINED_SHADER);

		virtual std::string makeUpShaderName(const std::string& filename,
			const std::string& functionName) const;

		virtual bool splitShaderName(const std::string& shaderName,
			std::string& filename,
			std::string& functionName) const;

		//_DEFINE_RESOURCE_DESTROY_METHODS(CShaderManager, mShadersMap, IShader);


	private:
		IResourceFactory*									mResourceFactory;

		//std::map<std::string, IShader*>					mShadersMap;
		std::map<std::string, std::list<IShader*>>			mShadersMap;

		CSortCodeAllocator<MAX_SHADER_SORT_CODE>			mCodeAllocator[5];

		std::map<std::string, SShaderVariableAttribute>			mBuiltInVariables;
		std::map<std::string, ISampler*>						mBuiltInSamplers;

	};
}

#endif