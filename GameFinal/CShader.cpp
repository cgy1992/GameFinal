#include "stdafx.h"
#include "CShader.h"

namespace gf
{
	void CShader::registerAutoSamplers(const std::map<std::string, ISampler*>& samplerMap)
	{
		for (auto it = samplerMap.begin(); it != samplerMap.end(); it++)
		{
			std::string varname = it->first;
			ISampler* sampler = it->second;

			if (existSampler(varname))
			{
				SShaderSamplerVariable var;
				var.VariableName = varname;
				var.ShaderTypes = getType();
				var.Sampler = sampler;
				mSamplerVariables.push_back(var);
			}
		}
	}

	void CShader::applySamplers()
	{
		for (u32 i = 0; i < mSamplerVariables.size(); i++)
		{
			const SShaderSamplerVariable& var = mSamplerVariables[i];
			setSampler(var.VariableName, var.Sampler);
		}
	}

}
