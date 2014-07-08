#include "stdafx.h"
#include "CD3D11Driver.h"
#include "CD3D11SimpleMesh.h"
#include "CD3D11ResourceFactory.h"
#include "CD3D11Texture.h"
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

namespace gf
{
	CD3D11Driver::CD3D11Driver(IDevice* device)
		:mDevice(device)
	{

	}



	bool CD3D11Driver::init(SCreationParameters& createParam)
	{
		HRESULT result;
		IDXGIFactory* factory;
		IDXGIAdapter* adapter;
		IDXGIOutput* adapterOutput;
		unsigned int numModes, i, numerator, denominator, stringLength;
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
			if (displayModeList[i].Width == (unsigned int)createParam.ClientWidth)
			{
				if (displayModeList[i].Height == (unsigned int)createParam.ClientHeight)
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
		swapChainDesc.BufferDesc.Width = createParam.ClientWidth;
		swapChainDesc.BufferDesc.Height = createParam.ClientHeight;

		// Set regular 32-bit surface for the back buffer.
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		// Set the refresh rate of the back buffer.
		if (createParam.VsyncEnabled)
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
		if (createParam.MultiSamplingQuality == 0)
		{
			swapChainDesc.SampleDesc.Count = 1;
			swapChainDesc.SampleDesc.Quality = 0;

			createParam.MultiSamplingCount = 1;
		}
		else
		{
			swapChainDesc.SampleDesc.Count = createParam.MultiSamplingCount;
			UINT numQualityLevels;
			md3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &numQualityLevels);

			swapChainDesc.SampleDesc.Quality = min(numQualityLevels - 1, createParam.MultiSamplingQuality);

			createParam.MultiSamplingQuality = swapChainDesc.SampleDesc.Quality;
		}
		
		// set member attributes of class


		// Set to a single back buffer.
		swapChainDesc.BufferCount = 1;

		// Set the usage of the back buffer.
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

		// Set the handle for the window to render to.
		swapChainDesc.OutputWindow = (HWND)createParam.WindowHandle;

		// Set to full screen or windowed mode.
		if (createParam.WindowStyle & EWS_FULLSCREEN)
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


		// Initialize the description of the depth buffer.
		ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

		// Set up the description of the depth buffer.
		/*
		depthBufferDesc.Width = createParam.ClientWidth;
		depthBufferDesc.Height = createParam.ClientHeight;
		depthBufferDesc.MipLevels = 1;
		depthBufferDesc.ArraySize = 1;
		depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthBufferDesc.SampleDesc.Count = swapChainDesc.SampleDesc.Count;
		depthBufferDesc.SampleDesc.Quality = swapChainDesc.SampleDesc.Quality;
		depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthBufferDesc.CPUAccessFlags = 0;
		depthBufferDesc.MiscFlags = 0;

		// Create the texture for the depth buffer using the filled out description.
		result = md3dDevice->CreateTexture2D(&depthBufferDesc, NULL, &mDepthStencilBuffer);
		md3dDevice->CreateDepthStencilView(mDepthStencilBuffer, 0, &mDepthStencilView);

		// Initialize the description of the stencil state.
		ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

		// Set up the description of the stencil state.
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

		depthStencilDesc.StencilEnable = true;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;

		// Stencil operations if pixel is front-facing.
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Stencil operations if pixel is back-facing.
		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Create the depth stencil state.
		result = md3dDevice->CreateDepthStencilState(&depthStencilDesc, &mDepthStencilState);
		if (FAILED(result))
		{
			return false;
		}

		// Set the depth stencil state.
		md3dDeviceContext->OMSetDepthStencilState(mDepthStencilState, 1);

//		md3dDeviceContext->OMSetRenderTargets(1, &mDefaultRenderTargetView, mDepthStencilView);


		md3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		*/

		// Setup the viewport for rendering.
		setViewport(0, 0, static_cast<f32>(mDevice->getClientWidth()), static_cast<f32>(mDevice->getClientHeight()));

		//create resource factory
		mResourceFactory = new CD3D11ResourceFactory(
			md3dDevice, md3dDeviceContext, this);

		//create geometry creator
		mGeometryCreator = new CGeometryCreator;

		//create material manager
		mMaterialManager = new CMaterialManager;

		// create shadermanager
		mShaderManager = new CShaderManager(mResourceFactory);

		// create inputlayout manager
		mInputLayoutManager = new CInputLayoutManager(mResourceFactory);

		//create pipeline manager
		mPipeManager = new CPipelineManager(mResourceFactory);

		//create texture manager
		mTextureManager = new CTextureManager(mDevice, mResourceFactory);

		//create render state manager
		mRenderStateManager = new CRenderStateManager(mResourceFactory);

		//create mesh manager
		mMeshManager = new CMeshManager(mResourceFactory, mGeometryCreator, mTextureManager);

		//create sampler manager
		mSamplerManager = new CSamplerManager(mResourceFactory);

		//create resource group manager
		mResourceGroupManager = new CResourceGroupManager(mTextureManager, mShaderManager,
			mInputLayoutManager, mRenderStateManager, mPipeManager, mMaterialManager, mMeshManager, mSamplerManager);

		mResourceFactory->setResourceGroupManager(mResourceGroupManager);

		// create default depth-stencil-buffer
		bool multiSampling = (createParam.MultiSamplingCount > 1);

		mDepthStencilSurface = mTextureManager->createDepthStencilSurface("_default_depth_stencil_surface", 0, 0, createParam.DepthBits, createParam.StencilBits,
			multiSampling, createParam.MultiSamplingCount, createParam.MultiSamplingQuality, false);

		if (!mDepthStencilSurface)
		{
			GF_PRINT_CONSOLE_INFO("Depth Stencil Surface has failed to be created.\n");
			return false;
		}

		D3D11DriverState.DepthStencilSurface = mDepthStencilSurface;
		CD3D11DepthStencilSurface* d3dDepthStencilSurface = dynamic_cast<CD3D11DepthStencilSurface*>(mDepthStencilSurface);
		mDefaultDepthStencilView = d3dDepthStencilSurface->getDepthStencilView();
		D3D11DriverState.DepthStencilView = mDefaultDepthStencilView;

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

		ReleaseReferenceCounted(mMeshManager);
		ReleaseReferenceCounted(mMaterialManager);
		ReleaseReferenceCounted(mPipeManager);
		ReleaseReferenceCounted(mTextureManager);
		ReleaseReferenceCounted(mShaderManager);
		ReleaseReferenceCounted(mInputLayoutManager);
		ReleaseReferenceCounted(mRenderStateManager);

		ReleaseReferenceCounted(mResourceFactory);
		ReleaseReferenceCounted(mGeometryCreator);
	}

	void CD3D11Driver::beginScene(
		bool zBuffer,
		bool stencilBuffer,
		const f32 clearColor[],
		f32 depthValue,
		u8 stencilValue)
	{
		//md3dDeviceContext->ClearRenderTargetView(mDefaultRenderTargetView, clearColor);

		UINT clearFlag = 0;
		if (zBuffer) clearFlag |= D3D11_CLEAR_DEPTH;
		if (stencilBuffer) clearFlag |= D3D11_CLEAR_STENCIL;

		//md3dDeviceContext->ClearDepthStencilView(mDepthStencilView, clearFlag, depthValue, stencilValue);

		D3D11DriverState.Reset();

		setDefaultRenderTargetAndDepthStencil();

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
		if (D3D11DriverState.RenderTargetView)
			md3dDeviceContext->ClearRenderTargetView(D3D11DriverState.RenderTargetView, color);
	}

	void CD3D11Driver::setRenderTarget(IRenderTarget* pRenderTarget)
	{
		// if param is null, set the default render target
		if (!pRenderTarget)
		{
			D3D11DriverState.RenderTarget = nullptr;
			D3D11DriverState.RenderTargetView = nullptr;
			//md3dDeviceContext->OMSetRenderTargets(0, nullptr, D3D11DriverState.DepthStencilView);
		}
		else
		{
			// there is no need to change
			if (pRenderTarget == D3D11DriverState.RenderTarget)
				return;

			D3D11DriverState.RenderTarget = pRenderTarget;
			CD3D11RenderTarget* d3d11RenderTarget = dynamic_cast<CD3D11RenderTarget*>(pRenderTarget);
			D3D11DriverState.RenderTargetView = d3d11RenderTarget->getRenderTargetView();
		
			// check if the texture has already been bound to a shader.
			ID3D11ShaderResourceView* d3dShaderResourceView = d3d11RenderTarget->getShaderResourceView();

			if (d3dShaderResourceView)
				unbindTextureFromShaders(d3dShaderResourceView);
		}
		md3dDeviceContext->OMSetRenderTargets(1, &D3D11DriverState.RenderTargetView, D3D11DriverState.DepthStencilView);
	}

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
		if (D3D11DriverState.RenderTargetView != mDefaultRenderTargetView)
		{
			D3D11DriverState.RenderTarget = nullptr;
			D3D11DriverState.RenderTargetView = mDefaultRenderTargetView;
			md3dDeviceContext->OMSetRenderTargets(1, &mDefaultRenderTargetView, D3D11DriverState.DepthStencilView);
		}
	}

	IRenderTarget* CD3D11Driver::getRenderTarget()
	{
		return D3D11DriverState.RenderTarget;
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

			md3dDeviceContext->OMSetRenderTargets(1, &D3D11DriverState.RenderTargetView, D3D11DriverState.DepthStencilView);
		}
	}

	void CD3D11Driver::setRenderTargetAndDepthStencil(IRenderTarget* pRenderTarget, IDepthStencilSurface* pDepthStencilSurface)
	{
		if (pRenderTarget != D3D11DriverState.RenderTarget)
		{
			if (!pRenderTarget)
			{
				D3D11DriverState.RenderTarget = nullptr;
				D3D11DriverState.RenderTargetView = nullptr;
			}
			else
			{
				D3D11DriverState.RenderTarget = pRenderTarget;
				CD3D11RenderTarget* d3d11RenderTarget = dynamic_cast<CD3D11RenderTarget*>(pRenderTarget);
				D3D11DriverState.RenderTargetView = d3d11RenderTarget->getRenderTargetView();
				ID3D11ShaderResourceView* d3dShaderResourceView = d3d11RenderTarget->getShaderResourceView();

				if (d3dShaderResourceView)
					unbindTextureFromShaders(d3dShaderResourceView);
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

		md3dDeviceContext->OMSetRenderTargets(1, &D3D11DriverState.RenderTargetView, D3D11DriverState.DepthStencilView);
	}


	IDepthStencilSurface* CD3D11Driver::getDepthStencilSurface()
	{
		return D3D11DriverState.DepthStencilSurface;
	}

	void CD3D11Driver::setDefaultDepthStencilSurface()
	{
		D3D11DriverState.DepthStencilSurface = mDepthStencilSurface;
		D3D11DriverState.DepthStencilView = mDefaultDepthStencilView;

		md3dDeviceContext->OMSetRenderTargets(1, &D3D11DriverState.RenderTargetView, D3D11DriverState.DepthStencilView);
	}

	void CD3D11Driver::setDefaultRenderTargetAndDepthStencil()
	{
		// set default render target
		D3D11DriverState.RenderTarget = nullptr;
		D3D11DriverState.RenderTargetView = mDefaultRenderTargetView;

		// set default depth stencil surface
		D3D11DriverState.DepthStencilSurface = mDepthStencilSurface;
		D3D11DriverState.DepthStencilView = mDefaultDepthStencilView;

		md3dDeviceContext->OMSetRenderTargets(1, &D3D11DriverState.RenderTargetView, D3D11DriverState.DepthStencilView);
	}


	void CD3D11Driver::setTexture(E_SHADER_TYPE shadertype, u32 slot, ITexture* texture)
	{
		switch (texture->getType())
		{
		case ETT_TEXTURE_2D:
			{
				CD3D11Texture*	d3d11Texture = dynamic_cast<CD3D11Texture*>(texture);
				if (d3d11Texture->getShaderResourceView() != D3D11DriverState.ShaderResourceViews[shadertype][slot])
				{
					D3D11DriverState.ShaderResourceViews[shadertype][slot] = d3d11Texture->getShaderResourceView();
					D3D11DriverState.ShaderResourceViewIsDirty[shadertype] = true;
				}
			}
			break;
		case ETT_RENDER_TARGET:
			{
				CD3D11RenderTarget* d3d11RenderTarget = dynamic_cast<CD3D11RenderTarget*>(texture);
				if (d3d11RenderTarget->getShaderResourceView() != D3D11DriverState.ShaderResourceViews[shadertype][slot])
				{
					D3D11DriverState.ShaderResourceViews[shadertype][slot] = d3d11RenderTarget->getShaderResourceView();
					D3D11DriverState.ShaderResourceViewIsDirty[shadertype] = true;
				}
			}
			break;
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
		}
	}

	void CD3D11Driver::setViewport(const SViewport& viewport)
	{
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
}
