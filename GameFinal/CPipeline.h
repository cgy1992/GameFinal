#ifndef __CPIPELINE_H__
#define __CPIPELINE_H__

#include "GameFinal.h"
namespace gf
{
	class CPipeline : public IPipeline
	{
	public:
		CPipeline(const std::string& name, IShader** shaders, u32 shaderCount, IInputLayout* inputLayout, E_PRIMITIVE_TYPE primitiveType);

		virtual u32 setMatrix(const std::string& varname, const f32* m, bool ignoreIfAlreadyUpdated = false);

		virtual bool setMatrix(E_SHADER_TYPE shaderType, const std::string& varname, const f32* m, bool ignoreIfAlreadyUpdated = false);


		virtual u32 setMatrix(const std::string& varname, CXMMATRIX matrix, bool ignoreIfAlreadyUpdated = false);

		virtual u32 setMatrix(E_SHADER_TYPE shaderType, const std::string& varname, CXMMATRIX matrix, bool ignoreIfAlreadyUpdated = false);

		virtual u32 setMatrix(const std::string& varname, const XMFLOAT4X4& matrix, bool ignoreIfAlreadyUpdated = false);

		virtual u32 setMatrix(E_SHADER_TYPE shaderType, const std::string& varname, const XMFLOAT4X4& matrix, bool ignoreIfAlreadyUpdated = false);

		virtual bool setTransposedMatrixArray(E_SHADER_TYPE shaderType, const std::string& varname, const f32* matrixs, u32 count, bool ignoreIfAlreadyUpdated = false);

		virtual bool setMatrixArray(E_SHADER_TYPE shaderType, const std::string& varname, XMFLOAT4X4 matrixs[], u32 count, bool ignoreIfAlreadyUpdate = false);

		virtual u32 setAttribute(const std::string& varname, const XMFLOAT4& val, bool ignoreIfAlreadyUpdated = false);

		virtual bool setAttribute(E_SHADER_TYPE shaderType, const std::string& varname, const XMFLOAT4& val, bool ignoreIfAlreadyUpdated = false);

		virtual u32 setVector(const std::string& varname, const f32* v, bool ignoreIfAlreadyUpdated = false);

		virtual bool setVector(E_SHADER_TYPE shaderType, const std::string& varname, const f32* v, bool ignoreIfAlreadyUpdated = false);

		virtual u32 setVector(const std::string& varname, const XMFLOAT4& val, bool ignoreIfAlreadyUpdated = false);

		virtual bool setVector(E_SHADER_TYPE shaderType, const std::string& varname, const XMFLOAT4& val, bool ignoreIfAlreadyUpdated = false);

		virtual u32 setUint(const std::string& varname, u32 v, bool ignoreIfAlreadyUpdated = false);

		virtual bool setUint(E_SHADER_TYPE shaderType, const std::string& varname, u32 v, bool ignoreIfAlreadyUpdated = false);

		virtual u32 setFloat(const std::string& varname, f32 v, bool ignoreIfAlreadyUpdated = false);

		virtual bool setFloat(E_SHADER_TYPE shaderType, const std::string& varname, f32 v, bool ignoreIfAlreadyUpdated = false);

		virtual u32 setRawValue(const std::string& varname, void* raw, u32 size, bool ignoreIfAlreadyUpdated = false);

		virtual bool setRawValue(E_SHADER_TYPE shaderType, const std::string& varname, void* raw, u32 size, bool ignoreIfAlreadyUpdated = false);

		virtual u32 setArray(const std::string& varname, void* data, u32 arraySize, u32 elementSize, bool ignoreIfAlreadyUpdate = false);

		// return the actual element count that has been set.
		virtual u32 setArray(E_SHADER_TYPE shaderType, const std::string& varname, void* data, u32 arraySize, u32 elementSize, bool ignoreIfAlreadyUpdate = false);

		virtual u32 setTexture(const std::string& varname, ITexture* texture);

		virtual bool setTexture(E_SHADER_TYPE shaderType, const std::string& varname, ITexture* texture);

		//virtual bool setInt(u32 v) = 0;

		//virtual void updateInputLayout();

		virtual IShader* getShader(E_SHADER_TYPE type)
		{
			return mShaders[type];
		}

		virtual IInputLayout* getInputLayout()
		{
			return mInputLayout;
		}

		virtual E_PRIMITIVE_TYPE getPrimitiveType() const
		{
			return mPrimitiveType;
		}

		virtual void addShaderAutoVariable(const SShaderAutoVariable& var);

		//virtual void registerAutoSamplers(const std::map<std::string, ISampler*>& samplerMap);

		virtual bool setSampler(const std::string& varname, ISampler* sampler);

		virtual void applySamplers() const;

		virtual const std::vector<SShaderAutoVariable>& getShaderAutoVariables() const;

		virtual ~CPipeline();

		virtual u64 getSortCode() const = 0;

		virtual const std::string& getName() const
		{
			return mName;
		}

		virtual void setInputLayout(IInputLayout* inputLayout)
		{
			mInputLayout = inputLayout;
		}

	protected:
		std::string			mName;
		IShader*			mShaders[5];
		IInputLayout*		mInputLayout;

		E_PRIMITIVE_TYPE	mPrimitiveType;

		std::vector<SShaderAutoVariable>		mShaderAutoVariables;
		std::vector<SShaderSamplerVariable>		mSamplerVariables;

		//std::vector<> mShader;

	private:

	};
}
#endif
