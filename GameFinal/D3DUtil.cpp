#include "stdafx.h"
#include "D3DUtil.h"

namespace gf
{

	/*
			if (depthBitNum == 32 && stencilBitNum == 0)
			{
			d3dTexFormat = DXGI_FORMAT_R32_TYPELESS;
			d3dDepthStencilFormat = DXGI_FORMAT_D32_FLOAT;
			d3dShaderViewFormat = DXGI_FORMAT_R32_FLOAT;
			}
			else if (depthBitNum == 24 && stencilBitNum == 8)
			{
			d3dTexFormat = DXGI_FORMAT_R24G8_TYPELESS;
			d3dDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
			if (bindDepthToShader)
			d3dShaderViewFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			else
			d3dShaderViewFormat = DXGI_FORMAT_X24_TYPELESS_G8_UINT;
			}
			else if (depthBitNum == 16 && stencilBitNum == 0)
			{
			d3dTexFormat = DXGI_FORMAT_R16_TYPELESS;
			d3dDepthStencilFormat = DXGI_FORMAT_D16_UNORM;
			d3dShaderViewFormat = DXGI_FORMAT_R16_FLOAT;
			}
	*/

	DXGI_FORMAT getDxgiFormat(E_GI_FORMAT format)
	{
		switch (format)
		{
		case EGF_R32G32B32A32_FLOAT:	return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case EGF_R32G32B32_FLOAT:		return DXGI_FORMAT_R32G32B32_FLOAT;
		case EGF_R32G32_FLOAT:			return DXGI_FORMAT_R32G32_FLOAT;
		case EGF_R32_FLOAT:				return DXGI_FORMAT_R32_FLOAT;
		case EGF_R8_UINT:				return DXGI_FORMAT_R8_UINT;
		case EGF_R8G8_UINT:				return DXGI_FORMAT_R8G8_UINT;
		case EGF_R8G8B8A8_UINT:			return DXGI_FORMAT_R8G8B8A8_UINT;
		case EGF_R16_FLOAT:				return DXGI_FORMAT_R16_FLOAT;
		case EGF_R8G8B8A8_UNORM:		return DXGI_FORMAT_R8G8B8A8_UNORM;
		case EGF_D16:					return DXGI_FORMAT_R16_TYPELESS;
		case EGF_D24S8:					return DXGI_FORMAT_R24G8_TYPELESS;
		case EGF_D32:					return DXGI_FORMAT_R32_TYPELESS;
		}
		return DXGI_FORMAT_UNKNOWN;
	}

	E_GI_FORMAT getFormatFromDxgi(DXGI_FORMAT format)
	{
		switch (format)
		{
		case DXGI_FORMAT_R32G32B32A32_FLOAT:	return EGF_R32G32B32A32_FLOAT;
		case DXGI_FORMAT_R32G32B32_FLOAT:		return EGF_R32G32B32_FLOAT;
		case DXGI_FORMAT_R32G32_FLOAT:			return EGF_R32G32_FLOAT;
		case DXGI_FORMAT_R32_FLOAT:				return EGF_R32_FLOAT;
		case DXGI_FORMAT_R8_UINT:				return EGF_R8_UINT;
		case DXGI_FORMAT_R8G8_UINT:				return EGF_R8G8_UINT;
		case DXGI_FORMAT_R8G8B8A8_UINT:			return EGF_R8G8B8A8_UINT;
		case DXGI_FORMAT_R16_FLOAT:				return EGF_R16_FLOAT;
		case DXGI_FORMAT_R8G8B8A8_UNORM:		return EGF_R8G8B8A8_UNORM;
		}

		return EGF_UNKNOWN;
	}

	D3D11_USAGE getD3d11Usage(E_MEMORY_USAGE usage)
	{
		switch (usage)
		{
		case EMU_STATIC:	return D3D11_USAGE_IMMUTABLE;
		case EMU_DEFAULT:	return D3D11_USAGE_DEFAULT;
		case EMU_DYNAMIC:	return D3D11_USAGE_DYNAMIC;
		case EMU_STAGING:	return D3D11_USAGE_STAGING;
		}
		return D3D11_USAGE_IMMUTABLE;
	}

	D3D11_COMPARISON_FUNC getD3d11ComparisonFunc(E_COMPARISON_FUNC func)
	{
		switch (func)
		{
		case ECF_NEVER: return D3D11_COMPARISON_NEVER;
		case ECF_LESS: return D3D11_COMPARISON_LESS;
		case ECF_EQUAL: return D3D11_COMPARISON_EQUAL;
		case ECF_LESS_EQUAL: return D3D11_COMPARISON_LESS_EQUAL;
		case ECF_GREATER: return D3D11_COMPARISON_GREATER;
		case ECF_NOT_EQUAL: return D3D11_COMPARISON_NOT_EQUAL;
		case ECF_GREATER_EQUAL: return D3D11_COMPARISON_GREATER_EQUAL;
		case ECF_ALWAYS: return D3D11_COMPARISON_ALWAYS;
		}
		return D3D11_COMPARISON_ALWAYS;
	}

	UINT getD3dx11BindFlags(u32 flags)
	{
		UINT d3dFlags = 0;
		if (flags & ETBT_SHADER)
			d3dFlags |= D3D11_BIND_SHADER_RESOURCE;
		if (flags & ETBT_RENDER_TARGET)
			d3dFlags |= D3D11_BIND_RENDER_TARGET;
		if (flags & ETBT_DEPTH_STENCIL)
			d3dFlags |= D3D11_BIND_DEPTH_STENCIL;

		return d3dFlags;
	}


	DXGI_FORMAT getDxgiSRVFormat(E_GI_FORMAT format)
	{
		if (format == EGF_D16)
			return DXGI_FORMAT_R16_FLOAT;
		if (format == EGF_D24S8)
			return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		if (format == EGF_D32)
			return DXGI_FORMAT_R32_FLOAT;

		return getDxgiFormat(format);
	}

	DXGI_FORMAT getDxgiDSVFormat(E_GI_FORMAT format)
	{
		if (format == EGF_D16)
			return DXGI_FORMAT_D16_UNORM;
		if (format == EGF_D24S8)
			return DXGI_FORMAT_D24_UNORM_S8_UINT;
		if (format == EGF_D32)
			return DXGI_FORMAT_D32_FLOAT;

		return getDxgiFormat(format);
	}
}

