#ifndef CD3D11Driver_H
#define CD3D11Driver_H

#include "GameFinal.h"
#include "CCompositorFactory.h"

#define _GF_SHADER_RESOURCE_VIEW_COUNT 32

namespace gf
{
	class CD3D11Driver : public IVideoDriver
	{
	public:
		struct SD3D11DriverState
		{
			ID3D11Buffer*				VertexBuffers[16];
			ID3D11Buffer*				IndexBuffer;
			IPipeline*					Pipeline;
			IShader*					Shaders[5];
			ID3D11ShaderResourceView*	ShaderResourceViews[5][_GF_SHADER_RESOURCE_VIEW_COUNT];
			bool						ShaderResourceViewIsDirty[5];
			ID3D11SamplerState*			SamplerStates[5][_GF_SHADER_RESOURCE_VIEW_COUNT];
			bool						SamplerStateIsDirty[5];

			IInputLayout*				InputLayout;
			E_PRIMITIVE_TYPE			PrimitiveType;
			ID3D11DepthStencilState*	DepthStencilState;
			ID3D11RasterizerState*		RasterizerState;
			ID3D11BlendState*			BlendState;

			//IRenderTarget*				RenderTarget;
			//ID3D11RenderTargetView*		RenderTargetView;
			IRenderTarget*				RenderTargets[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];
			ID3D11RenderTargetView*		RenderTargetViews[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];

			IDepthStencilSurface*		DepthStencilSurface;
			ID3D11DepthStencilView*		DepthStencilView;

			ID3D11RasterizerState*		ShadowMapRasterizerState;

			void Reset()
			{
				memset(VertexBuffers, 0, sizeof(VertexBuffers));
				IndexBuffer = NULL;
				Pipeline = NULL;

				memset(Shaders, 0, sizeof(Shaders));
				//memset(ShaderResourceViews, 0, sizeof(ShaderResourceViews));
				//memset(ShaderResourceViewIsDirty, 0, sizeof(ShaderResourceViewIsDirty));
				memset(SamplerStates, 0, sizeof(SamplerStates));
				memset(SamplerStateIsDirty, 0, sizeof(SamplerStateIsDirty));

				InputLayout = NULL;
				PrimitiveType = EPT_TRIANGLELIST;
				DepthStencilState = NULL;
				RasterizerState = NULL;
				BlendState = NULL;

				DepthStencilSurface = NULL;
				DepthStencilView = NULL;

				//RenderTarget = NULL;
				//RenderTargetView = NULL;

				memset(RenderTargets, 0, sizeof(RenderTargets));
				memset(RenderTargetViews, 0, sizeof(RenderTargetViews));

			}

			SD3D11DriverState()
			{
				memset(Shaders, 0, sizeof(Shaders));
				memset(ShaderResourceViews, 0, sizeof(ShaderResourceViews));
				memset(ShaderResourceViewIsDirty, 0, sizeof(ShaderResourceViewIsDirty));
				memset(SamplerStates, 0, sizeof(SamplerStates));
				memset(SamplerStateIsDirty, 0, sizeof(SamplerStateIsDirty));
				ShadowMapRasterizerState = NULL;
			}
		};

	public:
		CD3D11Driver(IDevice* device);

		bool init(SCreationParameters& createParam);

		virtual void beginScene(
			bool zBuffer,
			bool stencilBuffer,
			const f32 color[],
			f32 depthValue,
			u8 stencilValue);

		virtual void endScene();

		virtual void bindPrimitiveType(E_PRIMITIVE_TYPE primitiveType);

		virtual void clearDepthStencil(f32 depth, u8 stencil);

		virtual void clearDepth(f32 val);

		virtual void clearStencil(u8 val);

		virtual void clearRenderTarget(const f32 color[]);

		virtual void setDefaultRenderTarget();

		virtual void setRenderTarget(IRenderTarget* pRenderTarget);

		//virtual void setRenderTargets(IRenderTarget* pRenderTargets[], u32 count);

		virtual IRenderTarget* getRenderTarget(u32 index = 0);

		virtual void setDepthStencilSurface(IDepthStencilSurface* depthStencilSurface);

		virtual IDepthStencilSurface* getDepthStencilSurface();

		virtual void setDefaultDepthStencilSurface();

		virtual void setRenderTargetAndDepthStencil(IRenderTarget* pRenderTarget, IDepthStencilSurface* pDepthStencilSurface);

		virtual void setDefaultRenderTargetAndDepthStencil();

		virtual void setMultipleRenderTargets(IRenderTarget* renderTargets[], u32 count);

		virtual void setMultipleRenderTargets(IRenderTarget* renderTargets[], u32 count, IDepthStencilSurface* pDepthStencilSurface);

		//void setTexture(E_SHADER_TYPE shadertype, u32 slot, ITexture* texture);

		void setTexture(E_SHADER_TYPE shadertype, u32 slot, ID3D11ShaderResourceView* shaderResourceView);

		void setSampler(E_SHADER_TYPE shadertype, u32 slot, ISampler* sampler);

		void bindTexture(E_SHADER_TYPE shaderType, u32 textureCount);

		void bindSampler(E_SHADER_TYPE shaderType, u32 samplerCount);

		virtual void clearShader(E_SHADER_TYPE shaderType);

		virtual void setViewport(f32 topLeftX, f32 topLeftY, f32 width, f32 height, f32 minDepth = 0.0f, f32 maxDepth = 1.0f);

		virtual void setViewport(const SViewport& viewport);

		virtual void setViewports(const std::vector<SViewport>& viewports);

		virtual ICompositorFactory*		getCompositorFactory()
		{
			return &mCompositorFactory;
		}

		virtual void setPipelineUsage(E_PIPELINE_USAGE usage);

		virtual void setGBuffersAsRenderTargets();

		virtual void getGBuffers(IRenderTarget* renderTargets[]) const;

		virtual IRenderTarget* getDefaultRenderTarget() { return mDefaultRenderTarget; }

		virtual ~CD3D11Driver();

	private:

		void unbindTextureFromShaders(ID3D11ShaderResourceView* d3dShaderResourceView);

		bool setupGBuffer();

		HWND						mHwnd;
		IDevice*					mDevice;
		IDXGISwapChain*				md3dSwapChain;
		ID3D11Device*				md3dDevice;
		ID3D11DeviceContext*		md3dDeviceContext;
		ID3D11RenderTargetView*		mDefaultRenderTargetView;
		IRenderTarget*				mDefaultRenderTarget;
		
		ID3D11Texture2D*			mDepthStencilBuffer;
		//ID3D11DepthStencilState*	mDepthStencilState;
		ID3D11DepthStencilView*		mDefaultDepthStencilView;
		IDepthStencilSurface*		mDepthStencilSurface;

		ID3D11RasterizerState*		mShadowMapRasterizeState;

		CCompositorFactory			mCompositorFactory;

		ID3D11Debug*				md3dDebug;

		

	public:
		/* this member is to store the current state of d3d11 driver context. */
		SD3D11DriverState			D3D11DriverState;
	};
}
#endif