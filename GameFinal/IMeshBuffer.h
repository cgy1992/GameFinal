#ifndef __MESH_BUFFER_INTERFACE_H__
#define __MESH_BUFFER_INTERFACE_H__

#include "IRenderableBuffer.h"
#include "IMesh.h"

namespace gf
{

	class IMeshBuffer : public IRenderableBuffer
	{
	public:
		IMeshBuffer(E_MEMORY_USAGE usage)
			: mMemoryUsage(usage)
		{

		}

		virtual bool init(void* vertices,
			void* indices,
			u32 vertexCount,
			u32 indicesCount,
			u32 vertexStride,
			bool bit32Index) = 0;

		virtual bool setVertexData(void* data, u32 count) = 0;

		virtual bool setIndiceData(void* data, u32 count) = 0;

		virtual void bind() = 0;

		virtual void bind(IMeshBuffer* pInstanceBuffer) = 0;

		virtual void draw() const = 0;

		virtual void drawIndexed(u32 start, u32 count, u32 baseVertex) const = 0;

		virtual void draw(u32 start, u32 count) const = 0;

		virtual void drawInstanced(u32 instanceCount) const = 0;

		virtual void drawIndexedInstanced(u32 start, u32 indiceCountPerInstance, u32 baseVertex, u32 instanceCount) const = 0;

		virtual void drawInstanced(u32 start, u32 vertexCountPerInstance, u32 instanceCount) const = 0;

		virtual E_BUFFER_TYPE getType() const
		{
			return EBT_MESH_BUFFER;
		}

	protected:
		E_MEMORY_USAGE		mMemoryUsage;
	};

}

#endif