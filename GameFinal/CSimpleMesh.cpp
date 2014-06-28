#include "stdafx.h"
#include "CSimpleMesh.h"
namespace gf
{
	void CSimpleMesh::bind()
	{
		mMeshBuffer->bind();
	}

	void CSimpleMesh::draw() const
	{
		mMeshBuffer->draw();
	}
}