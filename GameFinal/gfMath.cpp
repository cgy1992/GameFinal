#include "stdafx.h"
#include "gfMath.h"

namespace gf
{
	namespace math
	{
		XMMATRIX InverseTranspose(CXMMATRIX M)
		{
			XMMATRIX A = M;
			A.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
			XMVECTOR det = XMMatrixDeterminant(A);
			return XMMatrixTranspose(XMMatrixInverse(&det, A));
		}



		bool FloatArrayEqual(const f32 color1[], const f32 color2[], u32 elementCount, f32 e)
		{
			for (u32 i = 0; i < elementCount; i++)
			{
				if (!FloatEqual(color1[i], color2[i]))
					return false;
			}

			return true;
		}

		static inline BOOL XMVector3AnyTrue(FXMVECTOR V)
		{
			XMVECTOR C;

			// Duplicate the fourth element from the first element.
			C = XMVectorSwizzle(V, 0, 1, 2, 0);

			return XMComparisonAnyTrue(XMVector4EqualIntR(C, XMVectorTrueInt()));
		}


		E_INTERSECT_STATE IntersectAabbPlane(const SAxisAlignedBox& aabb, const XMFLOAT4& plane)
		{
			/*
			e = hx*|nx| + hy*|ny| + hz*|nz|
			note:   (hx,hy,hz) is the half size of the obb
					(nx, ny, nz) is the normal of the plane

			s = dot(c * n) + d;
			note:	c is the center of the obb
			n is the normal of the plane
			d is the constant term of the plane

			if s - e > 0 then INSIDE
			if s + e < 0 then OUTSIDE
			else INTERSECTING
			*/

			f32 e = aabb.Extents.x * fabs(plane.x)
				+ aabb.Extents.y * fabs(plane.y)
				+ aabb.Extents.z * fabs(plane.z);

			f32 s = aabb.Center.x * plane.x + aabb.Center.y * plane.y + aabb.Center.z * plane.z + plane.w;
			
			if (s - e > 0)
				return EIS_INSIDE;
			if (s + e < 0)
				return EIS_OUTSIDE;

			return EIS_INTERSECTING;
		}

		


		E_INTERSECT_STATE IntersectOrientedBoxPlane(const SOrientedBox& obb, const XMFLOAT4& plane)
		{
			/*
			e = hx*|(n*bu)| + hy*|(n*bv)| + hz*|dot(n*bw)|
			note:   (hx,hy,hz) is the half size of the obb
			n is the normal of the plane
			bu,bv,bw is the coordinate system axes of the obb

			s = dot(c * n) + d;
			note:	c is the center of the obb
			n is the normal of the plane
			d is the constant term of the plane

			if s - e > 0 then INSIDE
			if s + e < 0 then OUTSIDE
			else INTERSECTING
			*/

			XMVECTOR n = XMLoadFloat4(&plane);
			XMVECTOR bu = XMLoadFloat3(&obb.Axis[0]);
			XMVECTOR bv = XMLoadFloat3(&obb.Axis[1]);
			XMVECTOR bw = XMLoadFloat3(&obb.Axis[2]);
			XMVECTOR c = XMLoadFloat3(&obb.Center);

			f32 e = obb.Extents.x * fabs(XMVectorGetX(XMVector3Dot(n, bu)))
				+ obb.Extents.y * fabs(XMVectorGetX(XMVector3Dot(n, bv)))
				+ obb.Extents.z * fabs(XMVectorGetX(XMVector3Dot(n, bw)));

			f32 s = XMVectorGetX(XMVector3Dot(c, n)) + plane.w;
			if (s - e > 0)
				return EIS_INSIDE;
			if (s + e < 0)
				return EIS_OUTSIDE;

			return EIS_INTERSECTING;

		}


		E_INTERSECT_STATE IntersectAabbFrustum(const SAxisAlignedBox& aabb, const SFrustum& frustum)
		{
			bool bIntersect = false;
			for (u32 i = 0; i < 6; i++)
			{
				E_INTERSECT_STATE state = IntersectAabbPlane(aabb, frustum.Planes[i]);
				if (state == EIS_OUTSIDE)
					return EIS_OUTSIDE;

				if (state == EIS_INTERSECTING)
					bIntersect = true;
			}

			if (bIntersect)
				return EIS_INTERSECTING;

			return EIS_INSIDE;
		}


		E_INTERSECT_STATE IntersectOrientedBoxFrustum(const SOrientedBox& obb, const SFrustum& frustum)
		{
			bool bIntersect = false;
			for (u32 i = 0; i < 6; i++)
			{
				E_INTERSECT_STATE state = IntersectOrientedBoxPlane(obb, frustum.Planes[i]);
				if (state == EIS_OUTSIDE)
					return EIS_OUTSIDE;

				if (state == EIS_INTERSECTING)
					bIntersect = true;
			}

			if (bIntersect)
				return EIS_INTERSECTING;

			return EIS_INSIDE;
		}

		

		void ComputeAabbFromPoints(SAxisAlignedBox* aabb, u32 count, const XMFLOAT3 points[])
		{
			XMFLOAT3 maxPoint(-FLT_MAX, -FLT_MAX, -FLT_MAX);
			XMFLOAT3 minPoint(FLT_MAX, FLT_MAX, FLT_MAX);

			for (u32 i = 0; i < count; i++)
			{
				XMFLOAT3 point = points[i];
				if (point.x > maxPoint.x)
					maxPoint.x = point.x;
				if (point.x < minPoint.x)
					minPoint.x = point.x;

				if (point.y > maxPoint.y)
					maxPoint.y = point.y;
				if (point.y < minPoint.y)
					minPoint.y = point.y;

				if (point.z > maxPoint.z)
					maxPoint.z = point.z;
				if (point.z < minPoint.z)
					minPoint.z = point.z;
			}

			aabb->Center = XMFLOAT3((maxPoint.x + minPoint.x) * 0.5f,
				(maxPoint.y + minPoint.y) * 0.5f,
				(maxPoint.z + minPoint.z) * 0.5f);

			aabb->Extents = XMFLOAT3((maxPoint.x - minPoint.x) * 0.5f,
				(maxPoint.y - minPoint.y) * 0.5f,
				(maxPoint.z - minPoint.z) * 0.5f);
		}

		void ComputeAabbFromOrientedBox(SAxisAlignedBox* aabb, const SOrientedBox& obb)
		{
			XMFLOAT3 points[8];
			XMVECTOR bu = XMLoadFloat3(&obb.Axis[0]);
			XMVECTOR bv = XMLoadFloat3(&obb.Axis[1]);
			XMVECTOR bw = XMLoadFloat3(&obb.Axis[2]);

			bu = XMVectorScale(bu, obb.Extents.x);
			bv = XMVectorScale(bv, obb.Extents.y);
			bw = XMVectorScale(bw, obb.Extents.z);

			XMStoreFloat3(&points[0], bu + bv + bw);
			XMStoreFloat3(&points[1], bu + bv - bw);
			XMStoreFloat3(&points[2], bu - bv + bw);
			XMStoreFloat3(&points[3], bu - bv - bw);
			XMStoreFloat3(&points[4], -bu + bv + bw);
			XMStoreFloat3(&points[5], -bu + bv - bw);
			XMStoreFloat3(&points[6], -bu - bv + bw);
			XMStoreFloat3(&points[7], -bu - bv - bw);

			XMFLOAT3 maxPoint(-FLT_MAX, -FLT_MAX, -FLT_MAX);
			XMFLOAT3 minPoint(FLT_MAX, FLT_MAX, FLT_MAX);

			for (u32 i = 0; i < 8; i++)
			{
				XMFLOAT3 point = points[i];
				if (point.x > maxPoint.x)
					maxPoint.x = point.x;
				if (point.x < minPoint.x)
					minPoint.x = point.x;

				if (point.y > maxPoint.y)
					maxPoint.y = point.y;
				if (point.y < minPoint.y)
					minPoint.y = point.y;

				if (point.z > maxPoint.z)
					maxPoint.z = point.z;
				if (point.z < minPoint.z)
					minPoint.z = point.z;
			}

			aabb->Center = obb.Center;
			aabb->Extents = XMFLOAT3((maxPoint.x - minPoint.x) * 0.5f,
				(maxPoint.y - minPoint.y) * 0.5f,
				(maxPoint.z - minPoint.z) * 0.5f);
		}

		E_INTERSECT_STATE IntersectSphereFrustum(const SSphere& sphere, const SFrustum& frustum)
		{
			XMFLOAT4 c(sphere.Center.x, sphere.Center.y, sphere.Center.z, 1.0f);
			XMVECTOR center = XMLoadFloat4(&c);

			bool bIntersect = false;
			for (u32 i = 0; i < 6; i++)
			{
				XMVECTOR plane = XMLoadFloat4(&frustum.Planes[i]);
				XMVECTOR dist = XMVector4Dot(center, plane);
				f32 r = XMVectorGetX(dist);
				
				if (r < -sphere.Radius)
					return EIS_OUTSIDE;

				if (r < sphere.Radius)
					bIntersect = true;
			}

			return bIntersect ? EIS_INTERSECTING : EIS_INSIDE;
		}

		bool IntersectTwoAabbs(const SAxisAlignedBox& aabb1, const SAxisAlignedBox& aabb2)
		{
			XMVECTOR CenterA = XMLoadFloat3(&aabb1.Center);
			XMVECTOR ExtentsA = XMLoadFloat3(&aabb1.Extents);

			XMVECTOR CenterB = XMLoadFloat3(&aabb2.Center);
			XMVECTOR ExtentsB = XMLoadFloat3(&aabb2.Extents);

			XMVECTOR MinA = CenterA - ExtentsA;
			XMVECTOR MaxA = CenterA + ExtentsA;

			XMVECTOR MinB = CenterB - ExtentsB;
			XMVECTOR MaxB = CenterB + ExtentsB;

			// for each i in (x, y, z) if a_min(i) > b_max(i) or b_min(i) > a_max(i) then return FALSE
			XMVECTOR Disjoint = XMVectorOrInt(XMVectorGreater(MinA, MaxB), XMVectorGreater(MinB, MaxA));

			return !XMVector3AnyTrue(Disjoint);
		}

		void ComputeFrustumFromCorners(const XMFLOAT3 corners[])
		{
			

		}

		/*
		XMFLOAT3 ComputeMirrorPointAboutPlane(const XMFLOAT4& plane, const XMFLOAT3& p)
		{
			XMVECTOR plane_v = XMLoadFloat4(&plane);
			XMVECTOR p_v = XMVectorSet(p.x, p.y, p.z, 1.0);
			XMVECTOR dot_v = XMVector4Dot(plane_v, p_v);

			XMVECTOR mirror_v = XMVectorMultiplyAdd(-2.0f * dot_v, plane_v, p_v);
			XMFLOAT3 mirror_point;
			XMStoreFloat3(&mirror_point, mirror_v);
			return mirror_point;
		}
		*/

	}
}


