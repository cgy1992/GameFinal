#ifndef __SIMPLE_MESH_INTERFACE_H__
#define __SIMPLE_MESH_INTERFACE_H__

#include "IMesh.h"
#include "IMaterial.h"

namespace gf
{

	class ISimpleMesh : public IMesh
	{
	public:
		ISimpleMesh(const std::string& name,
			u32 sortcode, 
			const math::SAxisAlignedBox& aabb)
			:IMesh(name, sortcode, aabb)
		{

		}

		virtual void bind() = 0;

		virtual void bind(IMeshBuffer* pInstanceBuffer) = 0;

		virtual void draw() const = 0;

		virtual void drawInstanced(u32 instanceCount) const = 0;
		

	};

}

#endif
