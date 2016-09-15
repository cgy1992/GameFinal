#ifndef __GF_VECTOR4_CLASS_H__
#define __GF_VECTOR4_CLASS_H__

#include "Vector3.h"

namespace gf
{
	template<typename T>
	class Vector4
	{
	public:
		T x, y, z, w;

		Vector4() :x(0), y(0), z(0), w(0){}

		Vector4& operator=(const Vector4& t)
		{
			x = t.x;
			y = t.y;
			z = t.z;
			w = t.w;
			return *this;
		}

		Vector4(T x, T y, T z, T w) :x(x), y(y), z(z), w(w) {}

		Vector4(T x, const Vector3<T>& t)
		{
			this->x = x;
			this->y = t.x;
			this->z = t.y;
			this->w = t.z;
		}

		Vector4(const Vector3<T>& t, T w)
		{
			this->x = t.x;
			this->y = t.y;
			this->z = t.z;
			this->w = w;
		}

		bool operator==(const Vector4& t) const {
			return x == t.x && y == t.y && z == t.z && w == t.w;
		}

		bool operator !=(const Vector4 &a) const {
			return x != a.x || y != a.y || z != a.z || w != a.w;
		}

		Vector4 operator -() const {
			return Vector4(-x, -y, -z, -w);
		}

		Vector4 operator +(const Vector4 &a) const {
			return Vector4(x + a.x, y + a.y, z + a.z, w + a.w);
		}

		Vector4 operator -(const Vector4 &a) const {
			return Vector4(x - a.x, y - a.y, z - a.z, w - a.w);
		}

		Vector4 operator *(T k) const {
			return Vector4(x*k, y*k, z*k, w*k);
		}

		Vector4 operator *(const Vector4 &a) const {
			return Vector4(x * a.x, y * a.y, z * a.z, w * a.w);
		}

		Vector4 operator /(T a) const {
			float	oneOverA = 1.0f / a; // NOTE: no check for divide by zero here
			return Vector4(x*oneOverA, y*oneOverA, z*oneOverA, w*oneOverA);
		}

		Vector4& operator +=(const Vector4 &a) {
			x += a.x; y += a.y; z += a.z; w += a.w;
			return *this;
		}

		Vector4& operator -=(const Vector4 &a) {
			x -= a.x; y -= a.y; z -= a.z; w -= a.w;
			return *this;
		}

		Vector4& operator *=(T a) {
			x *= a; y *= a; z *= a; w *= a;
			return *this;
		}

		Vector4& operator /=(T a) {
			float	oneOverA = 1.0f / a;
			x *= oneOverA; 
			y *= oneOverA; 
			z *= oneOverA;
			w *= oneOverA;
			return *this;
		}

		void normalize() {
			T magSq = x*x + y*y + z*z + w*w;
			if (magSq > 0.0f) { // check for divide-by-zero
				float oneOverMag = 1.0f / sqrt(magSq);
				x *= oneOverMag;
				y *= oneOverMag;
				z *= oneOverMag;
				w *= oneOverMag;
			}
		}

		float length() const {
			return sqrt(x*x + y*y + z*z + w*w);
		}

		T sqrLength() const {
			return x*x + y*y + z*z + w*w;
		}

		T dot(const Vector4& a) const {
			return x*a.x + y*a.y + z*a.z + w*a.w;
		}

		float distance(const Vector4 &b) const {
			T dx = x - b.x;
			T dy = y - b.y;
			T dz = z - b.z;
			T dw = w - b.w;
			return sqrtf(dx*dx + dy*dy + dz*dz + dw*dw);
		}

		T distanceSquared(const Vector4& b) const {
			T dx = x - b.x;
			T dy = y - b.y;
			T dz = z - b.z;
			T dw = w - b.w;
			return (dx*dx + dy*dy + dz*dz + dw*dw);
		}

	};

	template<class T>
	inline T dot(const Vector4<T> &a, const Vector4<T> &b)
	{
		return a.dot(b);
	}

	template<>
	Vector4<double> Vector4<double>::operator/(double a) const {
		double oneOverA = 1.0 / a; // NOTE: no check for divide by zero here
		return Vector4<double>(x*oneOverA, y*oneOverA, z*oneOverA, w*oneOverA);
	}

	template<>
	inline bool Vector4<float>::operator==(const Vector4<float>& t) const {
		return math::FloatEqual(x, t.x) &&
			math::FloatEqual(y, t.y) &&
			math::FloatEqual(z, t.z) &&
			math::FloatEqual(w, t.w);
	}

	template<>
	inline bool Vector4<double>::operator==(const Vector4<double>& t) const {
		return math::DoubleEqual(x, t.x) &&
			math::DoubleEqual(y, t.y) &&
			math::DoubleEqual(z, t.z) &&
			math::DoubleEqual(w, t.w);
	}

	template<>
	inline bool Vector4<float>::operator!=(const Vector4<float>& t) const {
		return !math::FloatEqual(x, t.x) ||
			!math::FloatEqual(y, t.y) ||
			!math::FloatEqual(z, t.z) ||
			!math::FloatEqual(w, t.w);
	}

	template<>
	inline bool Vector4<double>::operator!=(const Vector4<double>& t) const {
		return !math::DoubleEqual(x, t.x) ||
			!math::DoubleEqual(y, t.y) ||
			!math::DoubleEqual(z, t.z) ||
			!math::DoubleEqual(w, t.w);
	}

	template<>
	void Vector4<double>::normalize() {
		double magSq = x*x + y*y + z*z + w*w;
		if (magSq > 0.0) { // check for divide-by-zero
			double oneOverMag = 1.0 / sqrt(magSq);
			x *= oneOverMag;
			y *= oneOverMag;
			z *= oneOverMag;
			w *= oneOverMag;
		}
	}

	using Vector4f = Vector4<float>;
	using Vector4i = Vector4<int>;
	using Vector4d = Vector4<double>;
}

#endif
