#ifndef __GF_MATH_H__
#define __GF_MATH_H__

#include "xnacollision/xnacollision.h"

namespace gf
{
	/*
	class Math
	{
	public:
		static XMMATRIX InverseTranspose(CXMMATRIX M);

		static bool FloatEqual(f32 a, f32 b, f32 elipson = 0.00001f);

		static bool FloatArrayEqual(const f32 color1[], const f32 color2[], u32 elementCount = 4, f32 e = 0.00001f);
	};
	*/

	namespace math
	{
		enum E_INTERSECT_STATE
		{
			EIS_INSIDE,
			EIS_OUTSIDE,
			EIS_INTERSECTING
		};

		struct SAxisAlignedBox
		{
			XMFLOAT3				Center;            // Center of the box.
			XMFLOAT3				Extents;           // Distance from the center to each side.

			XNA::AxisAlignedBox getXnaAxisAlignedBox() const
			{
				XNA::AxisAlignedBox aabb;
				aabb.Center = Center;
				aabb.Extents = Extents;
				return aabb;
			}
		};

		struct SOrientedBox
		{
			XMFLOAT3				Center;
			XMFLOAT3				Extents;
			XMFLOAT3				Axis[3];

			XNA::OrientedBox getXnaOrientedBox() const
			{
				XNA::OrientedBox obb;
				obb.Center = Center;
				obb.Extents = Extents;

				XMMATRIX rot = XMLoadFloat3x3(reinterpret_cast<const XMFLOAT3X3*>(&Axis[0]));
				XMFLOAT4X4 M;
				XMStoreFloat4x4(&M, rot);

				XMVECTOR quaternion = XMQuaternionRotationMatrix(rot);
				XMStoreFloat4(&obb.Orientation, quaternion);
				return obb;
			}
		};

		struct SSphere
		{
			XMFLOAT3				Center;
			f32						Radius;

			XNA::Sphere getXnaSphere() const
			{
				XNA::Sphere sphere;
				sphere.Center = Center;
				sphere.Radius = Radius;
				return sphere;
			}
		};

		struct SFrustum
		{
			union
			{
				struct
				{
					XMFLOAT4	LeftPlane;
					XMFLOAT4	RightPlane;
					XMFLOAT4	TopPlane;
					XMFLOAT4	BottomPlane;
					XMFLOAT4	NearPlane;
					XMFLOAT4	FarPlane;
				};
				struct
				{
					XMFLOAT4		Planes[6];
				};
			};
		};

		XMMATRIX InverseTranspose(CXMMATRIX M);

		bool FloatEqual(f32 a, f32 b, f32 elipson = 0.00001f);

		bool FloatArrayEqual(const f32 color1[], const f32 color2[], u32 elementCount = 4, f32 e = 0.00001f);

		inline f32 RandomFloat(f32 a, f32 b)
		{
			// get a float from 0 to 1
			f32 i = rand() % 10000 / 10000.0f;
			return i * (b - a) + a;
		}

		E_INTERSECT_STATE IntersectAabbPlane(const SAxisAlignedBox& aabb, const XMFLOAT4& plane);

		E_INTERSECT_STATE IntersectOrientedBoxPlane(const SOrientedBox& obb, const XMFLOAT4& plane);
		
		//VOID ComputeBoundingAxisAlignedBoxFromPoints( AxisAlignedBox* pOut, UINT Count, const XMFLOAT3* pPoints, UINT Stride );

		E_INTERSECT_STATE IntersectOrientedBoxFrustum(const SOrientedBox& obb, const SFrustum& frustum);

		E_INTERSECT_STATE IntersectAabbFrustum(const SAxisAlignedBox& aabb, const SFrustum& frustum);

		E_INTERSECT_STATE IntersectSphereFrustum(const SSphere& sphere, const SFrustum& frustum);

		bool IntersectTwoAabbs(const SAxisAlignedBox& aabb1, const SAxisAlignedBox& aabb2);

		bool IntersectAabbAndOBB();

		void ComputeAabbFromPoints(SAxisAlignedBox* aabb, u32 count, const XMFLOAT3 points[]);

		void ComputeAabbFromOrientedBox(SAxisAlignedBox* aabb, const SOrientedBox& obb);

		void intersectOrientedBoxOrientedBox(const SOrientedBox& obb1, const SOrientedBox& obb2);
	}
}

#endif
