#pragma once
namespace JSharedLib
{
	class Vector3
	{
	public:
		Vector3();
		Vector3(float _x, float _y, float _z);
		~Vector3();

		Vector3 operator+(const Vector3& b);
		Vector3 operator-(const Vector3& b);
		float operator*(const Vector3& b);
		bool operator==(const Vector3& b);
		void operator=(const Vector3& b);
		float x;
		float y;
		float z;
	};

}

