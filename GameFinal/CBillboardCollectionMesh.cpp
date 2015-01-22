#include "stdafx.h"
#include "IMeshManager.h"
#include "CBillboardCollectionMesh.h"
#include "IMeshBuffer.h"

namespace gf
{
	CBillboardCollectionMesh::~CBillboardCollectionMesh()
	{
		ReleaseReferenceCounted(mMeshBuffer);
	}

	bool CBillboardCollectionMesh::submitUpdate()
	{
		if (!mAlterable)
			return false;

		mMeshBuffer->setVertexData((void*)&mBillboards[0], mBillboards.size());
		return true;
	}

	void CBillboardCollectionMesh::bind()
	{
		mMeshBuffer->bind();
	}

	void CBillboardCollectionMesh::draw() const
	{
		mMeshBuffer->draw();
	}

	void CBillboardCollectionMesh::bind(IMeshBuffer* pInstanceBuffer)
	{
		mMeshBuffer->bind(pInstanceBuffer);
	}

	void CBillboardCollectionMesh::drawInstanced(u32 instanceCount) const
	{
		mMeshBuffer->drawInstanced(instanceCount);
	}

	IRenderableBuffer* CBillboardCollectionMesh::getRenderableBuffer()
	{
		return mMeshBuffer;
	}
}

