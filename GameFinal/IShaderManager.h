#ifndef __ISHADERMANAGER_H__
#define __ISHADERMANAGER_H__

#include <map>
#include "IReferenceCounted.h"
#include "IShader.h"
namespace gf
{
	class IShaderManager : public IReferenceCounted
	{
	public:

		virtual IShader* load(E_SHADER_TYPE shaderType,
			const std::string& filename,
			const std::string& functionName,
			const SShaderMacroSet& macros) = 0;

		virtual IShader* get(const std::string& name, 
			bool bLoadIfNotExist = true, 
			E_SHADER_TYPE shaderType = EST_UNDEFINED_SHADER) = 0;

		virtual IShader* get(const std::string& name,
			const SShaderMacroSet& macros,
			bool bLoadIfNotExist = true,
			E_SHADER_TYPE shaderType = EST_UNDEFINED_SHADER) = 0;

		virtual std::string makeUpShaderName(const std::string& filename,
			const std::string& functionName) const = 0;

		virtual bool splitShaderName(const std::string& shaderName, 
			std::string& filename, 
			std::string& functionName) const = 0;

		_DECLARE_SINGLETON_INSTANCE(IShaderManager);

		//_DECLARE_RESOURCE_DESTROY_METHODS(IShader);

	protected:


	};

	inline bool operator==(IShader* shader, const SShaderMacroSet& macros)
	{
		return shader->getMacros() == macros;
	}

}

#endif