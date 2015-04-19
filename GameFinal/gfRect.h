#ifndef __GF_RECT_H__
#define __GF_RECT_H__
namespace gf
{
	namespace math
	{
		template <class T>
		class Rect
		{
		public:
			Rect(T x1, T y1, T x2, T y2)
				:x1(x1), y1(y1), x2(x2), y2(y2)
			{

			}

			T getWidth() const {
				return x2 - x1;
			}

			T getHeight() const {
				return y2 - y1;
			}

		public:
			T		x1, y1, x2, y2;

		};
	}
}

#endif