#ifndef __D3D11_RENDER_TARGET_CLASS_H__
#define __D3D11_RENDER_TARGET_CLASS_H__

#include "IRenderTarget.h"
#include "gfGIFormat.h"
#include "D3DUtil.h"
#include "CD3D11Driver.h"

namespace gf
{
	class CD3D11RenderTarget : public IRenderTarget
	{
	public:
		CD3D11RenderTarget(ID3D11Device* d3d11Device, 
			ID3D11DeviceContext* d3d11DeviceContext)
			:md3dDevice(d3d11Device)
			, md3dDeviceContext(d3d11DeviceContext)
			, md3dRenderTargetView(NULL)
			, md3dShaderResourceView(NULL)
		{
			
		}

		CD3D11RenderTarget(ID3D11Device* pd3dDevice, 
			ID3D11DeviceContext* pd3dDeviceContext,
			ID3D11RenderTargetView* d3dRenderTargetView,
			u32 width, u32 height)
			: md3dDevice(pd3dDevice)
			, md3dDeviceContext(pd3dDeviceContext)
			, md3dShaderResourceView(nullptr)
			, md3dRenderTargetView(d3dRenderTargetView)
		{
			mWidth = width;
			mHeight = height;
		}

		bool create(ITexture* texture, ID3D11Texture2D* pTexture2D, 
			ID3D11ShaderResourceView* d3dShaderResourceView, u32 width, u32 height);

		virtual void clear(const f32 color[]);

		virtual void clear();

		virtual void apply(E_SHADER_TYPE shaderType, u32 slot);

		ID3D11RenderTargetView* getRenderTargetView()
		{
			return md3dRenderTargetView;
		}

		ID3D11ShaderResourceView* getShaderResourceView()
		{
			return md3dShaderResourceView;
		}

	private:

		ID3D11RenderTargetView*			md3dRenderTargetView;
		ID3D11Device*					md3dDevice;
		ID3D11DeviceContext*			md3dDeviceContext;
		ID3D11ShaderResourceView*		md3dShaderResourceView;
	};

		/*
	class CD3D11RenderTarget : public IRenderTarget
	{
	public:
		CD3D11RenderTarget(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dDeviceContext, 
			const std::string& name, u32 sortcode, bool temporary, CD3D11Driver* d3d11Driver)
			:IRenderTarget(name, sortcode, temporary)
			, md3dDevice(pd3dDevice)
			, md3dDeviceContext(pd3dDeviceContext)
			, md3dTexture(nullptr)
			, md3dShaderResourceView(nullptr)
			, md3dRenderTargetView(nullptr)
			, mTextureWidth(0)
			, mTextureHeight(0)
			, md3d11Driver(d3d11Driver)
		{
			
		}

		CD3D11RenderTarget(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dDeviceContext,
			const std::string& name, CD3D11Driver* d3d11Driver, 
			ID3D11RenderTargetView* d3dRenderTargetView,
			u32 width, u32 height)
			:IRenderTarget(name, 0, false)
			, md3dDevice(pd3dDevice)
			, md3dDeviceContext(pd3dDeviceContext)
			, md3dTexture(nullptr)
			, md3dShaderResourceView(nullptr)
			, md3dRenderTargetView(d3dRenderTargetView)
			, md3d11Driver(d3d11Driver)
			, mTextureWidth(width)
			, mTextureHeight(height)
		{
			
		}

		bool create(u32 width, u32 height, E_GI_FORMAT format, bool multiSampling, u32 multiSamplingCount, u32 multiSamplingQuality);


		ID3D11ShaderResourceView* getShaderResourceView()
		{
			return md3dShaderResourceView;
		}
		
		ID3D11RenderTargetView* getRenderTargetView()
		{
			return md3dRenderTargetView;
		}

		virtual void clear(const f32 color[]);

		virtual void clear();

		virtual ~CD3D11RenderTarget()
		{
			ReleaseCOM(md3dTexture);
			ReleaseCOM(md3dShaderResourceView);
			ReleaseCOM(md3dRenderTargetView);
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
		ID3D11RenderTargetView*			md3dRenderTargetView;

		u32								mTextureWidth;
		u32								mTextureHeight;
	};

	*/
}


#endif
