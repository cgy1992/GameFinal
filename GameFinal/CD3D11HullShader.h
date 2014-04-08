#ifndef __CD3D11_HULL_SHADER_CLASS_H__
#define __CD3D11_HULL_SHADER_CLASS_H__

#include "CD3D11Shader.h"

class CD3D11HullShader : public CD3D11Shader
{
public:
	CD3D11HullShader(u32 id,
		std::string name,
		ID3D11Device* pd3dDevice,
		ID3D11DeviceContext* pd3dDeviceContext,
		CD3D11Driver* pd3dDriver)
		:CD3D11Shader(id, name, pd3dDevice, pd3dDeviceContext, pd3dDriver)
		, md3dHullShader(nullptr)
	{

	}

	virtual E_SHADER_TYPE getType() const
	{
		return EST_HULL_SHADER;
	}

	virtual const char* getShaderProfile() const
	{
		return "hs_5_0";
	}

	virtual bool loadFromFile(const char* szFileName, const char* szFunctionName);

	virtual void submit();

	virtual ~CD3D11HullShader()
	{
		ReleaseCOM(md3dHullShader);
	}

private:
	ID3D11HullShader*			md3dHullShader;
};



#endif
