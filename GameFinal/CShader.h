#ifndef __SHADER_CLASS_H__
#define __SHADER_CLASS_H__

#include "IShader.h"

namespace gf
{
	class CShader : public IShader
	{
	public:

		CShader(u32 sortCode, const std::string& name, const SShaderMacroSet& macros)
			:IShader(sortCode, name, macros)
		{

		}

		virtual void addShaderAutoVariable(const SShaderAutoVariable& var)
		{
			mShaderAutoVariables.push_back(var);
		}

		virtual const std::vector<SShaderAutoVariable>& getShaderAutoVariables() const
		{
			return mShaderAutoVariables;
		}

		virtual void registerAutoSamplers(const std::map<std::string, ISampler*>& samplerMap);
		
		virtual void applySamplers();
	
	private:
		std::vector<SShaderAutoVariable>		mShaderAutoVariables;
		std::vector<SShaderSamplerVariable>		mSamplerVariables;
	};
}

#endif

