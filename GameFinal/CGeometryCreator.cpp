#include "stdafx.h"
#include "CGeometryCreator.h"

namespace gf
{

	void CGeometryCreator::createCubeData(
		f32 width,
		f32 height,
		f32 depth,
		SGeometryData& geoData)
	{
		geoData.Vertices.resize(24);
		geoData.Indices.resize(36);

		f32 w2 = 0.5f*width;
		f32 h2 = 0.5f*height;
		f32 d2 = 0.5f*depth;

		SGeometryVertex* v = &geoData.Vertices[0];

		// Fill in the front face vertex data.
		v[0] = SGeometryVertex(-w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		v[1] = SGeometryVertex(-w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		v[2] = SGeometryVertex(+w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
		v[3] = SGeometryVertex(+w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		// Fill in the back face vertex data.
		v[4] = SGeometryVertex(-w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
		v[5] = SGeometryVertex(+w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		v[6] = SGeometryVertex(+w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		v[7] = SGeometryVertex(-w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

		// Fill in the top face vertex data.
		v[8] = SGeometryVertex(-w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		v[9] = SGeometryVertex(-w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		v[10] = SGeometryVertex(+w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
		v[11] = SGeometryVertex(+w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		// Fill in the bottom face vertex data.
		v[12] = SGeometryVertex(-w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
		v[13] = SGeometryVertex(+w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		v[14] = SGeometryVertex(+w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		v[15] = SGeometryVertex(-w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

		// Fill in the left face vertex data.
		v[16] = SGeometryVertex(-w2, -h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
		v[17] = SGeometryVertex(-w2, +h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
		v[18] = SGeometryVertex(-w2, +h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
		v[19] = SGeometryVertex(-w2, -h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

		// Fill in the right face vertex data.
		v[20] = SGeometryVertex(+w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
		v[21] = SGeometryVertex(+w2, +h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
		v[22] = SGeometryVertex(+w2, +h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
		v[23] = SGeometryVertex(+w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

		u16* i = &geoData.Indices[0];

		// Fill in the front face index data
		i[0] = 0; i[1] = 1; i[2] = 2;
		i[3] = 0; i[4] = 2; i[5] = 3;

		// Fill in the back face index data
		i[6] = 4; i[7] = 5; i[8] = 6;
		i[9] = 4; i[10] = 6; i[11] = 7;

		// Fill in the top face index data
		i[12] = 8; i[13] = 9; i[14] = 10;
		i[15] = 8; i[16] = 10; i[17] = 11;

		// Fill in the bottom face index data
		i[18] = 12; i[19] = 13; i[20] = 14;
		i[21] = 12; i[22] = 14; i[23] = 15;

		// Fill in the left face index data
		i[24] = 16; i[25] = 17; i[26] = 18;
		i[27] = 16; i[28] = 18; i[29] = 19;

		// Fill in the right face index data
		i[30] = 20; i[31] = 21; i[32] = 22;
		i[33] = 20; i[34] = 22; i[35] = 23;

		geoData.Aabb.Center = XMFLOAT3(0, 0, 0);
		geoData.Aabb.Extents = XMFLOAT3(w2, h2, d2);
	}

	void CGeometryCreator::createPlaneData(
		f32 width,
		f32 depth,
		u32 xsegments,
		u32 ysegments,
		f32 uTiles,
		f32 vTiles,
		SGeometryData& geoData)
	{
		UINT vertexCount = (xsegments + 1) * (ysegments + 1);
		UINT faceCount = xsegments * ysegments * 2;

		f32 halfWidth = 0.5f * width;
		f32 halfDepth = 0.5f * depth;

		f32 dx = width / xsegments;
		f32 dz = depth / ysegments;

		f32 du = uTiles / xsegments;
		f32 dv = vTiles / ysegments;

		geoData.Aabb.Center = XMFLOAT3(0, 0, 0);
		geoData.Aabb.Extents = XMFLOAT3(halfWidth, min(0.1f, width * 0.01f), halfDepth);

		geoData.Vertices.resize(vertexCount);
		for (u32 i = 0; i <= ysegments; ++i)
		{
			f32 z = halfDepth - i * dz;
			for (u32 j = 0; j <= xsegments; ++j)
			{
				f32 x = -halfWidth + j * dx;
				u32 index = i * (xsegments + 1) + j;
				geoData.Vertices[index].Position = XMFLOAT3(x, 0.0f, z);
				geoData.Vertices[index].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
				geoData.Vertices[index].TangentU = XMFLOAT3(1.0f, 0.0f, 0.0f);
				geoData.Vertices[index].TexC.x = j * du;
				geoData.Vertices[index].TexC.y = i * dv;
			}
		}

		geoData.Indices.resize(faceCount * 3); // 3 indices per face

		// Iterate over each quad and compute indices.
		UINT k = 0;
		for (UINT i = 0; i < ysegments; ++i)
		{
			for (UINT j = 0; j < xsegments; ++j)
			{
				geoData.Indices[k] = i * (xsegments + 1) + j;
				geoData.Indices[k + 1] = i * (xsegments + 1) + j + 1;
				geoData.Indices[k + 2] = (i + 1) * (xsegments + 1) + j;

				geoData.Indices[k + 3] = (i + 1) * (xsegments + 1) + j;
				geoData.Indices[k + 4] = i * (xsegments + 1) + j + 1;
				geoData.Indices[k + 5] = (i + 1) * (xsegments + 1) + j + 1;

				k += 6; // next quad
			}
		}
	}

	void CGeometryCreator::createSphereData(f32 radius, u32 sliceCount,
		u32 stackCount, SGeometryData& geoData)
	{
		geoData.Vertices.clear();
		geoData.Indices.clear();

		//
		// Compute the vertices stating at the top pole and moving down the stacks.
		//

		// Poles: note that there will be texture coordinate distortion as there is
		// not a unique point on the texture map to assign to the pole when mapping
		// a rectangular texture onto a sphere.
		SGeometryVertex topVertex(0.0f, +radius, 0.0f, 0.0f, +1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		SGeometryVertex bottomVertex(0.0f, -radius, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

		geoData.Vertices.push_back(topVertex);

		float phiStep = XM_PI / stackCount;
		float thetaStep = 2.0f*XM_PI / sliceCount;

		// Compute vertices for each stack ring (do not count the poles as rings).
		for (UINT i = 1; i <= stackCount - 1; ++i)
		{
			float phi = i*phiStep;

			// Vertices of ring.
			for (UINT j = 0; j <= sliceCount; ++j)
			{
				float theta = j*thetaStep;

				SGeometryVertex v;

				// spherical to cartesian
				v.Position.x = radius*sinf(phi)*cosf(theta);
				v.Position.y = radius*cosf(phi);
				v.Position.z = radius*sinf(phi)*sinf(theta);

				// Partial derivative of P with respect to theta
				v.TangentU.x = -radius*sinf(phi)*sinf(theta);
				v.TangentU.y = 0.0f;
				v.TangentU.z = +radius*sinf(phi)*cosf(theta);

				XMVECTOR T = XMLoadFloat3(&v.TangentU);
				XMStoreFloat3(&v.TangentU, XMVector3Normalize(T));

				XMVECTOR p = XMLoadFloat3(&v.Position);
				XMStoreFloat3(&v.Normal, XMVector3Normalize(p));

				v.TexC.x = theta / XM_2PI;
				v.TexC.y = phi / XM_PI;

				geoData.Vertices.push_back(v);
			}
		}

		geoData.Vertices.push_back(bottomVertex);

		//
		// Compute indices for top stack.  The top stack was written first to the vertex buffer
		// and connects the top pole to the first ring.
		//

		for (UINT i = 1; i <= sliceCount; ++i)
		{
			geoData.Indices.push_back(0);
			geoData.Indices.push_back(i + 1);
			geoData.Indices.push_back(i);
		}

		//
		// Compute indices for inner stacks (not connected to poles).
		//

		// Offset the indices to the index of the first vertex in the first ring.
		// This is just skipping the top pole vertex.
		UINT baseIndex = 1;
		UINT ringVertexCount = sliceCount + 1;
		for (UINT i = 0; i < stackCount - 2; ++i)
		{
			for (UINT j = 0; j < sliceCount; ++j)
			{
				geoData.Indices.push_back(baseIndex + i*ringVertexCount + j);
				geoData.Indices.push_back(baseIndex + i*ringVertexCount + j + 1);
				geoData.Indices.push_back(baseIndex + (i + 1)*ringVertexCount + j);

				geoData.Indices.push_back(baseIndex + (i + 1)*ringVertexCount + j);
				geoData.Indices.push_back(baseIndex + i*ringVertexCount + j + 1);
				geoData.Indices.push_back(baseIndex + (i + 1)*ringVertexCount + j + 1);
			}
		}

		//
		// Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer
		// and connects the bottom pole to the bottom ring.
		//

		// South pole vertex was added last.
		UINT southPoleIndex = (UINT)geoData.Vertices.size() - 1;

		// Offset the indices to the index of the first vertex in the last ring.
		baseIndex = southPoleIndex - ringVertexCount;

		for (UINT i = 0; i < sliceCount; ++i)
		{
			geoData.Indices.push_back(southPoleIndex);
			geoData.Indices.push_back(baseIndex + i);
			geoData.Indices.push_back(baseIndex + i + 1);
		}

		geoData.Aabb.Center = XMFLOAT3(0, 0, 0);
		geoData.Aabb.Extents = XMFLOAT3(radius, radius, radius);
	}

	void CGeometryCreator::createQuadData(XMFLOAT3 vertices[], math::SAxisAlignedBox& aabb)
	{
		vertices[0] = XMFLOAT3(-1.0f, 1.0f, 0.0f);
		vertices[1] = XMFLOAT3(1.0f, 1.0f, 0.0f);
		vertices[2] = XMFLOAT3(1.0f, -1.0f, 0.0f);

		vertices[3] = XMFLOAT3(-1.0f, 1.0f, 0.0f);
		vertices[4] = XMFLOAT3(1.0f, -1.0f, 0.0f);
		vertices[5] = XMFLOAT3(-1.0f, -1.0f, 0.0f);

		aabb.Center = XMFLOAT3(0, 0, 0);
		aabb.Extents = XMFLOAT3(1.0f, 1.0f, 0.01f);
	}

	void CGeometryCreator::createCylinderData(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, SGeometryData& meshData)
	{
		meshData.Vertices.clear();
		meshData.Indices.clear();

		//
		// Build Stacks.
		// 

		float stackHeight = height / stackCount;

		// Amount to increment radius as we move up each stack level from bottom to top.
		float radiusStep = (topRadius - bottomRadius) / stackCount;

		UINT ringCount = stackCount + 1;

		// Compute vertices for each stack ring starting at the bottom and moving up.
		for (UINT i = 0; i < ringCount; ++i)
		{
			float y = -0.5f*height + i*stackHeight;
			float r = bottomRadius + i*radiusStep;

			// vertices of ring
			float dTheta = 2.0f*XM_PI / sliceCount;
			for (UINT j = 0; j <= sliceCount; ++j)
			{
				SGeometryVertex vertex;

				float c = cosf(j*dTheta);
				float s = sinf(j*dTheta);

				vertex.Position = XMFLOAT3(r*c, y, r*s);

				vertex.TexC.x = (float)j / sliceCount;
				vertex.TexC.y = 1.0f - (float)i / stackCount;

				// Cylinder can be parameterized as follows, where we introduce v
				// parameter that goes in the same direction as the v tex-coord
				// so that the bitangent goes in the same direction as the v tex-coord.
				//   Let r0 be the bottom radius and let r1 be the top radius.
				//   y(v) = h - hv for v in [0,1].
				//   r(v) = r1 + (r0-r1)v
				//
				//   x(t, v) = r(v)*cos(t)
				//   y(t, v) = h - hv
				//   z(t, v) = r(v)*sin(t)
				// 
				//  dx/dt = -r(v)*sin(t)
				//  dy/dt = 0
				//  dz/dt = +r(v)*cos(t)
				//
				//  dx/dv = (r0-r1)*cos(t)
				//  dy/dv = -h
				//  dz/dv = (r0-r1)*sin(t)

				// This is unit length.
				vertex.TangentU = XMFLOAT3(-s, 0.0f, c);

				float dr = bottomRadius - topRadius;
				XMFLOAT3 bitangent(dr*c, -height, dr*s);

				XMVECTOR T = XMLoadFloat3(&vertex.TangentU);
				XMVECTOR B = XMLoadFloat3(&bitangent);
				XMVECTOR N = XMVector3Normalize(XMVector3Cross(T, B));
				XMStoreFloat3(&vertex.Normal, N);

				meshData.Vertices.push_back(vertex);
			}
		}

		// Add one because we duplicate the first and last vertex per ring
		// since the texture coordinates are different.
		UINT ringVertexCount = sliceCount + 1;

		// Compute indices for each stack.
		for (UINT i = 0; i < stackCount; ++i)
		{
			for (UINT j = 0; j < sliceCount; ++j)
			{
				meshData.Indices.push_back(i*ringVertexCount + j);
				meshData.Indices.push_back((i + 1)*ringVertexCount + j);
				meshData.Indices.push_back((i + 1)*ringVertexCount + j + 1);

				meshData.Indices.push_back(i*ringVertexCount + j);
				meshData.Indices.push_back((i + 1)*ringVertexCount + j + 1);
				meshData.Indices.push_back(i*ringVertexCount + j + 1);
			}
		}

		buildCylinderTopCap(bottomRadius, topRadius, height, sliceCount, stackCount, meshData);
		buildCylinderBottomCap(bottomRadius, topRadius, height, sliceCount, stackCount, meshData);
	
		meshData.Aabb.Center = XMFLOAT3(0, 0, 0);
		meshData.Aabb.Extents = XMFLOAT3(max(bottomRadius, topRadius), height / 2, max(bottomRadius, topRadius));
	}

	void CGeometryCreator::buildCylinderTopCap(float bottomRadius, float topRadius, float height,
		UINT sliceCount, UINT stackCount, SGeometryData& meshData)
	{
		UINT baseIndex = (UINT)meshData.Vertices.size();

		float y = 0.5f*height;
		float dTheta = 2.0f*XM_PI / sliceCount;

		// Duplicate cap ring vertices because the texture coordinates and normals differ.
		for (UINT i = 0; i <= sliceCount; ++i)
		{
			float x = topRadius*cosf(i*dTheta);
			float z = topRadius*sinf(i*dTheta);

			// Scale down by the height to try and make top cap texture coord area
			// proportional to base.
			float u = x / height + 0.5f;
			float v = z / height + 0.5f;

			meshData.Vertices.push_back(SGeometryVertex(x, y, z, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, u, v));
		}

		// Cap center vertex.
		meshData.Vertices.push_back(SGeometryVertex(0.0f, y, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f));

		// Index of center vertex.
		UINT centerIndex = (UINT)meshData.Vertices.size() - 1;

		for (UINT i = 0; i < sliceCount; ++i)
		{
			meshData.Indices.push_back(centerIndex);
			meshData.Indices.push_back(baseIndex + i + 1);
			meshData.Indices.push_back(baseIndex + i);
		}
	}

	void CGeometryCreator::buildCylinderBottomCap(float bottomRadius, float topRadius, float height,
		UINT sliceCount, UINT stackCount, SGeometryData& meshData)
	{
		// 
		// Build bottom cap.
		//

		UINT baseIndex = (UINT)meshData.Vertices.size();
		float y = -0.5f*height;

		// vertices of ring
		float dTheta = 2.0f*XM_PI / sliceCount;
		for (UINT i = 0; i <= sliceCount; ++i)
		{
			float x = bottomRadius*cosf(i*dTheta);
			float z = bottomRadius*sinf(i*dTheta);

			// Scale down by the height to try and make top cap texture coord area
			// proportional to base.
			float u = x / height + 0.5f;
			float v = z / height + 0.5f;

			meshData.Vertices.push_back(SGeometryVertex(x, y, z, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, u, v));
		}

		// Cap center vertex.
		meshData.Vertices.push_back(SGeometryVertex(0.0f, y, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f));

		// Cache the index of center vertex.
		UINT centerIndex = (UINT)meshData.Vertices.size() - 1;

		for (UINT i = 0; i < sliceCount; ++i)
		{
			meshData.Indices.push_back(centerIndex);
			meshData.Indices.push_back(baseIndex + i);
			meshData.Indices.push_back(baseIndex + i + 1);
		}
	}

}