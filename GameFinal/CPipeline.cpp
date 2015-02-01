#include "stdafx.h"
#include "CPipeline.h"
namespace gf
{
	CPipeline::CPipeline(const std::string& name,
		IShader** shaders,
		u32 shaderCount,
		IInputLayout* inputLayout,
		E_PRIMITIVE_TYPE primitiveType)
		:mName(name)
		, mPrimitiveType(primitiveType)
		, mInputLayout(inputLayout)
	{
		for (u32 i = 0; i < 5; i++)
			mShaders[i] = nullptr;


		for (u32 i = 0; i < shaderCount; i++)
		{
			if (!shaders[i])
				continue;

			E_SHADER_TYPE type = shaders[i]->getType();
			if (mShaders[type] != nullptr)
			{
				throw std::runtime_error("can not put the same type shader into one pipeline");
			}
			mShaders[type] = shaders[i];
			AddReferenceCounted(mShaders[type]);
		}

		if (!mInputLayout)
		{
			GF_PRINT_CONSOLE_INFO("ERROR: The input-layout is null in pipeline('%s')\n", name.c_str());
			throw std::runtime_error("The input-layout is null");
		}

		AddReferenceCounted(mInputLayout);
	}

	CPipeline::~CPipeline()
	{
		for (u32 i = 0; i < 5; i++)
		{
			ReleaseReferenceCounted(mShaders[i]);
		}

		ReleaseReferenceCounted(mInputLayout);
	}

	u32 CPipeline::setMatrix(const std::string& varname, const f32* m, bool ignoreIfAlreadyUpdated)
	{
		u32 result = 0;
		for (u32 i = 0; i < 5; i++)
		{
			if (mShaders[i] != nullptr && mShaders[i]->setMatrix(varname, m, ignoreIfAlreadyUpdated))
				result++;
		}
		return result;
	}

	bool CPipeline::setMatrix(E_SHADER_TYPE shaderType, const std::string& varname, const f32* m, bool ignoreIfAlreadyUpdated)
	{
		return mShaders[shaderType] != nullptr && mShaders[shaderType]->setMatrix(varname, m, ignoreIfAlreadyUpdated);
	}


	u32 CPipeline::setMatrix(const std::string& varname, CXMMATRIX matrix, bool ignoreIfAlreadyUpdated)
	{
		u32 result = 0;
		for (u32 i = 0; i < 5; i++)
		{
			if (mShaders[i] != nullptr && mShaders[i]->setMatrix(varname, matrix, ignoreIfAlreadyUpdated))
				result++;
		}
		return result;
	}

	u32 CPipeline::setMatrix(E_SHADER_TYPE shaderType, const std::string& varname, CXMMATRIX matrix, bool ignoreIfAlreadyUpdated)
	{
		return mShaders[shaderType] != nullptr && mShaders[shaderType]->setMatrix(varname, matrix, ignoreIfAlreadyUpdated);
	}

	u32 CPipeline::setMatrix(const std::string& varname, const XMFLOAT4X4& matrix, bool ignoreIfAlreadyUpdated)
	{
		u32 result = 0;
		for (u32 i = 0; i < 5; i++)
		{
			if (mShaders[i] != nullptr && mShaders[i]->setMatrix(varname, matrix, ignoreIfAlreadyUpdated))
				result++;
		}
		return result;
	}

	u32 CPipeline::setMatrix(E_SHADER_TYPE shaderType, const std::string& varname, const XMFLOAT4X4& matrix, bool ignoreIfAlreadyUpdated)
	{
		return mShaders[shaderType] != nullptr && mShaders[shaderType]->setMatrix(varname, matrix, ignoreIfAlreadyUpdated);
	}

	bool CPipeline::setTransposedMatrixArray(E_SHADER_TYPE shaderType, const std::string& varname, const f32* matrixs, u32 count, bool ignoreIfAlreadyUpdated)
	{
		return mShaders[shaderType] && mShaders[shaderType]->setTransposedMatrixArray(varname, matrixs, count, ignoreIfAlreadyUpdated);
	}

	bool CPipeline::setMatrixArray(E_SHADER_TYPE shaderType, const std::string& varname, XMFLOAT4X4 matrixs[], u32 count, bool ignoreIfAlreadyUpdate)
	{
		return mShaders[shaderType] && mShaders[shaderType]->setMatrixArray(varname, matrixs, count, ignoreIfAlreadyUpdate);
	}

	u32 CPipeline::setVector(const std::string& varname, const f32* v, bool ignoreIfAlreadyUpdated)
	{
		u32 result = 0;
		for (u32 i = 0; i < 5; i++)
		{
			if (mShaders[i] != nullptr && mShaders[i]->setVector(varname, v, ignoreIfAlreadyUpdated))
				result++;
		}
		return result;
	}

	bool CPipeline::setVector(E_SHADER_TYPE shaderType, const std::string& varname, const f32* v, bool ignoreIfAlreadyUpdated)
	{
		return mShaders[shaderType] != nullptr && mShaders[shaderType]->setVector(varname, v, ignoreIfAlreadyUpdated);
	}

	u32 CPipeline::setVector(const std::string& varname, const XMFLOAT4& val, bool ignoreIfAlreadyUpdated /*= false*/)
	{
		u32 result = 0;
		for (u32 i = 0; i < 5; i++)
		{
			if (mShaders[i] != nullptr && mShaders[i]->setVector(varname, val, ignoreIfAlreadyUpdated))
				result++;
		}
		return result;
	}

	bool CPipeline::setVector(E_SHADER_TYPE shaderType, const std::string& varname, const XMFLOAT4& val, bool ignoreIfAlreadyUpdated /*= false*/)
	{
		return mShaders[shaderType] != nullptr && mShaders[shaderType]->setVector(varname, val, ignoreIfAlreadyUpdated);
	}

	u32 CPipeline::setAttribute(const std::string& varname, const XMFLOAT4& val, bool ignoreIfAlreadyUpdated)
	{
		u32 result = 0;
		for (u32 i = 0; i < 5; i++)
		{
			if (mShaders[i] != nullptr && mShaders[i]->setAttribute(varname, val, ignoreIfAlreadyUpdated))
				result++;
		}
		return result;
	}

	bool CPipeline::setAttribute(E_SHADER_TYPE shaderType, const std::string& varname, const XMFLOAT4& val, bool ignoreIfAlreadyUpdated)
	{
		return mShaders[shaderType] != nullptr && mShaders[shaderType]->setAttribute(varname, val, ignoreIfAlreadyUpdated);
	}

	u32 CPipeline::setFloat(const std::string& varname, f32 v, bool ignoreIfAlreadyUpdated)
	{
		u32 result = 0;
		for (u32 i = 0; i < 5; i++)
		{
			if (mShaders[i] != nullptr && mShaders[i]->setFloat(varname, v, ignoreIfAlreadyUpdated))
				result++;
		}
		return result;
	}

	bool CPipeline::setFloat(E_SHADER_TYPE shaderType, const std::string& varname, f32 value, bool ignoreIfAlreadyUpdated)
	{
		return mShaders[shaderType] != nullptr && mShaders[shaderType]->setFloat(varname, value, ignoreIfAlreadyUpdated);
	}

	u32 CPipeline::setUint(const std::string& varname, u32 v, bool ignoreIfAlreadyUpdated)
	{
		u32 result = 0;
		for (u32 i = 0; i < 5; i++)
		{
			if (mShaders[i] != nullptr && mShaders[i]->setUint(varname, v, ignoreIfAlreadyUpdated))
				result++;
		}
		return result;
	}

	bool CPipeline::setUint(E_SHADER_TYPE shaderType, const std::string& varname, u32 v, bool ignoreIfAlreadyUpdated)
	{
		return mShaders[shaderType] != nullptr && mShaders[shaderType]->setUint(varname, v, ignoreIfAlreadyUpdated);
	}

	u32 CPipeline::setRawValue(const std::string& varname, void* raw, u32 size, bool ignoreIfAlreadyUpdated)
	{
		u32 result = 0;
		for (u32 i = 0; i < 5; i++)
		{
			if (mShaders[i] != nullptr && mShaders[i]->setRawData(varname, raw, size, ignoreIfAlreadyUpdated))
				result++;
		}
		return result;
	}

	bool CPipeline::setRawValue(E_SHADER_TYPE shaderType, const std::string& varname, void* raw, u32 size, bool ignoreIfAlreadyUpdated)
	{
		return mShaders[shaderType] != nullptr && mShaders[shaderType]->setRawData(varname, raw, size, ignoreIfAlreadyUpdated);
	}

	u32 CPipeline::setArray(const std::string& varname, void* data, u32 arraySize, u32 elementSize, bool ignoreIfAlreadyUpdate)
	{
		u32 result = 0;
		for (u32 i = 0; i < 5; i++)
		{
			if (mShaders[i] != nullptr && mShaders[i]->setArray(varname, data, arraySize, elementSize, ignoreIfAlreadyUpdate))
				result++;
		}
		return result;
	}

	u32 CPipeline::setArray(E_SHADER_TYPE shaderType, const std::string& varname, void* data, u32 arraySize, u32 elementSize, bool ignoreIfAlreadyUpdate)
	{
		if (mShaders[shaderType])
		{
			return mShaders[shaderType]->setArray(varname, data, arraySize, elementSize, ignoreIfAlreadyUpdate);
		}
		return 0;
	}

	u32 CPipeline::setTexture(const std::string& varname, ITexture* texture)
	{
		u32 result = 0;
		for (u32 i = 0; i < 5; i++)
		{
			if (mShaders[i] != nullptr && mShaders[i]->setTexture(varname, texture))
				result++;
		}
		return result;
	}

	bool CPipeline::setTexture(E_SHADER_TYPE shaderType, const std::string& varname, ITexture* texture)
	{
		return mShaders[shaderType] != nullptr && mShaders[shaderType]->setTexture(varname, texture);
	}


	void CPipeline::addShaderAutoVariable(const SShaderAutoVariable& var)
	{
		mShaderAutoVariables.push_back(var);
	}

	const std::vector<SShaderAutoVariable>& CPipeline::getShaderAutoVariables() const
	{
		return mShaderAutoVariables;
	}

	bool CPipeline::setSampler(const std::string& varname, ISampler* sampler)
	{
		if (!sampler)
			return false;

		u32 shaderType = 0;

		/* test if already have the sampler with the same name. */
		for (u32 i = 0; i < mSamplerVariables.size(); i++)
		{
			if (mSamplerVariables[i].VariableName == varname)
				return false;
		}

		/* test which shaders exist the sampler. */
		for (u32 i = 0; i < EST_SHADER_COUNT; i++)
		{
			if (mShaders[i] && mShaders[i]->existSampler(varname))
				shaderType |= (1 << i);
		}

		/* if no shader contains this sampler. */
		if (shaderType == 0)
			return false;

		SShaderSamplerVariable var;
		var.VariableName = varname;
		var.ShaderTypes = shaderType;
		var.Sampler = sampler;
		mSamplerVariables.push_back(var);

		return true;
	}

	void CPipeline::applySamplers() const
	{
		for (u32 i = 0; i < mSamplerVariables.size(); i++)
		{
			const SShaderSamplerVariable& var = mSamplerVariables[i];
			// foreach shader

			for (u32 shaderType = 0; shaderType < EST_SHADER_COUNT; shaderType++)
			{
				if (var.ShaderTypes & (1 << shaderType) && mShaders[shaderType])
				{
					mShaders[shaderType]->setSampler(var.VariableName, var.Sampler);
				}
			}
		}

		// apply samplers in shader.
		for (u32 i = 0; i < EST_SHADER_COUNT; i++)
		{
			IShader* shader = mShaders[i];
			if (shader)
			{
				shader->applySamplers();
			}
		}
	}

	/*
	void CPipeline::registerAutoSamplers(const std::map<std::string, ISampler*>& samplerMap)
	{
		for (auto it = samplerMap.begin(); it != samplerMap.end(); it++)
		{
			setSampler(it->first, it->second);
		}
	}
	*/
}

