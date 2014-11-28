#ifndef __MODEL_MESH_INTERFACE_H__
#define __MODEL_MESH_INTERFACE_H__

#include "IMesh.h"
#include "IMaterial.h"
namespace gf
{
	class IModelMesh : public IMesh
	{
	public:
		struct SModelSubset
		{
			u32					IndexCount;
			u32					StartIndexLocation;
			u32					BaseVertexLocation;
			XMFLOAT4X4			OffsetMatrix;
			IMaterial*			Material;
		};

	public:
		IModelMesh(const std::string& name,
			u32 sortcode, 
			const math::SAxisAlignedBox& aabb,
			u32 vertexFormat)
			:IMesh(name, sortcode, aabb, vertexFormat)
		{

		}

		virtual u32 getSubsetCount() const = 0;

		virtual IMaterial* getMaterial(u32 index) const = 0;

		virtual bool getMaterialName(u32 index, std::string& name) const = 0;

		virtual void drawSubset(u32 index) const = 0;

		virtual XMFLOAT4X4 getSubsetTransform(u32 subset) const = 0;

		virtual void bind() = 0;

		virtual void bind(IMeshBuffer* pInstanceBuffer) = 0;

		virtual void drawSubsetInstanced(u32 subsetIndex, u32 instanceCount) const = 0;

	protected:


	};

}
#endif

