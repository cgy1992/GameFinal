#ifndef __RENDERABLE_BUFFER_H_INTERFACE__
#define __RENDERABLE_BUFFER_H_INTERFACE__

#include "IReferenceCounted.h"

namespace gf
{
	enum E_BUFFER_TYPE
	{
		EBT_MESH_BUFFER,
		EBT_ANIMATED_MESH_BUFFER,
		EBT_INSTANCE_BUFFER,
	};

	class IRenderableBuffer : public IReferenceCounted
	{
		virtual E_BUFFER_TYPE getType() const = 0;
	};

}

#endif

