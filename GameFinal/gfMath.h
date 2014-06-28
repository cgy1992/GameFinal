#ifndef __GF_MATH_H__
#define __GF_MATH_H__

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
		};

		struct SOrientedBox
		{
			XMFLOAT3				Center;
			XMFLOAT3				Extents;
			XMFLOAT3				Axis[3];
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

		E_INTERSECT_STATE IntersectAabbPlane(const SAxisAlignedBox& aabb, const XMFLOAT4& plane);

		E_INTERSECT_STATE IntersectOrientedBoxPlane(const SOrientedBox& obb, const XMFLOAT4& plane);
		
		//VOID ComputeBoundingAxisAlignedBoxFromPoints( AxisAlignedBox* pOut, UINT Count, const XMFLOAT3* pPoints, UINT Stride );

		E_INTERSECT_STATE IntersectOrientedBoxFrustum(const SOrientedBox& obb, const SFrustum& frustum);

		E_INTERSECT_STATE IntersectAabbFrustum(const SAxisAlignedBox& aabb, const SFrustum& frustum);

		void ComputeAabbFromPoints(SAxisAlignedBox* aabb, u32 count, const XMFLOAT3 points[]);

		void ComputeAabbFromOrientedBox(SAxisAlignedBox* aabb, const SOrientedBox& obb);
	}
}

#endif
