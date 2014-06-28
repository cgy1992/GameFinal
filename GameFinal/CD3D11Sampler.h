#ifndef __D3D11SAMPLER_CLASS_H__
#define __D3D11SAMPLER_CLASS_H__

#include "ISampler.h"
#include "CD3D11SamplerStateCache.h"
namespace gf
{
	class CD3D11Sampler : public ISampler
	{
	public:
		CD3D11Sampler(
			const std::string& name,
			CD3D11SamplerStateCache* pSamplerStateCache,
			const D3D11_SAMPLER_DESC& desc)
			:ISampler(name)
			, mSamplerStateCache(pSamplerStateCache)
		{
			setSamplerState(desc);
		}

		virtual void setAddressV(E_ADDRESS_MODE mode)
		{
			D3D11_SAMPLER_DESC desc;
			md3dSamplerState->GetDesc(&desc);
			desc.AddressV = (D3D11_TEXTURE_ADDRESS_MODE)mode;
			setSamplerState(desc);
		}

		virtual void setAddressU(E_ADDRESS_MODE mode)
		{
			D3D11_SAMPLER_DESC desc;
			md3dSamplerState->GetDesc(&desc);
			desc.AddressU = (D3D11_TEXTURE_ADDRESS_MODE)mode;
			setSamplerState(desc);
		}

		virtual void setAddressW(E_ADDRESS_MODE mode)
		{
			D3D11_SAMPLER_DESC desc;
			md3dSamplerState->GetDesc(&desc);
			desc.AddressW = (D3D11_TEXTURE_ADDRESS_MODE)mode;
			setSamplerState(desc);
		}

		virtual void setFilter(E_SAMPLER_FILTER mode)
		{
			D3D11_SAMPLER_DESC desc;
			md3dSamplerState->GetDesc(&desc);
			desc.Filter = (D3D11_FILTER)mode;
			setSamplerState(desc);
		}

		virtual void setBorderColor(const f32 rgba[])
		{
			D3D11_SAMPLER_DESC desc;
			md3dSamplerState->GetDesc(&desc);
			memcpy(desc.BorderColor, rgba, sizeof(f32)* 4);
			setSamplerState(desc);
		}

		ID3D11SamplerState* getSamplerState()
		{
			return md3dSamplerState;
		}

		virtual u32 getSortCode() const
		{
			return mSortCode;
		}

	private:
		void setSamplerState(const D3D11_SAMPLER_DESC& desc)
		{
			SD3D11SamplerStateWrapper wrapper;
			mSamplerStateCache->getSamplerState(desc, wrapper);
			mSortCode = wrapper.SortCode;
			md3dSamplerState = wrapper.SamplerState;
		}

		CD3D11SamplerStateCache*	mSamplerStateCache;
		ID3D11SamplerState*			md3dSamplerState;
		u32							mSortCode;
	};
}

#endif
