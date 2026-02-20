#pragma once
namespace JSharedLib
{
	class Vector3
	{
	public:
		Vector3();
		Vector3(float _x, float _y, float _z);
		~Vector3();

		float GetMagnitude();
		std::string ToString();
		float operator*(const Vector3& b);
		friend bool operator==(const Vector3& a, const Vector3& b)
		{
			return (a.x == b.x) && (a.y == b.y) && (a.z == b.z);
		}
		friend bool operator!=(const Vector3& a, const Vector3& b)
		{
			return !(a == b);
		}
		Vector3& operator=(const Vector3& b);
		Vector3& operator+=(const Vector3& b);
		Vector3& operator-=(const Vector3& b);
		const Vector3 operator+(const Vector3& b);
		const Vector3 operator-(const Vector3& b);
		float x;
		float y;
		float z;
	};

	Vector3 Normalize(const Vector3& vector);
	float CalDist(const Vector3& a, const Vector3& b);
}

