#include "stdafx.h"
#include "CD3D11ResourceFactory.h"
#include "CD3D11Texture2D.h"
#include "CD3D11RenderState.h"
#include "CD3D11Pipeline.h"
#include "CD3D11Driver.h"
#include "CD3D11VertexShader.h"
#include "CD3D11PixelShader.h"
#include "CD3D11GeometryShader.h"
#include "CD3D11InputLayout.h"
#include "CD3D11MeshBuffer.h"
#include "CD3D11AnimatedMeshBuffer.h"
#include "CD3D11SamplerStateCache.h"
#include "CD3D11HullShader.h"
#include "CD3D11DomainShader.h"
#include "CD3D11RenderTarget.h"
#include "CD3D11DepthStencilSurface.h"
#include "CD3D11TextureCube.h"
#include "CD3D11Texture3D.h"
#include "CD3D11Texture2DArray.h"
#include "gfUtil.h"

namespace gf
{
	CD3D11ResourceFactory::CD3D11ResourceFactory(
		ID3D11Device* pd3dDevice,
		ID3D11DeviceContext* pd3dDeviceContext,
		CD3D11Driver* d3dDriver)
		: md3dDevice(pd3dDevice)
		, md3dDeviceContext(pd3dDeviceContext)
		, md3dDriver(d3dDriver)
	{
		md3dRenderStateCache = new CD3D11RenderStateCache(pd3dDevice);
		md3dSamplerStateCache = new CD3D11SamplerStateCache(pd3dDevice);
	}


	CD3D11ResourceFactory::~CD3D11ResourceFactory()
	{
		md3dRenderStateCache->drop();
	}


	ITexture* CD3D11ResourceFactory::loadTextureFromFile(const std::string& name, const std::string& filename, u32 sortcode)
	{
		CD3D11Texture2D* pTexture = new CD3D11Texture2D(md3dDevice, md3dDeviceContext, name, sortcode, md3dDriver);

		if (!pTexture->loadFromFile(filename))
		{
			pTexture->drop();
			pTexture = nullptr;
		}

		return pTexture;
	}

	ITextureCube* CD3D11ResourceFactory::loadCubeTextureFromFile(
		const std::string& name,
		const std::string& filepath,
		u32 sortcode)
	{
		CD3D11TextureCube* pTexture = new CD3D11TextureCube(md3dDevice, md3dDeviceContext, name, sortcode, md3dDriver);

		if (!pTexture->loadFromFile(filepath))
		{
			pTexture->drop();
			pTexture = nullptr;
		}

		return pTexture;
	}

	ITexture* CD3D11ResourceFactory::createTexture2D(
		const std::string& name,
		u32 sortcode,
		u32 width,
		u32 height,
		u32 bindFlags,
		void* data,
		u32 mipLevel,
		E_GI_FORMAT format,
		u32 pitch)
	{
		CD3D11Texture2D* pTexture = new CD3D11Texture2D(md3dDevice, md3dDeviceContext, name, sortcode, md3dDriver);

		if (!pTexture->create(width, height, bindFlags, data, mipLevel, format, pitch))
		{
			pTexture->drop();
			pTexture = nullptr;
		}

		return pTexture;
	}

	ITexture2DArray* CD3D11ResourceFactory::createTexture2DArray(
		const std::string& name,
		u32 sortcode,
		u32 width,
		u32 height,
		u32 arraySize,
		u32 bindFlags,
		void* data,
		u32 mipLevel,
		E_GI_FORMAT format,
		u32 pitch)
	{
		CD3D11Texture2DArray* pTexture = new CD3D11Texture2DArray(md3dDevice, md3dDeviceContext, name, sortcode, md3dDriver);
		if (!pTexture->create(width, height, arraySize, bindFlags, data, mipLevel, format, pitch))
		{
			pTexture->drop();
			pTexture = nullptr;
		}
		return pTexture;
	}

	ITextureCube* CD3D11ResourceFactory::createTextureCube(
		const std::string& name,
		u32 sortcode,
		u32 size,
		u32 bindFlags,
		void* rawData,
		u32 miplevel,
		E_GI_FORMAT format,
		u32 pitch)
	{
		CD3D11TextureCube* pTexture = new CD3D11TextureCube(md3dDevice, md3dDeviceContext, name, sortcode, md3dDriver);
		if (!pTexture->create(size, bindFlags, rawData, miplevel, format, pitch))
		{
			pTexture->drop();
			pTexture = nullptr;
		}
		return pTexture;
	}


	ITexture* CD3D11ResourceFactory::createDepthStencilTexture(
		const std::string& name,
		u32 sortcode,
		u32 width,
		u32 height,
		u32 depthBitNum,
		u32 stencilBitNum,
		bool multiSampling,
		u32 multiSamplingCount,
		u32 multiSamplingQuality,
		bool bShaderBound,
		bool bindDepthToShader)
	{
		CD3D11Texture2D* pTexture = new CD3D11Texture2D(md3dDevice, md3dDeviceContext, name, sortcode, md3dDriver);

		if (!pTexture->createDepthStencilTexture(width, height, depthBitNum, stencilBitNum, 
			multiSampling, multiSamplingCount, multiSamplingQuality,
			bShaderBound, bindDepthToShader))
		{
			pTexture->drop();
			pTexture = nullptr;
		}

		return pTexture;
	}

	ITexture3D* CD3D11ResourceFactory::createTexture3D(const std::string& name,
		u32 sortcode, u32 width, u32 height, u32 depth, u32	bindFlags,
		void* data, u32 mipLevel, E_GI_FORMAT format,
		u32 pitch /*= 0*/, u32 slicePitch /*= 0*/)
	{
		CD3D11Texture3D* pTexture = new CD3D11Texture3D(md3dDevice, md3dDeviceContext, name, sortcode, md3dDriver);
		if (!pTexture->create(width, height, depth, data, mipLevel, format, pitch, slicePitch))
		{
			pTexture->drop();
			pTexture = nullptr;
		}

		return pTexture;
	}


	/*
	IRenderTarget* CD3D11ResourceFactory::createRenderTarget(
		const std::string& name,
		u32 sortcode,
		bool temporay,
		u32 width,
		u32 height,
		E_GI_FORMAT format,
		bool multiSampling,
		u32 multiSamplingCount,
		u32 multiSamplingQuality)
	{
		if (width == 0)
			width = IDevice::getInstance()->getClientWidth();
		if (height == 0)
			height = IDevice::getInstance()->getClientHeight();

		if (!multiSampling)
		{
			multiSamplingCount = 1;
			multiSamplingQuality = 0;
		}
		else if (multiSamplingCount == 0)
		{
			const SCreationParameters& param = IDevice::getInstance()->getCreationParameters();
			multiSamplingCount = param.MultiSamplingCount;
			multiSamplingQuality = param.MultiSamplingQuality;
		}

		CD3D11RenderTarget* pRenderTarget = new CD3D11RenderTarget(md3dDevice, md3dDeviceContext, name, sortcode, temporay, md3dDriver);
		if (!pRenderTarget->create(width, height, format, multiSampling, multiSamplingCount, multiSamplingQuality))
		{
			pRenderTarget->drop();
			pRenderTarget = nullptr;
		}

		return pRenderTarget;
	}

	IDepthStencilSurface* CD3D11ResourceFactory::createDepthStencilSurface(
		const std::string& name,
		u32 sortcode,
		u32 width,
		u32 height,
		u32 depthBitNum,
		u32 stencilBitNum,
		bool multiSampling,
		u32 multiSamplingCount,
		u32 multiSamplingQuality,
		bool bShaderBound,
		bool bindDepthToShader)
	{
		CD3D11DepthStencilSurface* pDepthStencilSurface = new CD3D11DepthStencilSurface(md3dDevice, md3dDeviceContext, name, sortcode, md3dDriver);
		if (!pDepthStencilSurface->create(width, height, depthBitNum, stencilBitNum, multiSampling, multiSamplingCount, 
			multiSamplingQuality, bShaderBound, bindDepthToShader))
		{
			pDepthStencilSurface->drop();
			pDepthStencilSurface = nullptr;
		}
		return pDepthStencilSurface;
	}
	*/

	IRenderState* CD3D11ResourceFactory::createRenderState(const std::string& name)
	{
		CD3D11RenderState* pRenderState = new CD3D11RenderState(name, md3dRenderStateCache);
		return pRenderState;
	}

	ISampler* CD3D11ResourceFactory::createSampler(const std::string& name, const SSamplerDesc& desc)
	{
		D3D11_SAMPLER_DESC d3d11desc;
		d3d11desc.Filter = static_cast<D3D11_FILTER>(desc.Filter);
		d3d11desc.AddressU = static_cast<D3D11_TEXTURE_ADDRESS_MODE>(desc.AddressU);
		d3d11desc.AddressV = static_cast<D3D11_TEXTURE_ADDRESS_MODE>(desc.AddressV);
		d3d11desc.AddressW = static_cast<D3D11_TEXTURE_ADDRESS_MODE>(desc.AddressW);
		memcpy(&d3d11desc.BorderColor, &desc.BorderColor, sizeof(f32)* 4);
		d3d11desc.ComparisonFunc = getD3d11ComparisonFunc(desc.ComparsionFunc);

		/* set default values of d3d11desc struct */

		d3d11desc.MaxAnisotropy = 16;
		d3d11desc.MaxLOD = FLT_MAX;
		d3d11desc.MinLOD = -FLT_MAX;
		d3d11desc.MipLODBias = 0.0f;

		ISampler* sampler = new CD3D11Sampler(name, md3dSamplerStateCache, d3d11desc);
		return sampler;
	}

	IPipeline* CD3D11ResourceFactory::createPipeline(const std::string& name,
		IShader** shaders,
		u32 shaderCount,
		IInputLayout* inputlayout,
		E_PRIMITIVE_TYPE primitiveType,
		IRenderState* renderState)
	{
		CD3D11RenderState* pd3d11RenderState = dynamic_cast<CD3D11RenderState*>(renderState);

		CD3D11Pipeline* pipeline = new CD3D11Pipeline(name, shaders,
			shaderCount, inputlayout, primitiveType,
			pd3d11RenderState, md3dDeviceContext,
			md3dDriver);

		return pipeline;
	}

	IShader* CD3D11ResourceFactory::createShaderFromFile(
		E_SHADER_TYPE shaderType,
		const char* szFilename,
		const char* szFunctionName,
		u32 id,
		const std::string& shaderName,
		const SShaderMacroSet& macros)
	{
		CD3D11Shader* shader = nullptr;

		switch (shaderType)
		{
		case EST_VERTEX_SHADER:
			shader = new CD3D11VertexShader(id, shaderName, macros, md3dDevice, md3dDeviceContext, md3dDriver);
			break;
		case EST_PIXEL_SHADER:
			shader = new CD3D11PixelShader(id, shaderName, macros, md3dDevice, md3dDeviceContext, md3dDriver);
			break;
		case EST_GEOMETRY_SHADER:
			shader = new CD3D11GeometryShader(id, shaderName, macros, md3dDevice, md3dDeviceContext, md3dDriver);
			break;
		case EST_HULL_SHADER:
			shader = new CD3D11HullShader(id, shaderName, macros, md3dDevice, md3dDeviceContext, md3dDriver);
			break;
		case EST_DOMAIN_SHADER:
			shader = new CD3D11DomainShader(id, shaderName, macros, md3dDevice, md3dDeviceContext, md3dDriver);
			break;
		}

		if (shader && !shader->loadFromFile(szFilename, szFunctionName))
		{
			shader->drop();
			shader = nullptr;
		}

		return shader;
	}

	IInputLayout* CD3D11ResourceFactory::createInputLayout(
		u32 sortCode,
		const std::vector<SInputLayoutElement>& elements,
		IShader* shader,
		u32 hashcode)
	{
		if (shader->getType() != EST_VERTEX_SHADER)
		{
			GF_PRINT_CONSOLE_INFO("InputLayout must be created according to a vertex shader.\n");
			return nullptr;
		}

		CD3D11Shader* pdxShader = dynamic_cast<CD3D11Shader*>(shader);
		ID3D10Blob* pd3dBlob = pdxShader->getShaderBuffer();

		IInputLayout* layout = new CD3D11InputLayout(sortCode, md3dDevice, md3dDeviceContext, pd3dBlob,
			elements, hashcode);

		return layout;
	}

	IAnimatedMeshBuffer* CD3D11ResourceFactory::createAnimatedMeshBuffer(
		E_MEMORY_USAGE usage,
		void* vertices,
		void* animateVertices,
		void* indices,
		u32 vertexCount,
		u32 animateVertexCount,
		u32 indicesCount,
		u32 vertexStride,
		u32 animateVertexStride,
		bool bit32Index)
	{

		CD3D11AnimatedMeshBuffer* buffer = new CD3D11AnimatedMeshBuffer(md3dDevice, md3dDeviceContext,
			md3dDriver, usage);

		if (!buffer->init(vertices, animateVertices, indices, vertexCount,
			animateVertexCount, indicesCount, vertexStride, animateVertexStride, bit32Index))
		{
			buffer->drop();
			return nullptr;
		}

		return buffer;
	}

	IMeshBuffer* CD3D11ResourceFactory::createMeshBuffer(
		E_MEMORY_USAGE usage,
		void* vertices,
		void* indices,
		u32 vertexCount,
		u32 indicesCount,
		u32 vertexStride,
		bool bit32Index)
	{
		CD3D11MeshBuffer* buffer = new CD3D11MeshBuffer(usage, md3dDevice, md3dDeviceContext, md3dDriver);
		if (!buffer)
		{
			GF_PRINT_CONSOLE_INFO("The CD3D11MeshBuffer created failed! Due to lack of memory!\n");
			return nullptr;
		}

		if (!buffer->init(vertices, indices, vertexCount, indicesCount, vertexStride, bit32Index))
		{
			buffer->drop();
			return nullptr;
		}

		return buffer;
	}


}