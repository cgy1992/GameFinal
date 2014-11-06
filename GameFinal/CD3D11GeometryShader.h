#ifndef __CD3D11_GEOMETRY_SHADER_CLASS_H__
#define __CD3D11_GEOMETRY_SHADER_CLASS_H__

#include "CD3D11Shader.h"
namespace gf
{

	class CD3D11GeometryShader : public CD3D11Shader
	{
	public:
		CD3D11GeometryShader(u32 id,
			const std::string& name,
			const SShaderMacroSet& macros,
			ID3D11Device* pd3dDevice,
			ID3D11DeviceContext* pd3dDeviceContext,
			CD3D11Driver* pd3dDriver)
			:CD3D11Shader(id, name, macros, pd3dDevice, pd3dDeviceContext, pd3dDriver)
			, md3dGeometryShader(nullptr)
		{

		}

		virtual E_SHADER_TYPE getType() const
		{
			return EST_GEOMETRY_SHADER;
		}

		virtual const char* getShaderProfile() const
		{
			return "gs_5_0";
		}

		virtual bool loadFromFile(const char* szFileName, const char* szFunctionName);

		virtual void submit();

		virtual ~CD3D11GeometryShader()
		{
			ReleaseCOM(md3dGeometryShader);
		}

	private:
		ID3D11GeometryShader*		md3dGeometryShader;
	};
}
#endif
