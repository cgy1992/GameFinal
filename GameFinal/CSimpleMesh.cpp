#include "stdafx.h"
#include "CSimpleMesh.h"
namespace gf
{
	void CSimpleMesh::bind()
	{
		mMeshBuffer->bind();
	}

	void CSimpleMesh::bind(IMeshBuffer* pInstanceBuffer)
	{
		mMeshBuffer->bind(pInstanceBuffer);
	}

	void CSimpleMesh::draw() const
	{
		mMeshBuffer->draw();
	}

	void CSimpleMesh::drawInstanced(u32 instanceCount) const
	{
		mMeshBuffer->drawInstanced(instanceCount);
	}

}