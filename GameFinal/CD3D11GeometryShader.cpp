#include "stdafx.h"
#include "CD3D11GeometryShader.h"

namespace gf
{

	bool CD3D11GeometryShader::loadFromFile(const char* szFileName, const char* szFunctionName)
	{
		HRESULT hr;
		if (!compile(szFileName, szFunctionName, EST_GEOMETRY_SHADER))
			return false;

		hr = md3dDevice->CreateGeometryShader(mShaderBuffer->GetBufferPointer(), mShaderBuffer->GetBufferSize(), NULL, &md3dGeometryShader);

		if (FAILED(hr))
		{
			return false;
		}

		return true;
	}

	void CD3D11GeometryShader::submit()
	{
		u32 bufferCount = mConstantBuffers.size();

		md3dDeviceContext->GSSetConstantBuffers(0, bufferCount, md3dConstantBuffers);

		md3dDeviceContext->GSSetShader(md3dGeometryShader, 0, 0);
	}

}