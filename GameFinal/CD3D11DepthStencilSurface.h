#ifndef __DEPTH_STENCIL_SURFACE_CLASS_H__
#define __DEPTH_STENCIL_SURFACE_CLASS_H__

#include "IDepthStencilSurface.h"
#include "gfGIFormat.h"
#include "gfUtil.h"
#include "D3DUtil.h"
#include "CD3D11Driver.h"

namespace gf
{
	class CD3D11DepthStencilSurface : public IDepthStencilSurface
	{
	public:
		CD3D11DepthStencilSurface(ID3D11Device* d3d11Device,
			ID3D11DeviceContext* d3d11DeviceContext)
			:md3dDevice(d3d11Device)
			, md3dDeviceContext(d3d11DeviceContext)
			, md3dDepthStencilView(NULL)
			, md3dShaderResourceView(NULL)
		{

		}

		bool create(ITexture* texture, ID3D11Texture2D* pTexture2D,
			DXGI_FORMAT depthStencilFormat, 
			ID3D11ShaderResourceView* d3dShaderResourceView,
			bool multiSampling, u32 width, u32 height,
			u32 depthBits, u32 stencilBits);

		bool createOneInArray(ITexture* texture,
			ID3D11Texture2D* pTextureArray,
			ID3D11ShaderResourceView* d3dShaderResourceView,
			u32 index, u32 width, u32 height, E_GI_FORMAT format);

		ID3D11DepthStencilView* getDepthStencilView()
		{
			return md3dDepthStencilView;
		}

		ID3D11ShaderResourceView* getShaderResourceView()
		{
			return md3dShaderResourceView; 
		}

		virtual void clear(f32 depth = 1.0f, u8 stencil = 0);

		virtual void clearDepth(f32 depth = 1.0f);

		virtual void clearStencil(u8 stencil = 0);

		virtual void apply(E_SHADER_TYPE shaderType, u32 slot);



		virtual ~CD3D11DepthStencilSurface()
		{
			ReleaseCOM(md3dDepthStencilView);
		}

	private:

		ID3D11DepthStencilView*			md3dDepthStencilView;
		ID3D11Device*					md3dDevice;
		ID3D11DeviceContext*			md3dDeviceContext;
		ID3D11ShaderResourceView*		md3dShaderResourceView;
	};

	/*
	class CD3D11DepthStencilSurface : public IDepthStencilSurface
	{
	public:
		CD3D11DepthStencilSurface(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dDeviceContext,
			const std::string& name, u32 sortcode, CD3D11Driver* d3d11Driver)
			:IDepthStencilSurface(name, sortcode)
			, md3dDevice(pd3dDevice)
			, md3dDeviceContext(pd3dDeviceContext)
			, md3dTexture(nullptr)
			, md3dDepthStencilView(nullptr)
			, md3dShaderResourceView(nullptr)
			, mTextureWidth(0)
			, mTextureHeight(0)
			, md3d11Driver(d3d11Driver)
		{

		}

		bool create(u32 width, u32 height,
			u32 depthBitNum, u32 stencilBitNum, 
			bool multiSampling, u32 multiSamplingCount, u32 multiSamplingQuality,
			bool bShaderBound, bool bindDepthToShader);


		ID3D11ShaderResourceView* getShaderResourceView()
		{
			return md3dShaderResourceView;
		}



		virtual void clear(f32 depth = 1.0f, u8 stencil = 0);

		virtual void clearDepth(f32 depth = 1.0f);

		virtual void clearStencil(u8 stencil = 0);

		virtual ~CD3D11DepthStencilSurface()
		{
			ReleaseCOM(md3dTexture);
			ReleaseCOM(md3dShaderResourceView);
			ReleaseCOM(md3dDepthStencilView);
		}

		virtual u32 getWidth() const
		{
			return mTextureWidth;
		}

		virtual u32 getHeight() const
		{
			return mTextureHeight;
		}

		virtual void apply(E_SHADER_TYPE shaderType, u32 slot);

	private:
		ID3D11Device*					md3dDevice;
		ID3D11DeviceContext*			md3dDeviceContext;
		CD3D11Driver*					md3d11Driver;

		ID3D11Texture2D*				md3dTexture;
		ID3D11ShaderResourceView*		md3dShaderResourceView;
		ID3D11DepthStencilView*			md3dDepthStencilView;
		u32								mTextureWidth;
		u32								mTextureHeight;

	};
	*/
}

#endif

