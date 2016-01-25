#ifndef __GF_MATH_HEADER__
#define __GF_MATH_HEADER__

#include "xnacollision/xnacollision.h"

namespace gf
{
#ifdef GAMEFINAL_EXPORTS
#define GAMEFINAL_API __declspec(dllexport)
#else
#define GAMEFINAL_API __declspec(dllimport)
#endif

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

		struct SRay
		{
			XMFLOAT3	Origin;
			XMFLOAT3	Direction;
		};

		inline XMFLOAT4 VectorAdd(const XMFLOAT4& a, const XMFLOAT4& b)
		{
			return XMFLOAT4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
		}

		inline XMFLOAT3 VectorAdd(const XMFLOAT3& a, const XMFLOAT3& b)
		{
			return XMFLOAT3(a.x + b.x, a.y + b.y, a.z + b.z);
		}

		inline XMFLOAT4 VectorAdd(const XMFLOAT4& a, const XMFLOAT3& b)
		{
			return XMFLOAT4(a.x + b.x, a.y + b.y, a.z + b.z, a.w);
		}

		inline XMFLOAT4 VectorAdd(const XMFLOAT3& a, const XMFLOAT4& b)
		{
			return XMFLOAT4(a.x + b.x, a.y + b.y, a.z + b.z, b.w);
		}

		inline XMFLOAT4 VectorMinus(const XMFLOAT4& a, const XMFLOAT4& b)
		{
			return XMFLOAT4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
		}

		inline XMFLOAT3 VectorMinus(const XMFLOAT3& a, const XMFLOAT3& b)
		{
			return XMFLOAT3(a.x - b.x, a.y - b.y, a.z - b.z);
		}

		inline XMFLOAT3 VectorMultiply(const XMFLOAT3& v, f32 k)
		{
			return XMFLOAT3(v.x*k, v.y*k, v.z*k);
		}

		inline XMFLOAT4 VectorMultiply(const XMFLOAT4& v, f32 k)
		{
			return XMFLOAT4(v.x*k, v.y*k, v.z*k, v.w*k);
		}

		inline XMFLOAT4 VectorMultiply(const XMFLOAT4& a, const XMFLOAT4& b)
		{
			return XMFLOAT4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
		}

		inline XMFLOAT3 VectorMultiply(const XMFLOAT3& a, const XMFLOAT3& b)
		{
			return XMFLOAT3(a.x * b.x, a.y * b.y, a.z * b.z);
		}
		
		inline XMFLOAT3 VectorOpposite(const XMFLOAT3& a)
		{
			return XMFLOAT3(-a.x, -a.y, -a.z);
		}

		inline XMFLOAT4 VectorOpposite(const XMFLOAT4& a)
		{
			return XMFLOAT4(-a.x, -a.y, -a.z, -a.w);
		}

		inline f32 VectorDistance(const XMFLOAT3& a, const XMFLOAT3& b) 
		{
			return sqrtf((a.x - b.x) * (a.x - b.x)
				+ (a.y - b.y) * (a.y - b.y)
				+ (a.z - b.z) * (a.z - b.z));
		}

		inline f32 VectorSqrDistance(const XMFLOAT3& a, const XMFLOAT3& b)
		{
			return (a.x - b.x) * (a.x - b.x)
				+ (a.y - b.y) * (a.y - b.y)
				+ (a.z - b.z) * (a.z - b.z);
		}

		inline f32 VectorDot(const XMFLOAT3& a, const XMFLOAT3& b) {
			return a.x * b.x + a.y * b.y + a.z * b.z;
		}

		inline XMFLOAT3 VectorNormalize(const XMFLOAT3& n) {
			XMVECTOR v = XMLoadFloat3(&n);
			v = XMVector3Normalize(v);
			XMFLOAT3 vec;
			XMStoreFloat3(&vec, v); 
			return vec;
		}

		inline XMFLOAT3 GetNormalizedVector(f32 x, f32 y, f32 z)
		{
			XMVECTOR v = XMVectorSet(x, y, z, 0);
			v = XMVector3Normalize(v);
			XMFLOAT3 vec;
			XMStoreFloat3(&vec, v);
			return vec;
		}

		XMMATRIX InverseTranspose(CXMMATRIX M);

		inline bool FloatEqual(f32 a, f32 b, f32 elipson = 0.00001f)
		{
			return a - b <= elipson || a - b >= -elipson;
		}

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

		void ComputeFrustumFromCorners(const XMFLOAT3 corners[]);

		inline XMFLOAT3 ComputeMirrorPointAboutPlane(const XMFLOAT4& plane, const XMFLOAT3& p)
		{
			XMVECTOR plane_v = XMLoadFloat4(&plane);
			XMVECTOR p_v = XMVectorSet(p.x, p.y, p.z, 1.0);
			XMVECTOR dot_v = XMVector4Dot(plane_v, p_v);

			XMVECTOR mirror_v = XMVectorMultiplyAdd(-2.0f * dot_v, plane_v, p_v);
			XMFLOAT3 mirror_point;
			XMStoreFloat3(&mirror_point, mirror_v);
			return mirror_point;
		}

		

	}
}


#endif
