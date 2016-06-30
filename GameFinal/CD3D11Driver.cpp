#include "stdafx.h"
#include "CD3D11Driver.h"
#include "CD3D11SimpleMesh.h"
#include "CD3D11ResourceFactory.h"
#include "CD3D11Texture2D.h"
#include "CMaterialManager.h"
#include "CTextureManager.h"
#include "CMaterialManager.h"
#include "CRenderStateManager.h"
#include "CShaderManager.h"
#include "CInputLayoutManager.h"
#include "CPipelineManager.h"
#include "CMeshManager.h"
#include "CGeometryCreator.h"
#include "CResourceGroupManager.h"
#include "CSamplerManager.h"
#include "CD3D11RenderTarget.h"
#include "CD3D11DepthStencilSurface.h"
#include "CShaderVariableInjection.h"

namespace gf
{
	CD3D11Driver::CD3D11Driver(IApplication* device)
		:mDevice(device)
		, md3dDebug(nullptr)
		, mClearColor(0, 0, 0, 1.0f)
	{
		
	}

	bool CD3D11Driver::init(SAppSettings& settings)
	{
		HRESULT result;
		IDXGIFactory* factory;
		IDXGIAdapter* adapter;
		IDXGIOutput* adapterOutput;
		unsigned int numModes, i, numerator = 0, denominator = 1, stringLength;
		DXGI_MODE_DESC* displayModeList;
		DXGI_ADAPTER_DESC adapterDesc;
		int error;
		D3D11_TEXTURE2D_DESC depthBufferDesc;
		//D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		//D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		//D3D11_RASTERIZER_DESC rasterDesc;
		//D3D11_VIEWPORT viewport;

		//const SCreationParameters& params = mDevice->getCreationParameters();

		// Create a DirectX graphics interface factory.

		mBackBufferWidth = settings.Graphics.BackBufferWidth;
		mBackBufferHeight = settings.Graphics.BackBufferHeight;

		result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
		if (FAILED(result))
		{
			return false;
		}

		// Use the factory to create an adapter for the primary graphics interface (video card).
		result = factory->EnumAdapters(0, &adapter);
		if (FAILED(result))
		{
			return false;
		}

		// Enumerate the primary adapter output (monitor).
		result = adapter->EnumOutputs(0, &adapterOutput);
		if (FAILED(result))
		{
			return false;
		}

		// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
		result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
		if (FAILED(result))
		{
			return false;
		}

		// Create a list to hold all the possible display modes for this monitor/video card combination.
		displayModeList = new DXGI_MODE_DESC[numModes];
		if (!displayModeList)
		{
			return false;
		}

		// Now fill the display mode list structures.
		result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
		if (FAILED(result))
		{
			return false;
		}

		// Now go through all the display modes and find the one that matches the screen width and height.
		// When a match is found store the numerator and denominator of the refresh rate for that monitor.
		for (i = 0; i < numModes; i++)
		{
			if (displayModeList[i].Width == (unsigned int)settings.Graphics.BackBufferWidth)
			{
				if (displayModeList[i].Height == (unsigned int)settings.Graphics.BackBufferHeight)
				{
					numerator = displayModeList[i].RefreshRate.Numerator;
					denominator = displayModeList[i].RefreshRate.Denominator;
				}
			}
		}

		// Get the adapter (video card) description.
		result = adapter->GetDesc(&adapterDesc);
		if (FAILED(result))
		{
			return false;
		}

		// Store the dedicated video card memory in megabytes.
		mVideoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

		// Convert the name of the video card to a character array and store it.
		error = wcstombs_s(&stringLength, mVideoCardDescription, 128, adapterDesc.Description, 128);
		if (error != 0)
		{
			return false;
		}

		// Release the display mode list.
		delete[] displayModeList;
		displayModeList = 0;

		// Release the adapter output.
		adapterOutput->Release();
		adapterOutput = 0;

		// Release the adapter.
		adapter->Release();
		adapter = 0;

		// Release the factory.
		factory->Release();
		factory = 0;

		UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		D3D_FEATURE_LEVEL featureLevel;

		/* 如果将这个标志设为DEBUG，则绘制效率大大降低，且帧频极不稳定 */
		//createDeviceFlags = 0;

		HRESULT hr;
		hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE,
			NULL, createDeviceFlags, 0, 0, D3D11_SDK_VERSION, &md3dDevice, &featureLevel, &md3dDeviceContext);

		if (FAILED(hr))
			return false;

#if defined(DEBUG) || defined(_DEBUG)
		md3dDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&md3dDebug));
#endif

		if (featureLevel != D3D_FEATURE_LEVEL_11_0)
		{
			throw std::runtime_error("DirectX11 is not supported!");
			return false;
		}

		UINT numQualityLevels;
		md3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &numQualityLevels);

		IDXGIDevice* dxgiDevice = 0;
		md3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
		IDXGIAdapter* dxgiAdapter = 0;
		dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);
		IDXGIFactory* dxgiFactory = 0;
		dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);

		DXGI_SWAP_CHAIN_DESC swapChainDesc;

		// Initialize the swap chain description.
		ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

		// Set the width and height of the back buffer.
		swapChainDesc.BufferDesc.Width = settings.Graphics.BackBufferWidth;
		swapChainDesc.BufferDesc.Height = settings.Graphics.BackBufferHeight;

		// Set regular 32-bit surface for the back buffer.
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		// Set the refresh rate of the back buffer.
		if (settings.Graphics.VsyncEnabled)
		{
			swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
			swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
		}
		else
		{
			swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
			swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		}

		// Set multisampling.
		if (settings.Graphics.MultiSamplingQuality == 0)
		{
			swapChainDesc.SampleDesc.Count = 1;
			swapChainDesc.SampleDesc.Quality = 0;

			settings.Graphics.MultiSamplingCount = 1;
		}
		else
		{
			swapChainDesc.SampleDesc.Count = settings.Graphics.MultiSamplingCount;
			UINT numQualityLevels;
			md3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &numQualityLevels);

			swapChainDesc.SampleDesc.Quality = min(numQualityLevels - 1, settings.Graphics.MultiSamplingQuality);

			settings.Graphics.MultiSamplingQuality = swapChainDesc.SampleDesc.Quality;
		}
		
		// set member attributes of class


		// Set to a single back buffer.
		swapChainDesc.BufferCount = 1;

		// Set the usage of the back buffer.
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

		// Set the handle for the window to render to.
		swapChainDesc.OutputWindow = (HWND)settings.Window.BackBufferWindowHandle;

		// Set to full screen or windowed mode.
		if (settings.Window.Style & EWS_FULLSCREEN)
		{
			swapChainDesc.Windowed = false;
		}
		else
		{
			swapChainDesc.Windowed = true;
		}

		// Set the scan line ordering and scaling to unspecified.
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		// Discard the back buffer contents after presenting.
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		// Don't set the advanced flags.
		swapChainDesc.Flags = 0;

		hr = dxgiFactory->CreateSwapChain(md3dDevice, &swapChainDesc, &md3dSwapChain);
		if (FAILED(hr))
			return false;
		ReleaseCOM(dxgiDevice);
		ReleaseCOM(dxgiAdapter);
		ReleaseCOM(dxgiFactory);
		// Get the pointer to the back buffer.

		ID3D11Texture2D* backBuffer;
		result = md3dSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
		if (FAILED(result))
		{
			return false;
		}
		md3dDevice->CreateRenderTargetView(backBuffer, 0, &mDefaultRenderTargetView);
		ReleaseCOM(backBuffer);

		mDefaultRenderTarget = new CD3D11RenderTarget(md3dDevice, md3dDeviceContext, mDefaultRenderTargetView,
			settings.Graphics.BackBufferWidth, settings.Graphics.BackBufferHeight);

		// Initialize the description of the depth buffer.
		ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

		// Setup the viewport for rendering.
		setViewport(0, 0, static_cast<f32>(mBackBufferWidth), static_cast<f32>(mBackBufferHeight));

		//create resource factory
		mResourceFactory = new CD3D11ResourceFactory(
			md3dDevice, md3dDeviceContext, this);
		IResourceFactory::_setInstance(mResourceFactory);

		//create geometry creator
		mGeometryCreator = new CGeometryCreator;
		IGeometryCreator::_setInstance(mGeometryCreator);

		//create material manager
		mMaterialManager = new CMaterialManager;
		IMaterialManager::_setInstance(mMaterialManager);

		//create sampler manager
		mSamplerManager = new CSamplerManager(mResourceFactory);
		ISamplerManager::_setInstance(mSamplerManager);


		// create shadermanager
		mShaderManager = new CShaderManager(mResourceFactory);
		IShaderManager::_setInstance(mShaderManager);

		// create inputlayout manager
		mInputLayoutManager = new CInputLayoutManager(mResourceFactory);
		IInputLayoutManager::_setInstance(mInputLayoutManager);

		//create texture manager
		mTextureManager = new CTextureManager(mDevice, mResourceFactory);
		ITextureManager::_setInstance(mTextureManager);

		//create render state manager
		mRenderStateManager = new CRenderStateManager(mResourceFactory);
		IRenderStateManager::_setInstance(mRenderStateManager);

		//create mesh manager
		mMeshManager = new CMeshManager(mResourceFactory, mGeometryCreator, mTextureManager);
		IMeshManager::_setInstance(mMeshManager);

		
		//create pipeline manager
		mPipeManager = new CPipelineManager(mResourceFactory);
		IPipelineManager::_setInstance(mPipeManager);

		//create resource group manager
		mResourceGroupManager = new CResourceGroupManager(mTextureManager, mShaderManager,
			mInputLayoutManager, mRenderStateManager, mPipeManager, mMaterialManager, mMeshManager, mSamplerManager);
		IResourceGroupManager::_setInstance(mResourceGroupManager);

		//mResourceFactory->setResourceGroupManager(mResourceGroupManager);

		// create default depth-stencil-buffer
		bool multiSampling = (settings.Graphics.MultiSamplingCount > 1);


		mDepthStencilSurface = mTextureManager->createDepthStencilSurface("_default_depth_stencil_surface",
			0, 0, settings.Graphics.DepthBits, settings.Graphics.StencilBits,
			multiSampling, settings.Graphics.MultiSamplingCount, settings.Graphics.MultiSamplingQuality, true);

		if (!mDepthStencilSurface)
		{
			GF_PRINT_CONSOLE_INFO("Depth Stencil Surface has failed to be created.\n");
			return false;
		}

		D3D11DriverState.DepthStencilSurface = mDepthStencilSurface;
		CD3D11DepthStencilSurface* d3dDepthStencilSurface = dynamic_cast<CD3D11DepthStencilSurface*>(mDepthStencilSurface);
		mDefaultDepthStencilView = d3dDepthStencilSurface->getDepthStencilView();
		D3D11DriverState.DepthStencilView = mDefaultDepthStencilView;


		// create mShadowMapRasterizeState. This state is only for rendering shadow maps.
		D3D11_RASTERIZER_DESC shadowRasterizeState;
		shadowRasterizeState.FillMode = D3D11_FILL_SOLID;
		shadowRasterizeState.CullMode = D3D11_CULL_BACK; // RENDER BACK FACE
		shadowRasterizeState.FrontCounterClockwise = TRUE; 
		shadowRasterizeState.DepthBiasClamp = 0.0f;
		//shadowRasterizeState.DepthBias = 100000;
		//shadowRasterizeState.SlopeScaledDepthBias = 1.0f;
		shadowRasterizeState.DepthBias = 0;
		shadowRasterizeState.SlopeScaledDepthBias = 0;
		shadowRasterizeState.DepthClipEnable = TRUE;
		shadowRasterizeState.ScissorEnable = FALSE;
		shadowRasterizeState.MultisampleEnable = FALSE;
		shadowRasterizeState.AntialiasedLineEnable = FALSE;

		hr = md3dDevice->CreateRasterizerState(&shadowRasterizeState, &mShadowMapRasterizeState);
		if (FAILED(hr))
			return false;

		D3D11DriverState.ShadowMapRasterizerState = mShadowMapRasterizeState;

		return true;
	}


	CD3D11Driver::~CD3D11Driver()
	{
		/* 释放的顺序
		  1, mesh
		  2, material
		  3, pipeline
		  4, texture
		  5, shader,
		  6, inputlayout
		  7, render-state
		  */

		mDefaultRenderTarget->drop();
		ReleaseCOM(D3D11DriverState.ShadowMapRasterizerState);

		for (u32 i = 0; i < EST_SHADER_COUNT; i++)
			clearShader(E_SHADER_TYPE(i));

		ReleaseReferenceCounted(mMeshManager);
		ReleaseReferenceCounted(mMaterialManager);
		ReleaseReferenceCounted(mPipeManager);
		ReleaseReferenceCounted(mTextureManager);
		ReleaseReferenceCounted(mShaderManager);
		ReleaseReferenceCounted(mInputLayoutManager);
		ReleaseReferenceCounted(mRenderStateManager);

		ReleaseReferenceCounted(mResourceFactory);
		ReleaseReferenceCounted(mGeometryCreator);

		ReleaseCOM(md3dSwapChain);
		ReleaseCOM(md3dDeviceContext);
		ReleaseCOM(md3dDevice);
		//md3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);

		ReleaseCOM(md3dDebug);
	}

	void CD3D11Driver::beginScene(
		bool zBuffer,
		bool stencilBuffer,
		const f32 clearColor[],
		f32 depthValue,
		u8 stencilValue)
	{
		//md3dDeviceContext->ClearRenderTargetView(mDefaultRenderTargetView, clearColor);

		// save clear color
		mClearColor.x = clearColor[0];
		mClearColor.y = clearColor[1];
		mClearColor.z = clearColor[2];
		mClearColor.w = clearColor[3];

		UINT clearFlag = 0;
		if (zBuffer) clearFlag |= D3D11_CLEAR_DEPTH;
		if (stencilBuffer) clearFlag |= D3D11_CLEAR_STENCIL;

		//md3dDeviceContext->ClearDepthStencilView(mDepthStencilView, clearFlag, depthValue, stencilValue);

		D3D11DriverState.Reset();

		for (u32 i = 0; i <= EST_SHADER_COUNT; i++)
			clearShader((E_SHADER_TYPE)i);

		mCurrentPipelineUsage = EPU_FORWARD;

		setDefaultRenderTargetAndDepthStencil();

		// if it is deferred shading, setup gbuffer
		if (mDeferredShading)
		{
			setupGBuffer();
		}

		md3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		md3dDeviceContext->ClearRenderTargetView(mDefaultRenderTargetView, clearColor);
		md3dDeviceContext->ClearDepthStencilView(mDefaultDepthStencilView, clearFlag, depthValue, stencilValue);
	}

	void CD3D11Driver::endScene()
	{
		md3dSwapChain->Present(0, 0);
	}


	void CD3D11Driver::bindPrimitiveType(E_PRIMITIVE_TYPE primitiveType)
	{
		D3D11_PRIMITIVE_TOPOLOGY pt;

		/*
		if (primitiveType == EPT_TRIANGLELIST)
		pt = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		else if (primitiveType == EPT_POINTLIST)
		pt = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
		*/

		pt = static_cast<D3D11_PRIMITIVE_TOPOLOGY>(primitiveType);
		md3dDeviceContext->IASetPrimitiveTopology(pt);
	}

	void CD3D11Driver::clearDepthStencil(f32 depth, u8 stencil)
	{
		if (D3D11DriverState.DepthStencilView)
		{
			UINT clearFlag = D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL;
			md3dDeviceContext->ClearDepthStencilView(D3D11DriverState.DepthStencilView, clearFlag, depth, stencil);
		}
	}

	void CD3D11Driver::clearDepth(f32 depth)
	{
		if (D3D11DriverState.DepthStencilView)
			md3dDeviceContext->ClearDepthStencilView(D3D11DriverState.DepthStencilView, D3D11_CLEAR_DEPTH, depth, 0);
	}

	void CD3D11Driver::clearStencil(u8 stencil)
	{
		if (D3D11DriverState.DepthStencilView)
			md3dDeviceContext->ClearDepthStencilView(D3D11DriverState.DepthStencilView, D3D11_CLEAR_STENCIL, 1.0f, stencil);
	}

	void CD3D11Driver::clearRenderTarget(const f32 color[])
	{
		if (D3D11DriverState.RenderTargetViews[0])
			md3dDeviceContext->ClearRenderTargetView(D3D11DriverState.RenderTargetViews[0], color);
	}

	void CD3D11Driver::setRenderTarget(IRenderTarget* pRenderTarget)
	{
		// if param is null, set the default render target
		if (!pRenderTarget)
		{
			D3D11DriverState.RenderTargets[0] = nullptr;
			D3D11DriverState.RenderTargetViews[0] = nullptr;
			//md3dDeviceContext->OMSetRenderTargets(0, nullptr, D3D11DriverState.DepthStencilView);
		}
		else
		{
			// there is no need to change
			if (pRenderTarget == D3D11DriverState.RenderTargets[0])
				return;

			D3D11DriverState.RenderTargets[0] = pRenderTarget;
			CD3D11RenderTarget* d3d11RenderTarget = dynamic_cast<CD3D11RenderTarget*>(pRenderTarget);
			D3D11DriverState.RenderTargetViews[0] = d3d11RenderTarget->getRenderTargetView();
		
			// check if the texture has already been bound to a shader.
			ID3D11ShaderResourceView* d3dShaderResourceView = d3d11RenderTarget->getShaderResourceView();

			if (d3dShaderResourceView)
				unbindTextureFromShaders(d3dShaderResourceView);

			// change viewport to adapt new size
			SViewport viewport = mViewport;
			viewport.Width = (f32)pRenderTarget->getWidth();
			viewport.Height = (f32)pRenderTarget->getHeight();
			setViewport(viewport);
		}
		md3dDeviceContext->OMSetRenderTargets(1, &D3D11DriverState.RenderTargetViews[0], D3D11DriverState.DepthStencilView);
	}

	/*
	void CD3D11Driver::setRenderTargets(IRenderTarget* pRenderTargets[], u32 count)
	{
		bool bNeedChange = false;
		const u32 maxTargetCount = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT;
		IRenderTarget* renderTargets[maxTargetCount] = { 0 };
		memcpy(renderTargets, pRenderTargets, sizeof(IRenderTarget*)* count);

		// check whether it needs change.
		for (u32 i = 0; i < maxTargetCount; i++)
		{
			if (D3D11DriverState.RenderTargets[i] != renderTargets[i])
			{
				bNeedChange = true;
				IRenderTarget* pRenderTarget = renderTargets[i];
				D3D11DriverState.RenderTargets[i] = pRenderTarget;
				if (pRenderTarget)
				{
					CD3D11RenderTarget* d3d11RenderTarget = dynamic_cast<CD3D11RenderTarget*>(pRenderTarget);
					D3D11DriverState.RenderTargetViews[i] = d3d11RenderTarget->getRenderTargetView();

					// check if the texture has already been bound to a shader.
					ID3D11ShaderResourceView* d3dShaderResourceView = d3d11RenderTarget->getShaderResourceView();

					if (d3dShaderResourceView)
						unbindTextureFromShaders(d3dShaderResourceView);
				}
				else
				{
					D3D11DriverState.RenderTargetViews[i] = NULL;
				}
			}
		}

		if (bNeedChange)
		{
			// change viewport size
			for (u32 i = 0; i < count; i++)
			{
				if (pRenderTargets[i])
				{
					SViewport viewport = mViewport;
					viewport.Width = (f32)pRenderTargets[i]->getWidth();
					viewport.Height = (f32)pRenderTargets[i]->getHeight();
					setViewport(viewport);
					break;
				}
			}

			md3dDeviceContext->OMSetRenderTargets(maxTargetCount, &D3D11DriverState.RenderTargetViews[0], D3D11DriverState.DepthStencilView);
		}
			
	}
	*/


	void CD3D11Driver::unbindTextureFromShaders(ID3D11ShaderResourceView* d3dShaderResourceView)
	{
		for (u32 i = 0; i < EST_SHADER_COUNT; i++)
		{
			bool bNeedChange = false;
			for (u32 j = 0; j < _GF_SHADER_RESOURCE_VIEW_COUNT; j++)
			{
				if (D3D11DriverState.ShaderResourceViews[i][j] == d3dShaderResourceView)
				{
					bNeedChange = true;
					D3D11DriverState.ShaderResourceViews[i][j] = NULL;
				}
			}

			if (bNeedChange)
			{
				D3D11DriverState.ShaderResourceViewIsDirty[i] = true;
				bindTexture((E_SHADER_TYPE)i, _GF_SHADER_RESOURCE_VIEW_COUNT);
			}
		}
	}

	void CD3D11Driver::setDefaultRenderTarget()
	{
		if (D3D11DriverState.RenderTargetViews[0] != mDefaultRenderTargetView)
		{
			D3D11DriverState.RenderTargets[0] = mDefaultRenderTarget;
			D3D11DriverState.RenderTargetViews[0] = mDefaultRenderTargetView;

			md3dDeviceContext->OMSetRenderTargets(1, &mDefaultRenderTargetView, D3D11DriverState.DepthStencilView);

			// set default viewport size
			SViewport viewport = mViewport;
			viewport.Width = (f32)mBackBufferWidth;
			viewport.Height = (f32)mBackBufferHeight;
			setViewport(viewport);
		}
	}

	IRenderTarget* CD3D11Driver::getRenderTarget(u32 index)
	{
		return D3D11DriverState.RenderTargets[index];
	}

	void CD3D11Driver::setDepthStencilSurface(IDepthStencilSurface* depthStencilSurface)
	{
		if (depthStencilSurface != D3D11DriverState.DepthStencilSurface)
		{
			if (!depthStencilSurface)
			{
				D3D11DriverState.DepthStencilSurface = nullptr;
				D3D11DriverState.DepthStencilView = nullptr;
			}
			else
			{
				D3D11DriverState.DepthStencilSurface = depthStencilSurface;
				CD3D11DepthStencilSurface* d3dDepthStencilSurface = dynamic_cast<CD3D11DepthStencilSurface*>(depthStencilSurface);
				D3D11DriverState.DepthStencilView = d3dDepthStencilSurface->getDepthStencilView();
				ID3D11ShaderResourceView* d3dShaderResourceView = d3dDepthStencilSurface->getShaderResourceView();
				if (d3dShaderResourceView)
					unbindTextureFromShaders(d3dShaderResourceView);
			}

			md3dDeviceContext->OMSetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, 
				&D3D11DriverState.RenderTargetViews[0], D3D11DriverState.DepthStencilView);
		}
	}

	void CD3D11Driver::setRenderTargetAndDepthStencil(IRenderTarget* pRenderTarget, IDepthStencilSurface* pDepthStencilSurface)
	{
		if (pRenderTarget != D3D11DriverState.RenderTargets[0])
		{
			if (!pRenderTarget)
			{
				D3D11DriverState.RenderTargets[0] = nullptr;
				D3D11DriverState.RenderTargetViews[0] = nullptr;
			}
			else
			{
				D3D11DriverState.RenderTargets[0] = pRenderTarget;
				CD3D11RenderTarget* d3d11RenderTarget = dynamic_cast<CD3D11RenderTarget*>(pRenderTarget);
				D3D11DriverState.RenderTargetViews[0] = d3d11RenderTarget->getRenderTargetView();
				ID3D11ShaderResourceView* d3dShaderResourceView = d3d11RenderTarget->getShaderResourceView();

				if (d3dShaderResourceView)
					unbindTextureFromShaders(d3dShaderResourceView);

				// set new viewport size
				// change viewport to adapt new size
				SViewport viewport = mViewport;
				viewport.Width = (f32)pRenderTarget->getWidth();
				viewport.Height = (f32)pRenderTarget->getHeight();
				setViewport(viewport);
			}
		}
		
		if (pDepthStencilSurface != D3D11DriverState.DepthStencilSurface)
		{
			if (!pDepthStencilSurface)
			{
				D3D11DriverState.DepthStencilSurface = nullptr;
				D3D11DriverState.DepthStencilView = nullptr;
			}
			else
			{
				D3D11DriverState.DepthStencilSurface = pDepthStencilSurface;
				CD3D11DepthStencilSurface* d3dDepthStencilSurface = dynamic_cast<CD3D11DepthStencilSurface*>(pDepthStencilSurface);
				D3D11DriverState.DepthStencilView = d3dDepthStencilSurface->getDepthStencilView();
				ID3D11ShaderResourceView* d3dShaderResourceView = d3dDepthStencilSurface->getShaderResourceView();
				if (d3dShaderResourceView)
					unbindTextureFromShaders(d3dShaderResourceView);
			}
		}

		md3dDeviceContext->OMSetRenderTargets(1, &D3D11DriverState.RenderTargetViews[0], D3D11DriverState.DepthStencilView);
	}


	IDepthStencilSurface* CD3D11Driver::getDepthStencilSurface()
	{
		return D3D11DriverState.DepthStencilSurface;
	}

	void CD3D11Driver::setDefaultDepthStencilSurface()
	{
		D3D11DriverState.DepthStencilSurface = mDepthStencilSurface;
		D3D11DriverState.DepthStencilView = mDefaultDepthStencilView;

		CD3D11DepthStencilSurface* d3dDepthStencilSurface = dynamic_cast<CD3D11DepthStencilSurface*>(mDepthStencilSurface);
		D3D11DriverState.DepthStencilView = d3dDepthStencilSurface->getDepthStencilView();
		ID3D11ShaderResourceView* d3dShaderResourceView = d3dDepthStencilSurface->getShaderResourceView();
		if (d3dShaderResourceView)
			unbindTextureFromShaders(d3dShaderResourceView);

		md3dDeviceContext->OMSetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, 
			&D3D11DriverState.RenderTargetViews[0], D3D11DriverState.DepthStencilView);
	}

	void CD3D11Driver::setDefaultRenderTargetAndDepthStencil()
	{
		// set default render target
		//memset(D3D11DriverState.RenderTargets, 0, sizeof(D3D11DriverState.RenderTargets));
		//memset(D3D11DriverState.RenderTargetViews, 0, sizeof(D3D11DriverState.RenderTargetViews));
		D3D11DriverState.RenderTargets[0] = mDefaultRenderTarget;
		D3D11DriverState.RenderTargetViews[0] = mDefaultRenderTargetView;

		// set default depth stencil surface
		CD3D11DepthStencilSurface* d3dDepthStencilSurface = dynamic_cast<CD3D11DepthStencilSurface*>(mDepthStencilSurface);
		D3D11DriverState.DepthStencilView = d3dDepthStencilSurface->getDepthStencilView();
		ID3D11ShaderResourceView* d3dShaderResourceView = d3dDepthStencilSurface->getShaderResourceView();
		if (d3dShaderResourceView)
			unbindTextureFromShaders(d3dShaderResourceView);

		D3D11DriverState.DepthStencilSurface = mDepthStencilSurface;
		D3D11DriverState.DepthStencilView = mDefaultDepthStencilView;

		md3dDeviceContext->OMSetRenderTargets(1, &D3D11DriverState.RenderTargetViews[0], D3D11DriverState.DepthStencilView);

		// set default viewport size
		SViewport viewport = mViewport;
		viewport.Width = (f32)(mBackBufferWidth);
		viewport.Height = (f32)(mBackBufferHeight);
		setViewport(viewport);
	}

	void CD3D11Driver::setMultipleRenderTargets(IRenderTarget* renderTargets[], u32 count)
	{
		if (count > D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT)
			count = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT;
		// check if they are equal to current RTs,
		memset(D3D11DriverState.RenderTargets, 0, sizeof(D3D11DriverState.RenderTargets));
		memset(D3D11DriverState.RenderTargetViews, 0, sizeof(D3D11DriverState.RenderTargetViews));

		std::vector<SViewport> viewports;
		viewports.resize(count);

		for (u32 i = 0; i < count; i++)
		{
			IRenderTarget* pRenderTarget = renderTargets[i];
			D3D11DriverState.RenderTargets[i] = pRenderTarget;
			if (pRenderTarget)
			{
				CD3D11RenderTarget* d3d11RenderTarget = dynamic_cast<CD3D11RenderTarget*>(pRenderTarget);
				D3D11DriverState.RenderTargetViews[i] = d3d11RenderTarget->getRenderTargetView();
				ID3D11ShaderResourceView* d3dShaderResourceView = d3d11RenderTarget->getShaderResourceView();
				if (d3dShaderResourceView)
					unbindTextureFromShaders(d3dShaderResourceView);

				SViewport viewport = mViewport;
				viewport.Width = (f32)pRenderTarget->getWidth();
				viewport.Height = (f32)pRenderTarget->getHeight();
				viewports[i] = viewport;
			}
			else
			{
				SViewport viewport = mViewport;
				viewport.Width = static_cast<f32>(mBackBufferWidth);
				viewport.Height = static_cast<f32>(mBackBufferHeight);
				viewports[i] = viewport;
			}
		}

		md3dDeviceContext->OMSetRenderTargets(count, D3D11DriverState.RenderTargetViews, D3D11DriverState.DepthStencilView);
		setViewports(viewports);
	}

	void  CD3D11Driver::setMultipleRenderTargets(IRenderTarget* renderTargets[], u32 count, IDepthStencilSurface* pDepthStencilSurface)
	{
		if (pDepthStencilSurface != D3D11DriverState.DepthStencilSurface)
		{
			if (!pDepthStencilSurface)
			{
				D3D11DriverState.DepthStencilSurface = nullptr;
				D3D11DriverState.DepthStencilView = nullptr;
			}
			else
			{
				D3D11DriverState.DepthStencilSurface = pDepthStencilSurface;
				CD3D11DepthStencilSurface* d3dDepthStencilSurface = dynamic_cast<CD3D11DepthStencilSurface*>(pDepthStencilSurface);
				D3D11DriverState.DepthStencilView = d3dDepthStencilSurface->getDepthStencilView();
				ID3D11ShaderResourceView* d3dShaderResourceView = d3dDepthStencilSurface->getShaderResourceView();
				if (d3dShaderResourceView)
					unbindTextureFromShaders(d3dShaderResourceView);
			}
		}

		setMultipleRenderTargets(renderTargets, count);
	}

	void CD3D11Driver::setTexture(E_SHADER_TYPE shadertype, u32 slot, ID3D11ShaderResourceView* shaderResourceView)
	{
		if (shaderResourceView != D3D11DriverState.ShaderResourceViews[shadertype][slot])
		{
			D3D11DriverState.ShaderResourceViews[shadertype][slot] = shaderResourceView;
			D3D11DriverState.ShaderResourceViewIsDirty[shadertype] = true;
		}
	}

	void CD3D11Driver::setRWTexture(u32 slot, ID3D11UnorderedAccessView* unorderedAccessView)
	{
		if (unorderedAccessView != D3D11DriverState.UnorderedAccessViews[slot])
		{
			D3D11DriverState.UnorderedAccessViews[slot] = unorderedAccessView;
			D3D11DriverState.UnorderedAccessViewIsDirty = true;
		}
	}

	void CD3D11Driver::setSampler(E_SHADER_TYPE shadertype, u32 slot, ISampler* sampler)
	{
		CD3D11Sampler* d3d11Sampler = dynamic_cast<CD3D11Sampler*>(sampler);
		if (d3d11Sampler->getSamplerState() != D3D11DriverState.SamplerStates[shadertype][slot])
		{
			D3D11DriverState.SamplerStates[shadertype][slot] = d3d11Sampler->getSamplerState();
			D3D11DriverState.SamplerStateIsDirty[shadertype] = true;
		}
	}

	void CD3D11Driver::bindTexture(E_SHADER_TYPE shaderType, u32 textureCount)
	{
		switch (shaderType)
		{
		case EST_VERTEX_SHADER:
			if (D3D11DriverState.ShaderResourceViewIsDirty[EST_VERTEX_SHADER])
			{
				md3dDeviceContext->VSSetShaderResources(0, textureCount, D3D11DriverState.ShaderResourceViews[EST_VERTEX_SHADER]);
				D3D11DriverState.ShaderResourceViewIsDirty[EST_VERTEX_SHADER] = false;
			}
			break;
		case EST_HULL_SHADER:
			if (D3D11DriverState.ShaderResourceViewIsDirty[EST_HULL_SHADER])
			{
				md3dDeviceContext->HSSetShaderResources(0, textureCount, D3D11DriverState.ShaderResourceViews[EST_HULL_SHADER]);
				D3D11DriverState.ShaderResourceViewIsDirty[EST_HULL_SHADER] = false;
			}
			break;
		case EST_DOMAIN_SHADER:
			if (D3D11DriverState.ShaderResourceViewIsDirty[EST_DOMAIN_SHADER])
			{
				md3dDeviceContext->DSSetShaderResources(0, textureCount, D3D11DriverState.ShaderResourceViews[EST_DOMAIN_SHADER]);
				D3D11DriverState.ShaderResourceViewIsDirty[EST_DOMAIN_SHADER] = false;
			}
			break;
		case EST_GEOMETRY_SHADER:
			if (D3D11DriverState.ShaderResourceViewIsDirty[EST_GEOMETRY_SHADER])
			{
				md3dDeviceContext->GSSetShaderResources(0, textureCount, D3D11DriverState.ShaderResourceViews[EST_GEOMETRY_SHADER]);
				D3D11DriverState.ShaderResourceViewIsDirty[EST_GEOMETRY_SHADER] = false;
			}
			break;
		case EST_PIXEL_SHADER:
			if (D3D11DriverState.ShaderResourceViewIsDirty[EST_PIXEL_SHADER])
			{
				md3dDeviceContext->PSSetShaderResources(0, textureCount, D3D11DriverState.ShaderResourceViews[EST_PIXEL_SHADER]);
				D3D11DriverState.ShaderResourceViewIsDirty[EST_PIXEL_SHADER] = false;
			}
			break;
		case EST_COMPUTE_SHADER:
			if (D3D11DriverState.ShaderResourceViewIsDirty[EST_COMPUTE_SHADER])
			{
				md3dDeviceContext->CSSetShaderResources(0, textureCount, D3D11DriverState.ShaderResourceViews[EST_COMPUTE_SHADER]);
				D3D11DriverState.ShaderResourceViewIsDirty[EST_COMPUTE_SHADER] = false;
			}
			break;
		}
	}

	void CD3D11Driver::bindSampler(E_SHADER_TYPE shaderType, u32 samplerCount)
	{
		switch (shaderType)
		{
		case EST_VERTEX_SHADER:
			if (D3D11DriverState.SamplerStateIsDirty[EST_VERTEX_SHADER])
			{
				md3dDeviceContext->VSSetSamplers(0, samplerCount, D3D11DriverState.SamplerStates[EST_VERTEX_SHADER]);
				D3D11DriverState.SamplerStateIsDirty[EST_VERTEX_SHADER] = false;
			}
			break;
		case EST_HULL_SHADER:
			if (D3D11DriverState.SamplerStateIsDirty[EST_HULL_SHADER])
			{
				md3dDeviceContext->HSSetSamplers(0, samplerCount, D3D11DriverState.SamplerStates[EST_HULL_SHADER]);
				D3D11DriverState.SamplerStateIsDirty[EST_HULL_SHADER] = false;
			}
			break;
		case EST_DOMAIN_SHADER:
			if (D3D11DriverState.SamplerStateIsDirty[EST_DOMAIN_SHADER])
			{
				md3dDeviceContext->DSSetSamplers(0, samplerCount, D3D11DriverState.SamplerStates[EST_DOMAIN_SHADER]);
				D3D11DriverState.SamplerStateIsDirty[EST_DOMAIN_SHADER] = false;
			}
			break;
		case EST_GEOMETRY_SHADER:
			if (D3D11DriverState.SamplerStateIsDirty[EST_GEOMETRY_SHADER])
			{
				md3dDeviceContext->GSSetSamplers(0, samplerCount, D3D11DriverState.SamplerStates[EST_GEOMETRY_SHADER]);
				D3D11DriverState.SamplerStateIsDirty[EST_GEOMETRY_SHADER] = false;
			}
			break;
		case EST_PIXEL_SHADER:
			if (D3D11DriverState.SamplerStateIsDirty[EST_PIXEL_SHADER])
			{
				md3dDeviceContext->PSSetSamplers(0, samplerCount, D3D11DriverState.SamplerStates[EST_PIXEL_SHADER]);
				D3D11DriverState.SamplerStateIsDirty[EST_PIXEL_SHADER] = false;
			}
			break;
		case EST_COMPUTE_SHADER:
			if (D3D11DriverState.SamplerStateIsDirty[EST_COMPUTE_SHADER])
			{
				md3dDeviceContext->CSSetSamplers(0, samplerCount, D3D11DriverState.SamplerStates[EST_COMPUTE_SHADER]);
				D3D11DriverState.SamplerStateIsDirty[EST_COMPUTE_SHADER] = false;
			}
			break;
		}
	}

	void CD3D11Driver::bindRWTexture(u32 count)
	{
		if (D3D11DriverState.UnorderedAccessViewIsDirty)
		{
			md3dDeviceContext->CSSetUnorderedAccessViews(0, count, D3D11DriverState.UnorderedAccessViews, 0);
			D3D11DriverState.UnorderedAccessViewIsDirty = false;
		}
	}

	void CD3D11Driver::clearShader(E_SHADER_TYPE shaderType)
	{
		switch (shaderType)
		{
		case EST_VERTEX_SHADER:   md3dDeviceContext->VSSetShader(NULL, 0, 0); break;
		case EST_PIXEL_SHADER:    md3dDeviceContext->PSSetShader(NULL, 0, 0); break;
		case EST_GEOMETRY_SHADER: md3dDeviceContext->GSSetShader(NULL, 0, 0); break;
		case EST_HULL_SHADER:	  md3dDeviceContext->HSSetShader(NULL, 0, 0); break;
		case EST_DOMAIN_SHADER:   md3dDeviceContext->DSSetShader(NULL, 0, 0); break;
		case EST_COMPUTE_SHADER:  md3dDeviceContext->CSSetShader(NULL, 0, 0); break;
		}
	}

	void CD3D11Driver::setViewport(const SViewport& viewport)
	{
		// if equals with the old.
		if (mViewport == viewport)
			return;

		mViewport = viewport;
		D3D11_VIEWPORT d3d11Viewport;
		d3d11Viewport.Width = viewport.Width;
		d3d11Viewport.Height = viewport.Height;
		d3d11Viewport.TopLeftX = viewport.TopLeftX;
		d3d11Viewport.TopLeftY = viewport.TopLeftY;
		d3d11Viewport.MinDepth = viewport.MinDepth;
		d3d11Viewport.MaxDepth = viewport.MaxDepth;

		md3dDeviceContext->RSSetViewports(1, &d3d11Viewport);
	}

	void CD3D11Driver::setViewport(f32 topLeftX, f32 topLeftY, f32 width, f32 height, f32 minDepth, f32 maxDepth)
	{
		SViewport viewport;
		viewport.Width = width;
		viewport.Height = height;
		viewport.TopLeftX = topLeftX;
		viewport.TopLeftY = topLeftY;
		viewport.MinDepth = minDepth;
		viewport.MaxDepth = maxDepth;

		setViewport(viewport);
	}

	void CD3D11Driver::setViewports(const std::vector<SViewport>& viewports)
	{
		u32 count = viewports.size();
		if (count > D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT)
			count = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT;

		mViewport = viewports[0];
		D3D11_VIEWPORT d3d11Viewports[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];
		for (u32 i = 0; i < count; i++)
		{
			d3d11Viewports[i].Width = viewports[i].Width;
			d3d11Viewports[i].Height = viewports[i].Height;
			d3d11Viewports[i].TopLeftX = viewports[i].TopLeftX;
			d3d11Viewports[i].TopLeftY = viewports[i].TopLeftY;
			d3d11Viewports[i].MinDepth = viewports[i].MinDepth;
			d3d11Viewports[i].MaxDepth = viewports[i].MaxDepth;
		}

		md3dDeviceContext->RSSetViewports(count, d3d11Viewports);
	}

	/*
	void CD3D11Driver::setRenderingShadowMap(bool b)
	{
		mRenderingShadowMap = b;
		if (mRenderingShadowMap)
		{
			md3dDeviceContext->RSSetState(mShadowMapRasterizeState);
			D3D11DriverState.RasterizerState = mShadowMapRasterizeState;
		}
		D3D11DriverState.Pipeline = nullptr;
	}
	*/

	void CD3D11Driver::setPipelineUsage(E_PIPELINE_USAGE usage)
	{
		mCurrentPipelineUsage = usage;
		D3D11DriverState.Pipeline = nullptr;
	}

	bool CD3D11Driver::setupGBuffer()
	{
		if (mGBuffers[0])
			return false;

		ITextureManager* textureManager = ITextureManager::getInstance();

		std::string textureNames[] = { "gbuffer0", "gbuffer1", "gbuffer2", "gbuffer3" };
		

		E_GI_FORMAT formats[] = { EGF_R32G32B32A32_FLOAT, EGF_R8G8B8A8_UNORM, EGF_R8G8B8A8_UNORM, EGF_R8G8B8A8_UNORM };

		for (u32 i = 0; i < EGT_GBUFFER_COUNT; i++)
		{
			IRenderTarget* pTarget = textureManager->createRenderTarget(textureNames[i], 0, 0, formats[i]);
			assert("set up gbuffer failed!");
			mGBuffers[i] = pTarget;
		}
		return true;
	}
	
	void CD3D11Driver::setGBuffersAsRenderTargets()
	{
		//setMultipleRenderTargets()
	}

	void CD3D11Driver::getGBuffers(IRenderTarget* renderTargets[])
	{
		setupGBuffer();
		memcpy(renderTargets, mGBuffers, EGT_GBUFFER_COUNT * sizeof(IRenderTarget*));
	}

	bool CD3D11Driver::runComputeShader(IShader* shader, u32 x, u32 y, u32 z,
		ISceneNode* node)
	{
		shader->reset();

		if (!shader || shader->getType() != EST_COMPUTE_SHADER)
			return false;

		// following actions must be done before calling this method
		// 1, setTexture or setRWTexture
		// 2, setVariable, except for auto variable.

		CShaderVariableInjection::injectToComputeShader(shader, node);
		
		// update constant variables
		shader->update();
		
		// update samplers
		shader->applySamplers();

		bindTexture(EST_COMPUTE_SHADER, shader->getTextureCount());
		bindRWTexture(shader->getRWTextureCount());
		bindSampler(EST_COMPUTE_SHADER, shader->getSamplerCount());

		shader->submit();

		md3dDeviceContext->Dispatch(x, y, z);

		return true;
	}

	void CD3D11Driver::resetRWTextures()
	{
		u32 maxPoint = 0;
		for (u32 i = 0; i < _GF_UNORDERED_ACCESS_VIEW_COUNT; i++)
		{
			if (D3D11DriverState.UnorderedAccessViews[i])
			{
				maxPoint = i + 1;
				D3D11DriverState.UnorderedAccessViews[i] = NULL;
			}
		}

		if (maxPoint > 0)
		{
			md3dDeviceContext->CSSetUnorderedAccessViews(0, maxPoint, D3D11DriverState.UnorderedAccessViews, NULL);
		}
		D3D11DriverState.UnorderedAccessViewIsDirty = false;
	}

	void CD3D11Driver::resetTextures(E_SHADER_TYPE shaderType)
	{
		if (shaderType == EST_UNDEFINED_SHADER)
		{
			resetTextures(EST_VERTEX_SHADER);
			resetTextures(EST_PIXEL_SHADER);
			resetTextures(EST_HULL_SHADER);
			resetTextures(EST_DOMAIN_SHADER);
			resetTextures(EST_GEOMETRY_SHADER);
			resetTextures(EST_COMPUTE_SHADER);
			return;
		}

		for (u32 i = 0; i < _GF_SHADER_RESOURCE_VIEW_COUNT; i++)
			D3D11DriverState.ShaderResourceViews[shaderType][i] = NULL;
		//memcpy(D3D11DriverState.ShaderResourceViews[shaderType], 0, sizeof(D3D11DriverState.ShaderResourceViews[shaderType]));
		D3D11DriverState.ShaderResourceViewIsDirty[shaderType] = false;

		switch (shaderType)
		{
		case EST_VERTEX_SHADER:
			md3dDeviceContext->VSSetShaderResources(0, _GF_SHADER_RESOURCE_VIEW_COUNT, D3D11DriverState.ShaderResourceViews[shaderType]);
			break;
		case EST_GEOMETRY_SHADER:
			md3dDeviceContext->GSSetShaderResources(0, _GF_SHADER_RESOURCE_VIEW_COUNT, D3D11DriverState.ShaderResourceViews[shaderType]);
			break;
		case EST_HULL_SHADER:
			md3dDeviceContext->HSSetShaderResources(0, _GF_SHADER_RESOURCE_VIEW_COUNT, D3D11DriverState.ShaderResourceViews[shaderType]);
			break;
		case EST_DOMAIN_SHADER:
			md3dDeviceContext->DSSetShaderResources(0, _GF_SHADER_RESOURCE_VIEW_COUNT, D3D11DriverState.ShaderResourceViews[shaderType]);
			break;
		case EST_PIXEL_SHADER:
			md3dDeviceContext->PSSetShaderResources(0, _GF_SHADER_RESOURCE_VIEW_COUNT, D3D11DriverState.ShaderResourceViews[shaderType]);
			break;
		case EST_COMPUTE_SHADER:
			md3dDeviceContext->CSSetShaderResources(0, _GF_SHADER_RESOURCE_VIEW_COUNT, D3D11DriverState.ShaderResourceViews[shaderType]);
			break;
		default:
			break;
		}
	}
}
