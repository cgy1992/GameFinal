#include "stdafx.h"
#include "CTerrainMesh.h"
#include "IResourceFactory.h"
#include "ITextureManager.h"

namespace gf
{
	bool CTerrainMesh::init(
		const std::string& heightmapFileName,
		const std::string& heigtmapFullPath,
		IResourceFactory* resourceFactory,
		ITextureManager* textureManager,
		bool bCreateTessellationMesh,
		bool bCreateNormal,
		f32 fTexcoordScale,
		u32 cellsPerPatch,
		E_MEMORY_USAGE usage)
	{
		FILE* fp = nullptr;
		errno_t err = fopen_s(&fp, heigtmapFullPath.c_str(), "rb");
		if (err != 0)
		{
			GF_PRINT_CONSOLE_INFO("The terrain mesh ('%s') create failed! \
				The raw file '%s' doesn't exist.\n", mName.c_str(), heightmapFileName.c_str());

			return false;
		}
			

		fseek(fp, 0, SEEK_END);
		u32 fileSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		mRowVertexNum = static_cast<u32>(sqrt((double)fileSize));
		if (mRowVertexNum * mRowVertexNum != fileSize)
		{
			GF_PRINT_CONSOLE_INFO("The terrain mesh ('%s') create failed! \
					The raw file '%s' should contain a square image.\n", mName.c_str(), heightmapFileName.c_str());

			fclose(fp);
			return false;
		}

		u8 *rawDatas = (u8*)malloc(fileSize);
		fread(rawDatas, 1, fileSize, fp);
		fclose(fp);

		f32 fMaxHeight = -FLT_MAX;
		f32 fMinHeight = FLT_MAX;

		mHeightValues.resize(mRowVertexNum * mRowVertexNum);

		for (u32 i = 0; i < fileSize; i++)
		{
			mHeightValues[i] = rawDatas[i] * mHeightScale;
			if (mHeightValues[i] > fMaxHeight)
				fMaxHeight = mHeightValues[i];
			if (mHeightValues[i] < fMinHeight)
				fMinHeight = mHeightValues[i];
		}

		free(rawDatas);
		mTotalWidth = (mRowVertexNum - 1) * mVertexSpace;

		// !! aabb's center doesn't equal with terrain center
		mAabb.Center = XMFLOAT3(0, (fMaxHeight + fMinHeight) * 0.5f, 0);
		mAabb.Extents = XMFLOAT3(mTotalWidth * 0.5f, (fMaxHeight - fMinHeight) * 0.5f, mTotalWidth * 0.5f);
		
		
		f32 startX = -mTotalWidth * 0.5f;
		f32 startZ = mTotalWidth * 0.5f;

		mTessellated = bCreateTessellationMesh;
		mCellsPerPatch = cellsPerPatch;
		mPatchWidth = cellsPerPatch * mVertexSpace;
		mPatchNumPerRow = u32(static_cast<f32>(mRowVertexNum - 1) / mCellsPerPatch);
		mTexcoordScale = fTexcoordScale;

		u8* vertices = nullptr;
		u8* indices = nullptr;
		u32 indiceCount;
		u32 vertexCount;
		u32 vertexStride;
		bool bUse32Index = false;

		if (!bCreateTessellationMesh)
		{
			// each vertex must contain POSITION and TEXCOORD
			vertexStride = sizeof(XMFLOAT3)+sizeof(XMFLOAT2);
			if (bCreateNormal)
				vertexStride += sizeof(XMFLOAT3);

			vertexCount = mRowVertexNum * mRowVertexNum;
			vertices = (u8*)malloc(vertexStride * vertexCount);
			f32 x, y, z, tu, tv;
			z = startZ;
			tv = 0.0f;
			u8* pCurVertex = vertices;
			f32 texSpace = 1.0f / (mRowVertexNum - 1) * fTexcoordScale;

			// fill the vertices buffer
			for (u32 row = 0; row < mRowVertexNum; row++, z -= mVertexSpace, tv += texSpace)
			{
				x = startX;
				tu = 0.0f;
				for (u32 col = 0; col < mRowVertexNum; col++, x += mVertexSpace, tu += texSpace)
				{
					y = mHeightValues[row * mRowVertexNum + col];
					XMFLOAT3 pos = XMFLOAT3(x, y, z);
					XMFLOAT2 tex = XMFLOAT2(tu, tv);

					memcpy(pCurVertex, &pos, sizeof(XMFLOAT3));
					pCurVertex += sizeof(XMFLOAT3);

					if (bCreateNormal)
					{
						XMFLOAT3 normal = computeVertexNormal(pos, row, col);
						memcpy(pCurVertex, &normal, sizeof(XMFLOAT3));
						pCurVertex += sizeof(XMFLOAT3);
					}

					memcpy(pCurVertex, &tex, sizeof(XMFLOAT2));
					pCurVertex += sizeof(XMFLOAT2);
				}
			}

			// fill the indices buffer
			indiceCount = (mRowVertexNum - 1) * (mRowVertexNum - 1) * 6;
			if (indiceCount >= (1 << 16))
				bUse32Index = true;

			if (!bUse32Index)
			{
				u16* pIndices = (u16*)malloc(indiceCount * sizeof(u16));
				u32 index = 0;
				for (u32 row = 0; row < mRowVertexNum - 1; row++)
				{
					for (u32 col = 0; col < mRowVertexNum - 1; col++, index += 6)
					{
						pIndices[index] = row * mRowVertexNum + col;
						pIndices[index + 1] = row * mRowVertexNum + col + 1;
						pIndices[index + 2] = (row + 1) * mRowVertexNum + col + 1;

						pIndices[index + 3] = row * mRowVertexNum + col;
						pIndices[index + 4] = (row + 1) * mRowVertexNum + col + 1;
						pIndices[index + 5] = (row + 1) * mRowVertexNum + col;
					}
				}

				indices = (u8*)pIndices;
			}
			else
			{
				u32* pIndices = (u32*)malloc(indiceCount * sizeof(u32));
				u32 index = 0;
				for (u32 row = 0; row < mRowVertexNum - 1; row++)
				{
					for (u32 col = 0; col < mRowVertexNum - 1; col++, index += 6)
					{
						pIndices[index] = row * mRowVertexNum + col;
						pIndices[index + 1] = row * mRowVertexNum + col + 1;
						pIndices[index + 2] = (row + 1) * mRowVertexNum + col + 1;

						pIndices[index + 3] = row * mRowVertexNum + col;
						pIndices[index + 4] = (row + 1) * mRowVertexNum + col + 1;
						pIndices[index + 5] = (row + 1) * mRowVertexNum + col;
					}
				}
				indices = (u8*)pIndices;
			}

			
		}
		else // if use tessellation 
		{
			// each vertex must contain POSITION, TEXCOORD and HEIGHTBOUND
			// BoundY contains minY and maxY, to compute the vertical extend of aabb in shader
			vertexStride = sizeof(XMFLOAT3)+sizeof(XMFLOAT2)+sizeof(XMFLOAT2);
			vertexCount = (mPatchNumPerRow + 1) * (mPatchNumPerRow + 1);
			indiceCount = (mPatchNumPerRow)* (mPatchNumPerRow)* 4;
			vertices = (u8*)malloc(vertexStride * vertexCount);
			
			f32 x, z, tu, tv;
			z = startZ;
			tv = 0.0f;
			u8* pCurVertex = vertices;
			f32 texSpace = 1.0f / mPatchNumPerRow * fTexcoordScale;

			for (u32 row = 0; row < mPatchNumPerRow + 1; row++, z -= mPatchWidth, tv += texSpace)
			{
				x = startX;
				tu = 0.0f;
				for (u32 col = 0; col < mPatchNumPerRow + 1; col++, x += mPatchWidth, tu += texSpace)
				{
					XMFLOAT3 pos = XMFLOAT3(x, 0, z);
					XMFLOAT2 tex = XMFLOAT2(tu, tv);
					XMFLOAT2 boundsY = getPatchHeightBounds(row, col);

					memcpy(pCurVertex, &pos, sizeof(XMFLOAT3));
					pCurVertex += sizeof(XMFLOAT3);

					memcpy(pCurVertex, &tex, sizeof(XMFLOAT2));
					pCurVertex += sizeof(XMFLOAT2);

					memcpy(pCurVertex, &boundsY, sizeof(XMFLOAT2));
					pCurVertex += sizeof(XMFLOAT2);
				}
			}

			if (indiceCount >= (1 << 16))
				bUse32Index = true;

			if (!bUse32Index)
			{
				u32 index = 0;
				u16* pIndices = (u16*)malloc(indiceCount * sizeof(u16));
				for (u32 i = 0; i < mPatchNumPerRow; i++)
				{
					for (u32 j = 0; j < mPatchNumPerRow; j++, index += 4)
					{
						pIndices[index] = i * (mPatchNumPerRow + 1) + j;
						pIndices[index + 1] = i * (mPatchNumPerRow + 1) + j + 1;
						pIndices[index + 2] = (i + 1) * (mPatchNumPerRow + 1) + j;
						pIndices[index + 3] = (i + 1) * (mPatchNumPerRow + 1) + j + 1;
					}
				}
				indices = (u8*)pIndices;
			}
			else
			{
				u32 index = 0;
				u32* pIndices = (u32*)malloc(indiceCount * sizeof(u32));
				for (u32 i = 0; i < mPatchNumPerRow; i++)
				{
					for (u32 j = 0; j < mPatchNumPerRow; j++, index += 4)
					{
						pIndices[index] = i * (mPatchNumPerRow + 1) + j;
						pIndices[index + 1] = i * (mPatchNumPerRow + 1) + j + 1;
						pIndices[index + 2] = (i + 1) * (mPatchNumPerRow + 1) + j;
						pIndices[index + 3] = (i + 1) * (mPatchNumPerRow + 1) + j + 1;
					}
				}
				indices = (u8*)pIndices;
			}
		}

		// create mesh
		mMeshBuffer = resourceFactory->createMeshBuffer(usage, vertices, indices,
			vertexCount, indiceCount,
			vertexStride, bUse32Index);

		if (!mMeshBuffer)
		{
			GF_PRINT_CONSOLE_INFO("The mesh ('%s') create failed!. Due to the failure of mesh buffer.\n", mName.c_str());
			return false;
		}

		free(vertices);
		free(indices);

		// create a heighmap texture
		std::vector<HALF> textureData(mRowVertexNum * mRowVertexNum);
		std::transform(mHeightValues.begin(), mHeightValues.end(), textureData.begin(), XMConvertFloatToHalf);

		mHeightMapTexture = textureManager->get(heightmapFileName, false);
		if (!mHeightMapTexture)
		{
			mHeightMapTexture = textureManager->createTexture2D(heightmapFileName, mRowVertexNum, mRowVertexNum,
				ETBT_SHADER_RESOURCE, (void*)&textureData[0],
				1, EGF_R16_FLOAT, 0);
		}

		if (!mHeightMapTexture)
		{
			GF_PRINT_CONSOLE_INFO("The mesh ('%s') create failed!. Due to the failure of height map texture('%s').\n", mName.c_str(), heightmapFileName.c_str());
			return false;
		}
			
		mHeightMapTexture->grab();
		return true;
	}

	XMFLOAT2 CTerrainMesh::getPatchHeightBounds(u32 patchRow, u32 patchCol) const
	{
		if (patchRow == mPatchNumPerRow || patchCol == mPatchNumPerRow)
		{
			return XMFLOAT2(0, 0);
		}

		f32 minHeight = FLT_MAX;
		f32 maxHeight = -FLT_MAX;

		u32 row = patchRow * mCellsPerPatch;
		u32 col = patchCol * mCellsPerPatch;

		for (u32 i = row; i <= row + mCellsPerPatch; i++)
		{
			for (u32 j = col; j <= col + mCellsPerPatch; j++)
			{
				f32 h = mHeightValues[i * mRowVertexNum + j];
				if (h < minHeight)
					minHeight = h;
				if (h > maxHeight)
					maxHeight = h;
			}
		}

		return XMFLOAT2(minHeight, maxHeight);
	}

	XMFLOAT3 CTerrainMesh::computeVertexNormal(XMFLOAT3 pos, u32 row, u32 col) const
	{
		f32 deltaX, deltaY, deltaZ;
		if (col == 0)
		{
			deltaX = mVertexSpace;
			deltaY = getHeightByRowAndCol(row, col + 1) - pos.y;
		}
		else if (col == mRowVertexNum - 1)
		{
			deltaX = mVertexSpace;
			deltaY = pos.y - getHeightByRowAndCol(row, col - 1);
		}
		else
		{
			deltaX = mVertexSpace + mVertexSpace;
			deltaY = getHeightByRowAndCol(row, col + 1) - getHeightByRowAndCol(row, col - 1);
		}

		XMVECTOR tangent = XMVectorSet(deltaX, deltaY, 0.0f, 0.0f);
		
		if (row == 0)
		{
			deltaZ = -mVertexSpace;
			deltaY = getHeightByRowAndCol(row + 1, col) - pos.y;
		}
		else if (row == mRowVertexNum - 1)
		{
			deltaZ = -mVertexSpace;
			deltaY = pos.y - getHeightByRowAndCol(row - 1, col);
		}
		else
		{
			deltaZ = -mVertexSpace - mVertexSpace;
			deltaY = getHeightByRowAndCol(row + 1, col) - getHeightByRowAndCol(row - 1, col);
		}

		XMVECTOR binormal = XMVectorSet(0.0f, deltaY, deltaZ, 0.0f);
		XMVECTOR normal = XMVector3Cross(tangent, binormal);
		
		XMFLOAT3 n;
		XMStoreFloat3(&n, normal);

		return n;
	}

	f32 CTerrainMesh::getHeightByRowAndCol(u32 row, u32 col) const
	{
		return mHeightValues[row * mRowVertexNum + col];
	}

	void CTerrainMesh::bind()
	{
		mMeshBuffer->bind();
	}

	void CTerrainMesh::draw() const
	{
		mMeshBuffer->draw();
	}

	f32 CTerrainMesh::getHeight(f32 x, f32 z) const
	{
		x = mTotalWidth * 0.5f + x;
		z = mTotalWidth * 0.5f - z;

		if (x < 0 || x > mTotalWidth || z < 0 || z > mTotalWidth)
			return 0.0f;

		x /= mVertexSpace;
		z /= mVertexSpace;

		int i = (int)z;
		int j = (int)x;

		/*
		A - B
		| / |
		C - D		
		*/

		//计算出A,B,C,D四个点的高度
		float A = mHeightValues[i * mRowVertexNum + j];
		float B = mHeightValues[i * mRowVertexNum + j + 1];
		float C = mHeightValues[(i + 1) * mRowVertexNum + j];
		float D = mHeightValues[(i + 1) * mRowVertexNum + j + 1];

		float dx = x - j;
		float dz = z - i;
		float height;

		if (dx + dz < 1.0f) // 如果落在三角形ABC中
		{
			height = A + (B - A) * dx + (C - A) * dz;
		}
		else //如果落在三角形BCD中
		{
			height = D + (B - D) * (1.0f - dz) + (C - D) * (1.0f - dx);
		}
		return height;


	}

	bool CTerrainMesh::intersectRay(const math::SRay& ray, XMFLOAT3& intersectPoint, float epsilon)
	{
		if (!isInsideTerrainScope(ray.Origin.x, ray.Origin.z))
			return false;

		float height = getHeight(ray.Origin.x, ray.Origin.z);

		// if the ray's origin is already below the terrain.
		if (height > ray.Origin.y)
			return false;

		float linearSearchMove = mVertexSpace;
		XMFLOAT3 prevPoint;
		XMFLOAT3 nextPoint = ray.Origin;

		while (nextPoint.y > height)
		{
			prevPoint = nextPoint;
			nextPoint = math::VectorAdd(nextPoint, math::VectorMultiply(ray.Direction, linearSearchMove));
			
			if (!isInsideTerrainScope(nextPoint.x, nextPoint.z))
				return false;
			
			height = getHeight(nextPoint.x, nextPoint.z);
		}

		// binary search
		XMFLOAT3 midPoint;
		u32 count = 0;
		while (count < 100)
		{
			midPoint = math::VectorAdd(math::VectorMultiply(prevPoint, 0.5f), math::VectorMultiply(nextPoint, 0.5f));
			height = getHeight(midPoint.x, midPoint.z);

			float err = midPoint.y - height;
			if (fabs(err) < epsilon)
			{
				intersectPoint = midPoint;
				return true;
			}

			if (err > 0)
				prevPoint = midPoint;
			else
				nextPoint = midPoint;

			count++;
		}

		intersectPoint = midPoint;
		return true;
	}

	bool CTerrainMesh::isInsideTerrainScope(f32 x, f32 z)
	{

		x = mTotalWidth * 0.5f + x;
		z = mTotalWidth * 0.5f - z;

		if (x < 0 || x > mTotalWidth || z < 0 || z > mTotalWidth)
			return false;

		return true;
	}

	CTerrainMesh::~CTerrainMesh()
	{
		ReleaseReferenceCounted(mMeshBuffer);
		ReleaseReferenceCounted(mHeightMapTexture);
	}




}

