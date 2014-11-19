#include "stdafx.h"
#include "D3DUtil.h"

#include "CD3D11RenderStateCache.h"

namespace gf
{
	CD3D11RenderStateCache::~CD3D11RenderStateCache()
	{
		for (auto it = mRasterizerStateWrappers.begin(); it != mRasterizerStateWrappers.end(); it++)
		{
			std::list<SD3D11RasterizerStateWrapper>& wrappers = it->second;
			for (auto sit = wrappers.begin(); sit != wrappers.end(); sit++)
			{
				ID3D11RasterizerState* rasterState = sit->RasterizerState;
				ReleaseCOM(rasterState);
			}
		}

		for (auto it = mBlendStateWrappers.begin(); it != mBlendStateWrappers.end(); it++)
		{
			std::list<SD3D11BlendStateWrapper>& wrappers = it->second;
			for (auto sit = wrappers.begin(); sit != wrappers.end(); sit++)
			{
				ID3D11BlendState* blendState = sit->BlendState;
				ReleaseCOM(blendState);
			}
		}

		for (auto it = mDepthStencilStateWrappers.begin(); it != mDepthStencilStateWrappers.end(); it++)
		{
			std::list<SD3D11DepthStencilStateWrapper>& wrappers = it->second;
			for (auto sit = wrappers.begin(); sit != wrappers.end(); sit++)
			{
				ID3D11DepthStencilState* depthStencilState = sit->DepthStencilState;
				ReleaseCOM(depthStencilState);
			}
		}
	}

}


