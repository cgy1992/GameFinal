#include "stdafx.h"
#include "CD3D11Buffer.h"
#include "D3DUtil.h"

namespace gf
{
	CD3D11Buffer::CD3D11Buffer(ID3D11Device* pd3dDevice,
		ID3D11DeviceContext* pd3dDeviceContext,
		const std::string& name, u32 sortCode,
		CD3D11Driver* d3d11Driver)
		:IBuffer(name, sortCode)
		, md3dDevice(pd3dDevice)
		, md3dDeviceContext(pd3dDeviceContext)
		, md3dSRV(NULL)
		, md3dUAV(NULL)
		, md3d11Driver(d3d11Driver)
		, mBindFlags(0)
		, md3dBuffer(NULL)
	{

	}

	bool CD3D11Buffer::create(u32 elementNum,
		u32 bindFlags,
		E_GI_FORMAT format,
		u32 elementStride,
		void* rawData,
		E_MEMORY_USAGE memoryUsage)
	{
		HRESULT hr;
		ID3D11Buffer* pd3dBuffer = NULL;
		ID3D11ShaderResourceView* pd3dSRV = NULL;
		ID3D11UnorderedAccessView* pd3dUAV = NULL;

		D3D11_BUFFER_DESC bufDesc;
		bufDesc.Usage = getD3d11Usage(memoryUsage);
		bufDesc.BindFlags = getD3dx11BindFlags(bindFlags);
		bufDesc.CPUAccessFlags = getD3dx11CpuAccessFlag(bindFlags);
		

		if (format == EGF_UNKNOWN)
		{
			bufDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			bufDesc.StructureByteStride = elementStride;
		}
		else
		{
			bufDesc.MiscFlags = 0;
			bufDesc.StructureByteStride = 0;
			elementStride = getFormatOffset(format);
		}

		bufDesc.ByteWidth = elementStride * elementNum;

		if (memoryUsage == EMU_UNKNOWN)
		{
			if ((bindFlags & ETBT_CPU_ACCESS_READ))
				bufDesc.Usage = D3D11_USAGE_STAGING;
			else if (bindFlags & ETBT_CPU_ACCESS_WRITE)
				bufDesc.Usage = D3D11_USAGE_DEFAULT;
			else if (rawData)
				bufDesc.Usage = D3D11_USAGE_IMMUTABLE;
			else if (!rawData)
				bufDesc.Usage = D3D11_USAGE_DEFAULT;
		}
		else
		{
			if (memoryUsage == EMU_DEFAULT || memoryUsage == EMU_STATIC)
			{
				if ((bindFlags & ETBT_CPU_ACCESS_READ) || (bindFlags & ETBT_CPU_ACCESS_WRITE))
				{
					GF_PRINT_CONSOLE_INFO("Static or Default Buffer cannot be accessed by CPU.\n");
					return false;
				}
			}
			else if (memoryUsage == EMU_DYNAMIC)
			{
				if (bindFlags & ETBT_CPU_ACCESS_READ)
				{
					GF_PRINT_CONSOLE_INFO("Dynamic Buffer cannot be read by CPU.\n");
					return false;
				}
			}
		}

		if (rawData)
		{
			D3D11_SUBRESOURCE_DATA bufData;
			bufData.pSysMem = rawData;
			bufData.SysMemPitch = 0;
			bufData.SysMemSlicePitch = 0;
			hr = md3dDevice->CreateBuffer(&bufDesc, &bufData, &pd3dBuffer);
		}
		else
		{
			hr = md3dDevice->CreateBuffer(&bufDesc, NULL, &pd3dBuffer);
		}

		if (FAILED(hr))
			return false;

		if (bindFlags & ETBT_SHADER_RESOURCE)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			srvDesc.Format = getDxgiSRVFormat(format);
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
			srvDesc.BufferEx.FirstElement = 0;
			srvDesc.BufferEx.NumElements = elementNum;
			srvDesc.BufferEx.Flags = 0;

			hr = md3dDevice->CreateShaderResourceView(pd3dBuffer, &srvDesc, &pd3dSRV);
			if (FAILED(hr))
			{
				ReleaseCOM(pd3dBuffer);
				return false;
			}
		}

		if (bindFlags & ETBT_UNORDERED_ACCESS)
		{
			D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
			uavDesc.Format = getDxgiUAVFormat(format);
			uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			uavDesc.Buffer.FirstElement = 0;
			uavDesc.Buffer.NumElements = elementNum;
			uavDesc.Buffer.Flags = (bindFlags & ETBT_BUFFER_APPEND) ? D3D11_BUFFER_UAV_FLAG_APPEND : 0;
			
			hr = md3dDevice->CreateUnorderedAccessView(pd3dBuffer, &uavDesc, &pd3dUAV);
			if (FAILED(hr))
			{
				ReleaseCOM(pd3dBuffer);
				return false;
			}
		}

		md3dBuffer = pd3dBuffer;
		md3dSRV = pd3dSRV;
		md3dUAV = pd3dUAV;

		mFormat = format;
		mElementNum = elementNum;
		mElementStride = elementStride;
		mBindFlags = bindFlags;

		return true;
	}

	CD3D11Buffer::~CD3D11Buffer()
	{
		ReleaseCOM(md3dUAV);
		ReleaseCOM(md3dSRV);
		ReleaseCOM(md3dBuffer);
	}

	u32 CD3D11Buffer::getElementSize() const
	{
		return mElementStride;
	}

	void CD3D11Buffer::apply(E_SHADER_TYPE shaderType, u32 slot, E_TEXTURE_BIND_TYPE bindType)
	{
		if (bindType == ETBT_SHADER_RESOURCE)
		{
			if (md3dSRV)
				md3d11Driver->setTexture(shaderType, slot, md3dSRV);
		}
		else if (bindType == ETBT_UNORDERED_ACCESS)
		{
			if (md3dUAV)
				md3d11Driver->setRWTexture(slot, md3dUAV);
		}
	}

	bool CD3D11Buffer::getData(void* data)
	{
		if (mBindFlags & ETBT_CPU_ACCESS_READ)
		{
			D3D11_MAPPED_SUBRESOURCE mappedData;
			md3dDeviceContext->Map(md3dBuffer, 0, D3D11_MAP_READ, 0, &mappedData);
			memcpy(data, mappedData.pData, mElementNum * mElementStride);
			md3dDeviceContext->Unmap(md3dBuffer, 0);
			return true;
		}

		return false;
	}

	bool CD3D11Buffer::setData(void* data, u32 elementCount)
	{
		elementCount = min(elementCount, mElementNum);

		if (mBindFlags & ETBT_CPU_ACCESS_WRITE)
		{
			D3D11_MAPPED_SUBRESOURCE mappedData;
			md3dDeviceContext->Map(md3dBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
			memcpy(mappedData.pData, data, mElementStride * elementCount);
			md3dDeviceContext->Unmap(md3dBuffer, 0);
			return true;
		}

		return false;
	}

	bool CD3D11Buffer::copyDataToAnotherBuffer(IBuffer* anotherBuffer)
	{
		if (!anotherBuffer)
			return false;

		if (mElementStride != anotherBuffer->getElementSize())
		{
			GF_PRINT_CONSOLE_INFO("Buffers with different element size couldn't copy data with each other.\n");
			return false;
		}

		if (mElementNum > anotherBuffer->getElementNum())
		{
			GF_PRINT_CONSOLE_INFO("Destination Buffer' size is smaller than Source Buffer.\n");
			return false;
		}

		CD3D11Buffer* pd3dAnotherBuffer = dynamic_cast<CD3D11Buffer*>(anotherBuffer);
		md3dDeviceContext->CopyResource(pd3dAnotherBuffer->md3dBuffer, md3dBuffer);
		return true;
	}

}
