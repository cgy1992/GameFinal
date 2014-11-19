#ifndef __SKIN_MODEL_RENDER_BUFFER_INTERFACE_H__
#define __SKIN_MODEL_RENDER_BUFFER_INTERFACE_H__

#include "IRenderableBuffer.h"

namespace gf
{

	class IAnimatedMeshBuffer : public IRenderableBuffer
	{
	public:
		virtual void bindIndexBuffer() = 0;

		virtual void bindVertexBuffer(bool bAnimated) = 0;

		virtual void drawIndexed(u32 start, u32 count, u32 baseVertex) = 0;

		virtual E_BUFFER_TYPE getType() const
		{
			return EBT_ANIMATED_MESH_BUFFER;
		}
	};

}

#endif
