#include "stdafx.h"
#include "gfMath.h"

XMMATRIX Math::InverseTranspose(CXMMATRIX M)
{
	XMMATRIX A = M;
	A.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR det = XMMatrixDeterminant(A);
	return XMMatrixTranspose(XMMatrixInverse(&det, A));
}

bool Math::FloatEqual(f32 a, f32 b, f32 e)
{
	return a - b <= e || a - b >= -e;
}

bool Math::FloatArrayEqual(const f32 color1[], const f32 color2[], u32 elementCount, f32 e)
{
	for (u32 i = 0; i < elementCount; i++)
	{
		if (!FloatEqual(color1[i], color2[i]))
			return false;
	}

	return true;
}
