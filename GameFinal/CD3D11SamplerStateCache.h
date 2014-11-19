#ifndef __D3D11_SAMPLER_STATE_CACHE_CLASS_H__
#define __D3D11_SAMPLER_STATE_CACHE_CLASS_H__

#include "IReferenceCounted.h"
#include "CSortCodeAllocator.h"
#include "gfMath.h"
#include "D3DUtil.h"

namespace gf
{

	struct SD3D11SamplerStateWrapper
	{
		u32							SortCode;
		u32							HashCode;
		ID3D11SamplerState*			SamplerState;
	};


	class CD3D11SamplerStateCache : public IReferenceCounted
	{
	public:
		CD3D11SamplerStateCache(ID3D11Device* pd3ddevice)
			:md3dDevice(pd3ddevice)
		{

		}

		virtual ~CD3D11SamplerStateCache()
		{
			//std::map<u32, std::list<SD3D11SamplerStateWrapper>>		mSamplerStateWrappers;
			for (auto it = mSamplerStateWrappers.begin(); it != mSamplerStateWrappers.end(); it++)
			{
				std::list<SD3D11SamplerStateWrapper>& wrappers = it->second;
				for (auto sit = wrappers.begin(); sit != wrappers.end(); sit++)
				{
					ID3D11SamplerState* samplerState = sit->SamplerState;
					ReleaseCOM(samplerState);
				}
			}
		}

		bool getSamplerState(const D3D11_SAMPLER_DESC& desc, SD3D11SamplerStateWrapper& wrapper, bool addIfNotFound = true)
		{
			// calcuate the hash code according to the desc.
			u32 code = getSamplerStateHashCode(desc);
			auto it = mSamplerStateWrappers.find(code);

			//if not have sampler state with same hash code in the cache.
			if (it == mSamplerStateWrappers.end())
			{
				if (!addIfNotFound)
					return false;

				std::list<SD3D11SamplerStateWrapper> stateList;
				wrapper.HashCode = code;
				wrapper.SortCode = mSamplerStateCodeAllocator.allocate();
				md3dDevice->CreateSamplerState(&desc, &wrapper.SamplerState);
				stateList.push_back(wrapper);
				mSamplerStateWrappers.insert(std::make_pair(code, stateList));
				return true;
			}

			// if have rasterizer state with the same code,
			// then search whether the state really equals.
			auto stateIt = it->second.begin();
			for (; stateIt != it->second.end(); stateIt++)
			{
				// if the two raster state really equals.
				// which means they have the same descs.
				if (equalsWithSameCode(desc, stateIt->SamplerState))
				{
					wrapper = *stateIt;
					return true;
				}
			}

			// if not found, just create it, then add it to the list.
			wrapper.HashCode = code;
			wrapper.SortCode = mSamplerStateCodeAllocator.allocate();
			md3dDevice->CreateSamplerState(&desc, &wrapper.SamplerState);
			it->second.push_back(wrapper);

			return true;

		}

	private:
		u32 getSamplerStateHashCode(const D3D11_SAMPLER_DESC& desc) const
		{
			/*
			|  AddressU | AddressV | AddressW | Filter |
			|    3      |     3    |     3    |   16   |
			*/

			return (desc.AddressU << 22) | (desc.AddressV << 19) | (desc.AddressW << 16) | desc.Filter;
		}

		bool equalsWithSameCode(const D3D11_SAMPLER_DESC& desc, ID3D11SamplerState* pd3dSamplerState)
		{
			D3D11_SAMPLER_DESC desc2;
			pd3dSamplerState->GetDesc(&desc2);

			if (desc.ComparisonFunc == desc2.ComparisonFunc
				&& desc.MaxAnisotropy == desc2.MaxAnisotropy
				&& math::FloatEqual(desc.MinLOD, desc2.MinLOD)
				&& math::FloatEqual(desc.MaxLOD, desc2.MaxLOD)
				&& math::FloatArrayEqual(desc.BorderColor, desc2.BorderColor))
			{
				return true;
			}
			return false;
		}

		std::map<u32, std::list<SD3D11SamplerStateWrapper>>		mSamplerStateWrappers;
		CSortCodeAllocator<255>									mSamplerStateCodeAllocator;
		ID3D11Device*											md3dDevice;
	};

}

#endif
