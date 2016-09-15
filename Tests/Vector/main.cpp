#include <iostream>
#include <string>
#include <sstream>
#include <large_bits.h>
#include <Vector3.h>
#include <Vector4.h>
#include <assert.h>
#include <Vector2.h>
//#include <gfMath.h>

using namespace std;
using namespace gf; 

void testVector3()
{
	Vector3<float> v1(1.0f, 2.0f, 3.0f);
	Vector3<float> v2(4.0f, 5.0f, 6.0f);
	Vector3<float> v3 = v1 + v2;
	assert(v3 == Vector3<float>(5, 7, 9));
	v3 = v1 - v2;
	assert(v3 == Vector3<float>(-3, -3, -3));

	Vector3f v4 = v1;
	v4 += Vector3f(1.0f, -1.0f, 2.0f);
	assert(v4 == Vector3f(2.0f, 1.0f, 5.0f));

	v4 -= Vector3f(1.0f, -1.0f, 2.0f);
	assert(v4 == v1);

	Vector3f v5 = v1 * 2.0f;
	assert(v5 == Vector3f(2.0f, 4.0f, 6.0f));

	Vector3f v6 = v5 / 2.0f;
	assert(v6 == v1);

	assert(v6.length() == sqrtf(14.0f));
	v6.normalize();
	assert(math::FloatEqual(v6.length(), 1.0f));

	assert(math::FloatEqual(v1.dot(v2), 32.0f));
	assert(math::FloatEqual(dot(v1, v2), 32.0f));

	assert(math::FloatEqual(v1.distance(v2), sqrtf(27.0f)));
	
	Vector3f v7 = v1.cross(v2);
	assert(math::FloatEqual(dot(v7, v1), 0));
	assert(math::FloatEqual(dot(v7, v2), 0));

	Vector3f v8 = cross(v1, v2);
	assert(v7 == v8);
}

void testVector4()
{
	Vector4d v1(1.0f, 2.0f, 3.0f, -1.0f);
	Vector4d v2(4.0f, 5.0f, 6.0f, -4.0f);
	Vector4d v3 = v1 + v2;
	assert(v3 == Vector4d(5, 7, 9, -5));
	v3 = v1 - v2;
	assert(v3 == Vector4d(-3, -3, -3, 3));

	Vector4d v4 = v1;
	v4 += Vector4d(1.0f, -1.0f, 2.0f, 3.0f);
	assert(v4 == Vector4d(2.0f, 1.0f, 5.0f, 2.0f));

	v4 -= Vector4d(1.0f, -1.0f, 2.0f, 3.0f);
	assert(v4 == v1);

	Vector4d v5 = v1 * 2.0f;
	assert(v5 == Vector4d(2.0f, 4.0f, 6.0f, -2.0f));

	Vector4d v6 = v5 / 2.0f;
	assert(v6 == v1);

	assert(v6.length() == sqrtf(15.0f));
	v6.normalize();
	assert(math::DoubleEqual(v6.length(), 1));

	assert(math::DoubleEqual(v1.dot(v2), 36.0));
	assert(math::DoubleEqual(dot(v1, v2), 36.0));

	assert(math::DoubleEqual(v1.distance(v2), 6.0));
}

int main()
{
	testVector3();
	testVector4();

	system("pause");
	return 0;

}

