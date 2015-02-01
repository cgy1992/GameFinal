#include "stdafx.h"
#include "CD3D11ComputeShader.h"

namespace gf
{
	bool CD3D11ComputeShader::loadFromFile(const char* szFileName, const char* szFunctionName)
	{
		HRESULT hr;
		if (!compile(szFileName, szFunctionName))
			return false;

		hr = md3dDevice->CreateComputeShader(mShaderBuffer->GetBufferPointer(), mShaderBuffer->GetBufferSize(), NULL, &md3dComputeShader);

		if (FAILED(hr))
		{
			return false;
		}

		return true;
	}

	void CD3D11ComputeShader::submit()
	{
		u32 bufferCount = mConstantBuffers.size();

		md3dDeviceContext->CSSetConstantBuffers(0, bufferCount, md3dConstantBuffers);

		md3dDeviceContext->CSSetShader(md3dComputeShader, 0, 0);
	}

}
