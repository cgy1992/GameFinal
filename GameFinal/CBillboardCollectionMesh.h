#ifndef __BILLBOARD_COLLECTION_NODE_CLASS_H__
#define __BILLBOARD_COLLECTION_NODE_CLASS_H__

#include "IBillboardCollectionMesh.h"

namespace gf
{
	class CBillboardCollectionMesh : public IBillboardCollectionMesh
	{
	public:
		CBillboardCollectionMesh(
			const std::string& name,
			u32 sortcode,
			const math::SAxisAlignedBox& aabb,
			u32 maxNum,
			bool alterable,
			const std::vector<SBillboard>& billboards,
			IMeshBuffer* buffer)
			: IBillboardCollectionMesh(name, sortcode, aabb, maxNum, alterable, billboards)
			, mMeshBuffer(buffer)
		{
			
		}

		virtual ~CBillboardCollectionMesh();

		virtual bool submitUpdate();

		virtual void bind();

		virtual void bind(IMeshBuffer* pInstanceBuffer);

		virtual void draw() const;

		virtual void drawInstanced(u32 instanceCount) const;

		virtual IRenderableBuffer* getRenderableBuffer();

	private:
		IMeshBuffer*			mMeshBuffer;

	};
}



#endif

