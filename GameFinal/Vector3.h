#ifndef __GF_VECTOR3_CLASS_H__
#define __GF_VECTOR3_CLASS_H__
#include <math.h>
#include "MathCommon.h"

namespace gf
{
	template<class T>
	class Vector3
	{
	public:

		T x, y, z;

		Vector3()
		{
			x = 0;
			y = 0;
			z = 0;
		}

		Vector3& operator=(const Vector3& t)
		{
			x = t.x;
			y = t.y;
			z = t.z;
			return *this;
		}

		Vector3(T x, T y, T z) :x(x), y(y), z(z) {}

		bool operator==(const Vector3& t) const {
			return x == t.x && y == t.y && z == t.z;
		}

		bool operator !=(const Vector3 &a) const {
			return x != a.x || y != a.y || z != a.z;
		}

		Vector3 operator -() const { return Vector3(-x, -y, -z); }

		Vector3 operator +(const Vector3 &a) const {
			return Vector3(x + a.x, y + a.y, z + a.z);
		}

		Vector3 operator -(const Vector3 &a) const {
			return Vector3(x - a.x, y - a.y, z - a.z);
		}

		Vector3 operator *(T k) const {
			return Vector3(x*k, y*k, z*k);
		}

		Vector3 operator *(const Vector3 &a) const {
			return Vector3(x * a.x, y * a.y, z * a.z);
		}

		Vector3 operator /(T a) const {
			float	oneOverA = 1.0f / a; // NOTE: no check for divide by zero here
			return Vector3(x*oneOverA, y*oneOverA, z*oneOverA);
		}

		Vector3& operator +=(const Vector3 &a) {
			x += a.x; y += a.y; z += a.z;
			return *this;
		}

		Vector3& operator -=(const Vector3 &a) {
			x -= a.x; y -= a.y; z -= a.z;
			return *this;
		}

		Vector3& operator *=(T a) {
			x *= a; y *= a; z *= a;
			return *this;
		}

		Vector3& operator /=(T a) {
			float	oneOverA = 1.0f / a;
			x *= oneOverA; y *= oneOverA; z *= oneOverA;
			return *this;
		}

		void normalize() {
			T magSq = x*x + y*y + z*z;
			if (magSq > 0.0f) { // check for divide-by-zero
				float oneOverMag = 1.0f / sqrt(magSq);
				x *= oneOverMag;
				y *= oneOverMag;
				z *= oneOverMag;
			}
		}

		float length() const {
			return sqrt(x*x + y*y + z*z);
		}

		T sqrLength() const {
			return x*x + y*y + z*z;
		}

		T dot(const Vector3& a) const {
			return x*a.x + y*a.y + z*a.z;
		}

		Vector3 cross(const Vector3 &b) const {
			return Vector3(
				y*b.z - z*b.y,
				z*b.x - x*b.z,
				x*b.y - y*b.x
				);
		}

		float distance(const Vector3 &b) const {
			T dx = x - b.x;
			T dy = y - b.y;
			T dz = z - b.z;
			return sqrt(dx*dx + dy*dy + dz*dz);
		}

		T distanceSquared(const Vector3& b) const {
			T dx = x - b.x;
			T dy = y - b.y;
			T dz = z - b.z;
			return (dx*dx + dy*dy + dz*dz);
		}
	};

	template<class T>
	inline Vector3<T> cross(const Vector3<T> &a, const Vector3<T> &b) {
		return Vector3<T>(
			a.y*b.z - a.z*b.y,
			a.z*b.x - a.x*b.z,
			a.x*b.y - a.y*b.x
			);
	}

	template<class T>
	inline T dot(const Vector3<T> &a, const Vector3<T> &b)
	{
		return a.dot(b);
	}

	template<>
	Vector3<double> Vector3<double>::operator/(double a) const {
		double oneOverA = 1.0 / a; // NOTE: no check for divide by zero here
		return Vector3<double>(x*oneOverA, y*oneOverA, z*oneOverA);
	}

	template<>
	inline bool Vector3<float>::operator==(const Vector3<float>& t) const {
		return math::FloatEqual(x, t.x) &&
			math::FloatEqual(y, t.y) &&
			math::FloatEqual(z, t.z);
	}

	template<>
	inline bool Vector3<double>::operator==(const Vector3<double>& t) const {
		return math::DoubleEqual(x, t.x) &&
			math::DoubleEqual(y, t.y) &&
			math::DoubleEqual(z, t.z);
	}

	template<>
	inline bool Vector3<float>::operator!=(const Vector3<float>& t) const {
		return !math::FloatEqual(x, t.x) ||
			!math::FloatEqual(y, t.y) ||
			!math::FloatEqual(z, t.z);
	}

	template<>
	inline bool Vector3<double>::operator!=(const Vector3<double>& t) const {
		return !math::DoubleEqual(x, t.x) ||
			!math::DoubleEqual(y, t.y) ||
			!math::DoubleEqual(z, t.z);
	}

	template<>
	void Vector3<double>::normalize() {
		double magSq = x*x + y*y + z*z;
		if (magSq > 0.0) { // check for divide-by-zero
			double oneOverMag = 1.0 / sqrt(magSq);
			x *= oneOverMag;
			y *= oneOverMag;
			z *= oneOverMag;
		}
	}


	using Vector3f = Vector3<float>;
	using Vector3i = Vector3<int>;
	using Vector3d = Vector3<double>;
}




#endif