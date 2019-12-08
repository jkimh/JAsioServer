#include "Vector3.h"


namespace JSharedLib
{


	Vector3::Vector3()
		: x(0.0f), y(0.0f), z(0.0f)
	{
	}

	Vector3::Vector3(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3::~Vector3()
	{
	}


	Vector3 Vector3::operator+(const Vector3& b)
	{
		return Vector3(x + b.x, y + b.y, z + b.z);
	}


	Vector3 Vector3::operator-(const Vector3& b)
	{
		return Vector3(x - b.x, y - b.y, z - b.z);
	}

	float Vector3::operator*(const Vector3& b)
	{
		return x * b.x + y * b.y + z * b.z;
	}
	bool Vector3::operator==(const Vector3& b)
	{
		return (x == b.x) && (y == b.y) && (z == b.z);
	}

	void Vector3::operator=(const Vector3& b)
	{
		x = b.x;
		y = b.y;
		z = b.z;
	}
}