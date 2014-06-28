#ifndef __CD3D11_DOMAIN_SHADER_CLASS_H__
#define __CD3D11_DOMAIN_SHADER_CLASS_H__

#include "CD3D11Shader.h"
namespace gf
{

	class CD3D11DomainShader : public CD3D11Shader
	{
	public:
		CD3D11DomainShader(u32 id,
			std::string name,
			ID3D11Device* pd3dDevice,
			ID3D11DeviceContext* pd3dDeviceContext,
			CD3D11Driver* pd3dDriver)
			:CD3D11Shader(id, name, pd3dDevice, pd3dDeviceContext, pd3dDriver)
			, md3dDomainShader(nullptr)
		{

		}

		virtual E_SHADER_TYPE getType() const
		{
			return EST_DOMAIN_SHADER;
		}

		virtual const char* getShaderProfile() const
		{
			return "ds_5_0";
		}

		virtual bool loadFromFile(const char* szFileName, const char* szFunctionName);

		virtual void submit();

		virtual ~CD3D11DomainShader()
		{
			ReleaseCOM(md3dDomainShader);
		}

	private:
		ID3D11DomainShader*			md3dDomainShader;
	};

}
#endif
