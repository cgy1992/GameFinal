#ifndef __GF_MATH_H__
#define __GF_MATH_H__

struct SAxisAlignedBox
{
	XMFLOAT3				Center;            // Center of the box.
	XMFLOAT3				Extents;           // Distance from the center to each side.
};


class Math
{
public:
	static XMMATRIX InverseTranspose(CXMMATRIX M);

	static bool FloatEqual(f32 a, f32 b, f32 elipson = 0.00001f);

	static bool FloatArrayEqual(const f32 color1[], const f32 color2[], u32 elementCount = 4, f32 e = 0.00001f);
};

#endif
