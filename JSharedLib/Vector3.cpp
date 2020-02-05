#include "stdafx.h"
#include "Vector3.h"
using namespace std;
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

	float Vector3::operator*(const Vector3& b)
	{
		return x * b.x + y * b.y + z * b.z;
	}
	bool Vector3::operator==(const Vector3& b)
	{
		return (x == b.x) && (y == b.y) && (z == b.z);
	}

	Vector3& Vector3::operator=(const Vector3& b)
	{
		x = b.x;
		y = b.y;
		z = b.z;
		return *this;
	}

	Vector3& Vector3::operator+=(const Vector3& b)
	{
		x += b.x;
		y += b.y;
		z += b.z;
		return *this;
	}

	Vector3& Vector3::operator-=(const Vector3& b)
	{
		x -= b.x;
		y -= b.y;
		z -= b.z;
		return *this;
	}

	const Vector3 Vector3::operator+(const Vector3& b)
	{
		return Vector3(*this)+=b;
	}


	const Vector3 Vector3::operator-(const Vector3& b)
	{
		return Vector3(*this) -= b;
	}

	float Vector3::GetMagnitude()
	{
		return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
	}
	std::string Vector3::ToString()
	{
		std::ostringstream oss;
		oss << "x : " << x << ", y : " << y << ", z : " << z << std::endl;
		return oss.str();
	}

	Vector3 Normalize(const Vector3& vector)
	{
		auto dist = sqrt(pow(vector.x, 2) + pow(vector.y, 2) + pow(vector.z, 2));
		return Vector3(vector.x/dist, vector.y / dist, vector.z/ dist);
	}
	float CalDist(const Vector3& a, const Vector3& b)
	{
		Vector3 c(a);
		c -= b;
		return c.GetMagnitude();
	}
}