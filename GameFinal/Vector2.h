#ifndef __GF_VECTOR2_CLASS_H__
#define __GF_VECTOR2_CLASS_H__

#include "MathCommon.h"
#include <math.h>
namespace gf
{
	template<class T>
	class Vector2
	{
	public:
		T x, y;

		Vector2()
		{
			x = 0;
			y = 0;
		}

		Vector2& operator=(const Vector2& t)
		{
			x = t.x;
			y = t.y;
			return *this;
		}

		Vector2(T x, T y) :x(x), y(y) {}

		bool operator==(const Vector2& t) const {
			return x == t.x && y == t.y;
		}

		bool operator !=(const Vector2 &a) const {
			return x != a.x || y != a.y;
		}

		Vector2 operator -() const { return Vector2(-x, -y); }

		Vector2 operator +(const Vector2 &a) const {
			return Vector2(x + a.x, y + a.y);
		}

		Vector2 operator -(const Vector2 &a) const {
			return Vector2(x - a.x, y - a.y);
		}

		Vector2 operator *(T k) const {
			return Vector2(x*k, y*k);
		}

		Vector2 operator *(const Vector2 &a) const {
			return Vector2(x * a.x, y * a.y);
		}

		Vector2 operator /(T a) const {
			float	oneOverA = 1.0f / a; 
			return Vector2(x*oneOverA, y*oneOverA);
		}

		Vector2& operator +=(const Vector2 &a) {
			x += a.x; y += a.y;
			return *this;
		}

		Vector2& operator -=(const Vector2 &a) {
			x -= a.x; y -= a.y; 
			return *this;
		}

		Vector2& operator *=(T a) {
			x *= a; y *= a; 
			return *this;
		}

		Vector2& operator /=(T a) {
			float	oneOverA = 1.0f / a;
			x *= oneOverA; y *= oneOverA;
			return *this;
		}

		void normalize() {
			T magSq = x*x + y*y;
			if (magSq > 0.0f) { // check for divide-by-zero
				float oneOverMag = 1.0f / sqrt(magSq);
				x *= oneOverMag;
				y *= oneOverMag;
			}
		}

		float length() const {
			return sqrt(x*x + y*y);
		}

		T sqrLength() const {
			return x*x + y*y;
		}

		T dot(const Vector2& a) const {
			return x*a.x + y*a.y;
		}

		float distance(const Vector2 &b) const {
			T dx = x - b.x;
			T dy = y - b.y;
			return sqrt(dx*dx + dy*dy);
		}

		T distanceSquared(const Vector2& b) const {
			T dx = x - b.x;
			T dy = y - b.y;
			return (dx*dx + dy*dy);
		}
	};

	template<class T>
	inline T dot(const Vector2<T> &a, const Vector2<T> &b)
	{
		return a.dot(b);
	}

	template<>
	Vector2<double> Vector2<double>::operator/(double a) const {
		double oneOverA = 1.0 / a; 
		return Vector2<double>(x*oneOverA, y*oneOverA);
	}

	template<>
	inline bool Vector2<float>::operator==(const Vector2<float>& t) const {
		return math::FloatEqual(x, t.x) &&
			math::FloatEqual(y, t.y);
	}

	template<>
	inline bool Vector2<double>::operator==(const Vector2<double>& t) const {
		return math::DoubleEqual(x, t.x) &&
			math::DoubleEqual(y, t.y);
	}

	template<>
	inline bool Vector2<float>::operator!=(const Vector2<float>& t) const {
		return !math::FloatEqual(x, t.x) ||
			!math::FloatEqual(y, t.y);
	}

	template<>
	inline bool Vector2<double>::operator!=(const Vector2<double>& t) const {
		return !math::DoubleEqual(x, t.x) ||
			!math::DoubleEqual(y, t.y);
	}

	template<>
	void Vector2<double>::normalize() {
		double magSq = x*x + y*y;
		if (magSq > 0.0) { // check for divide-by-zero
			double oneOverMag = 1.0 / sqrt(magSq);
			x *= oneOverMag;
			y *= oneOverMag;
		}
	}

	using Vector2f = Vector2<float>;
	using Vector2i = Vector2<int>;
	using Vector2d = Vector2<double>;
}

#endif
