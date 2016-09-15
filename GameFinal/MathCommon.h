#ifndef __GF_MATH_COMMON_H__
#define __GF_MATH_COMMON_H__

namespace gf
{
	namespace math
	{
		inline bool FloatEqual(float a, float b, float elipson = 0.00001f)
		{
			return a - b <= elipson && a - b >= -elipson;
		}

		inline bool DoubleEqual(double a, double b, double epsilon = 0.000001)
		{
			return a - b <= epsilon && a - b >= -epsilon;
		}

	}
}

#endif