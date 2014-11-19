#include "stdafx.h"
#include "CModelMesh.h"

namespace gf
{
	void CModelMesh::drawSubset(u32 index) const
	{
		if (index >= mSubsets.size())
			return;

		const SModelSubset& subset = mSubsets[index];
		mMeshBuffer->drawIndexed(subset.StartIndexLocation, subset.IndexCount, subset.BaseVertexLocation);
	}

	void CModelMesh::bind()
	{
		mMeshBuffer->bind();
	}

	void CModelMesh::bind(IMeshBuffer* pInstanceBuffer)
	{
		mMeshBuffer->bind(pInstanceBuffer);
	}

	void CModelMesh::drawSubsetInstanced(u32 subsetIndex, u32 instanceCount) const
	{
		if (subsetIndex > mSubsets.size())
			return;

		const SModelSubset& subset = mSubsets[subsetIndex];
		mMeshBuffer->drawIndexedInstanced(subset.StartIndexLocation, subset.IndexCount, subset.BaseVertexLocation, instanceCount);
	}

	

}
