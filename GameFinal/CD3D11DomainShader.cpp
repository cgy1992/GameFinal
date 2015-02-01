#include "stdafx.h"
#include "CD3D11DomainShader.h"

namespace gf
{

	bool CD3D11DomainShader::loadFromFile(const char* szFileName, const char* szFunctionName)
	{
		HRESULT hr;
		if (!compile(szFileName, szFunctionName))
			return false;

		hr = md3dDevice->CreateDomainShader(mShaderBuffer->GetBufferPointer(), mShaderBuffer->GetBufferSize(), NULL, &md3dDomainShader);

		if (FAILED(hr))
		{
			return false;
		}

		return true;
	}

	void CD3D11DomainShader::submit()
	{
		u32 bufferCount = mConstantBuffers.size();

		md3dDeviceContext->DSSetConstantBuffers(0, bufferCount, md3dConstantBuffers);

		md3dDeviceContext->DSSetShader(md3dDomainShader, 0, 0);
	}

}