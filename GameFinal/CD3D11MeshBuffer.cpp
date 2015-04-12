#include "stdafx.h"
#include "CD3D11MeshBuffer.h"

namespace gf
{

	bool CD3D11MeshBuffer::init(void* vertices,
		void* indices,
		u32 vertexCount,
		u32 indicesCount,
		u32 vertexStride,
		bool bit32Index)
	{
		HRESULT hr;

		mVertexCount = vertexCount;
		mIndiceCount = indicesCount;
		mVertexStride = vertexStride;

		if (indices == nullptr || indicesCount == 0)
		{
			mUseIndexBuffer = false;
		}
		else
		{
			mUseIndexBuffer = true;
			if (bit32Index)
				mIndexFormat = DXGI_FORMAT_R32_UINT;
			else
				mIndexFormat = DXGI_FORMAT_R16_UINT;
		}

		// TODO: should modify in the future
		if (mMemoryUsage != EMU_STATIC && mMemoryUsage != EMU_DEFAULT
			&& mMemoryUsage != EMU_DYNAMIC)
		{
			throw std::runtime_error("can not create dynamic now!");
			return false;
		}

		D3D11_BUFFER_DESC vbDesc;
		vbDesc.ByteWidth = vertexStride * vertexCount;
		vbDesc.Usage = getD3d11Usage(mMemoryUsage);
		vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbDesc.StructureByteStride = 0;
		vbDesc.MiscFlags = 0;

		if (mMemoryUsage == EMU_STATIC || mMemoryUsage == EMU_DEFAULT)
		{
			vbDesc.CPUAccessFlags = 0;
			D3D11_SUBRESOURCE_DATA vbData;
			vbData.pSysMem = vertices;
			hr = md3dDevice->CreateBuffer(&vbDesc, &vbData, &md3dVertexBuffer);
		}
		else if (mMemoryUsage == EMU_DYNAMIC)
		{
			vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			hr = md3dDevice->CreateBuffer(&vbDesc, 0, &md3dVertexBuffer);
		}

		if (FAILED(hr))
			return false;

		if (mUseIndexBuffer)
		{
			u32 indiceStride = sizeof(WORD);
			if (bit32Index)
				indiceStride = sizeof(DWORD);

			D3D11_BUFFER_DESC ibDesc;
			ibDesc.ByteWidth = indiceStride * indicesCount;
			ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			ibDesc.MiscFlags = 0;
			ibDesc.StructureByteStride = 0;
			ibDesc.Usage = getD3d11Usage(mMemoryUsage);

			if (mMemoryUsage == EMU_STATIC || mMemoryUsage == EMU_DEFAULT)
			{
				ibDesc.CPUAccessFlags = 0;
				D3D11_SUBRESOURCE_DATA ibData;
				ibData.pSysMem = indices;
				hr = md3dDevice->CreateBuffer(&ibDesc, &ibData, &md3dIndexBuffer);
			}
			else if(mMemoryUsage == EMU_DYNAMIC)
			{
				ibDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				hr = md3dDevice->CreateBuffer(&ibDesc, 0, &md3dIndexBuffer);
			}

			if (FAILED(hr))
				return false;
		}

		return true;
	}

	CD3D11MeshBuffer::~CD3D11MeshBuffer()
	{
		ReleaseCOM(md3dVertexBuffer);
		ReleaseCOM(md3dIndexBuffer);
	}

	void CD3D11MeshBuffer::bind()
	{
		u32 offset = 0;

		CD3D11Driver::SD3D11DriverState& currentState = md3d11Driver->D3D11DriverState;
		
		bool bNeedUpdate = false;
		if (currentState.VertexBuffers[0] != md3dVertexBuffer)
			bNeedUpdate = true;

		u32 maxUpdateNum = 1;
		for (u32 i = 1; i < 16; i++)
		{
			if (currentState.VertexBuffers[i] != nullptr)
			{
				maxUpdateNum = i + 1;
				bNeedUpdate = true;
			}
		}
		
		if (bNeedUpdate)
		{
			ID3D11Buffer* vbs[16] = { 0 };
			vbs[0] = md3dVertexBuffer;
			md3dDeviceContext->IASetVertexBuffers(0, maxUpdateNum, vbs, &mVertexStride, &offset);
			memcpy(currentState.VertexBuffers, vbs, sizeof(vbs));
		}

		if (mUseIndexBuffer && currentState.IndexBuffer != md3dIndexBuffer)
		{
			md3dDeviceContext->IASetIndexBuffer(md3dIndexBuffer, mIndexFormat, 0);
			currentState.IndexBuffer = md3dIndexBuffer;
		}
	}

	void CD3D11MeshBuffer::bind(IMeshBuffer* pInstanceBuffer)
	{
		CD3D11MeshBuffer* pd3dInstanceBuffer = dynamic_cast<CD3D11MeshBuffer*>(pInstanceBuffer);
		CD3D11Driver::SD3D11DriverState& currentState = md3d11Driver->D3D11DriverState;

		bool bNeedUpdate = false;
		if (currentState.VertexBuffers[0] != md3dVertexBuffer
			|| currentState.VertexBuffers[1] != pd3dInstanceBuffer->md3dVertexBuffer)
		{
			bNeedUpdate = true;
		}

		u32 maxUpdateNum = 2;
		for (u32 i = 2; i < 16; i++)
		{
			if (currentState.VertexBuffers[i] != nullptr)
			{
				bNeedUpdate = true;
				maxUpdateNum = i + 1;
			}
		}

		if (bNeedUpdate)
		{
			ID3D11Buffer* vbs[16] = { 0 };
			vbs[0] = md3dVertexBuffer;
			vbs[1] = pd3dInstanceBuffer->md3dVertexBuffer;
			u32 strides[2] = { mVertexStride, pd3dInstanceBuffer->mVertexStride };
			u32 offsets[2] = { 0, 0 };

			md3dDeviceContext->IASetVertexBuffers(0, maxUpdateNum, vbs, strides, offsets);
			memcpy(currentState.VertexBuffers, vbs, sizeof(vbs));
		}

		if (mUseIndexBuffer && currentState.IndexBuffer != md3dIndexBuffer)
		{
			md3dDeviceContext->IASetIndexBuffer(md3dIndexBuffer, mIndexFormat, 0);
			currentState.IndexBuffer = md3dIndexBuffer;
		}
	}


	void CD3D11MeshBuffer::draw() const
	{
		if (mUseIndexBuffer)
		{
			md3dDeviceContext->DrawIndexed(mIndiceCount, 0, 0);
		}
		else
		{
			md3dDeviceContext->Draw(mVertexCount, 0);
		}
	}

	void CD3D11MeshBuffer::drawInstanced(u32 instanceCount) const
	{
		if (mUseIndexBuffer)
		{
			md3dDeviceContext->DrawIndexedInstanced(mIndiceCount, instanceCount, 0, 0, 0);
		}
		else
		{
			md3dDeviceContext->DrawInstanced(mVertexCount, instanceCount, 0, 0);
		}
	}

	void CD3D11MeshBuffer::drawIndexed(u32 start, u32 count, u32 baseVertex) const
	{
		if (!mUseIndexBuffer)
		{
			GF_PRINT_CONSOLE_INFO("The mesh don't use index buffer.\n");
			return;
		}

		md3dDeviceContext->DrawIndexed(count, start, baseVertex);
	}

	void CD3D11MeshBuffer::drawIndexedInstanced(u32 start, u32 indiceCountPerInstance, 
		u32 baseVertex, u32 instanceCount) const
	{
		if (!mUseIndexBuffer)
		{
			GF_PRINT_CONSOLE_INFO("The mesh don't use index buffer.\n");
			return;
		}

		md3dDeviceContext->DrawIndexedInstanced(indiceCountPerInstance, instanceCount, start, baseVertex, 0);
	}

	void CD3D11MeshBuffer::draw(u32 start, u32 count) const
	{
		md3dDeviceContext->Draw(count, start);
	}

	void CD3D11MeshBuffer::drawInstanced(u32 start, u32 vertexCountPerInstance, u32 instanceCount) const
	{
		md3dDeviceContext->DrawInstanced(vertexCountPerInstance, instanceCount, start, 0);
	}

	bool CD3D11MeshBuffer::setVertexData(void* data, u32 count)
	{
		if (!md3dVertexBuffer)
			return false;

		if (mMemoryUsage != EMU_DYNAMIC)
			return false;

		D3D11_MAPPED_SUBRESOURCE mappedData;
		md3dDeviceContext->Map(md3dVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
		memcpy(mappedData.pData, data, count * mVertexStride);
		md3dDeviceContext->Unmap(md3dVertexBuffer, 0);

		mVertexCount = count;
		return true;
	}

	bool CD3D11MeshBuffer::setIndiceData(void* data, u32 count)
	{
		if (!md3dIndexBuffer)
			return false;

		if (mMemoryUsage != EMU_DYNAMIC)
			return false;

		u32 indiceStride = (mIndexFormat == DXGI_FORMAT_R32_UINT) ? 4 : 2;

		D3D11_MAPPED_SUBRESOURCE mappedData;
		md3dDeviceContext->Map(md3dIndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
		memcpy(mappedData.pData, data, count * indiceStride);
		md3dDeviceContext->Unmap(md3dIndexBuffer, 0);

		mIndiceCount = count;
		return true;
	}
}

