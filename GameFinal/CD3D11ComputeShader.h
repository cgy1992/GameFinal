#ifndef __D3D11_COMPUTE_SHADER_CLASS_H__
#define __D3D11_COMPUTE_SHADER_CLASS_H__

#include "CD3D11Shader.h"

namespace gf
{

	class CD3D11ComputeShader : public CD3D11Shader
	{
	public:
		CD3D11ComputeShader(u32 id,
			const std::string& name,
			const SShaderMacroSet& macros,
			ID3D11Device* pd3dDevice,
			ID3D11DeviceContext* pd3dDeviceContext,
			CD3D11Driver* pd3dDriver)
			:CD3D11Shader(id, name, macros, pd3dDevice, pd3dDeviceContext, pd3dDriver)
			, md3dComputeShader(nullptr)
		{

		}

		virtual E_SHADER_TYPE getType() const
		{
			return EST_COMPUTE_SHADER;
		}

		virtual const char* getShaderProfile() const
		{
			return "cs_5_0";
		}

		virtual bool loadFromFile(const char* szFileName, const char* szFunctionName);

		virtual void submit();

		virtual ~CD3D11ComputeShader()
		{
			ReleaseCOM(md3dComputeShader);
		}

	private:
		ID3D11ComputeShader*		md3dComputeShader;
	};

}

#endif