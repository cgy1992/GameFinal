#ifndef __CD3D11_MESH_MESH_CLASS_H__
#define __CD3D11_MESH_MESH_CLASS_H__

#include "IMeshBuffer.h"
#include "CD3D11Driver.h"
#include "D3DUtil.h"
namespace gf
{

	class CD3D11MeshBuffer : public IMeshBuffer
	{
	public:
		CD3D11MeshBuffer(E_MEMORY_USAGE usage,
			ID3D11Device* pd3dDevice,
			ID3D11DeviceContext* pd3dDeviceContext,
			CD3D11Driver* pd3d11Driver)
			:IMeshBuffer(usage)
			, md3dDevice(pd3dDevice)
			, md3dDeviceContext(pd3dDeviceContext)
			, md3d11Driver(pd3d11Driver)
			, md3dVertexBuffer(nullptr)
			, md3dIndexBuffer(nullptr)
			, mVertexCount(0)
			, mIndiceCount(0)
			, mVertexStride(0)
			, mUseIndexBuffer(false)
		{

		}

		virtual ~CD3D11MeshBuffer();

		virtual bool init(void* vertices,
			void* indices,
			u32 vertexCount,
			u32 indicesCount,
			u32 vertexStride,
			bool bit32Index);

		virtual bool setVertexData(void* data, u32 size);

		virtual bool setIndiceData(void* data, u32 count);

		virtual void bind();

		virtual void bind(IMeshBuffer* pInstanceBuffer);

		virtual void draw() const;

		virtual void drawIndexed(u32 start, u32 count, u32 baseVertex) const;

		virtual void draw(u32 start, u32 count) const;

		virtual void drawInstanced(u32 instanceCount) const;

		virtual void drawIndexedInstanced(u32 start, u32 indiceCountPerInstance, u32 baseVertex, u32 instanceCount) const;

		virtual void drawInstanced(u32 start, u32 vertexCountPerInstance, u32 instanceCount) const;


	private:
		ID3D11Device*			md3dDevice;
		ID3D11DeviceContext*	md3dDeviceContext;

		CD3D11Driver*			md3d11Driver;

		ID3D11Buffer*			md3dVertexBuffer;
		ID3D11Buffer*			md3dIndexBuffer;

		u32						mVertexCount;
		u32						mIndiceCount;
		u32						mVertexStride;

		bool					mUseIndexBuffer;

		DXGI_FORMAT				mIndexFormat;
	};
}
#endif
