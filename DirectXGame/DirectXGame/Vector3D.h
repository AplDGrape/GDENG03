#pragma once
#include <cmath>

class Vector3D
{
public:
	Vector3D():m_x(0),m_y(0),m_z(0)
	{

	}
	Vector3D(float x, float y, float z) :m_x(x), m_y(y), m_z(z)
	{

	}
	Vector3D(const Vector3D& vector) :m_x(vector.m_x), m_y(vector.m_y), m_z(vector.m_z)
	{

	}

	//
	static Vector3D zeros()
	{
		return Vector3D(0.0f, 0.0f, 0.0f);
	}
	//
	static Vector3D ones()
	{
		return Vector3D(1.0f, 1.0f, 1.0f);
	}

	static Vector3D lerp(const Vector3D& start, const Vector3D& end, float delta)
	{
		Vector3D v;
		v.m_x = start.m_x * (1.0f - delta) + end.m_x * (delta);
		v.m_y = start.m_y * (1.0f - delta) + end.m_y * (delta);
		v.m_z = start.m_z * (1.0f - delta) + end.m_z * (delta);
		return v;
	}

	// Normalize this vector (returns a new vector)
	Vector3D normalize() const
	{
		float length = sqrt(m_x * m_x + m_y * m_y + m_z * m_z);
		if (length == 0) return Vector3D(0, 0, 0); // avoid division by zero
		return Vector3D(m_x / length, m_y / length, m_z / length);
	}

	// Dot product with another vector
	float dot(const Vector3D& other) const
	{
		return m_x * other.m_x + m_y * other.m_y + m_z * other.m_z;
	}

	// Cross product with another vector
	Vector3D cross(const Vector3D& other) const
	{
		return Vector3D(
			m_y * other.m_z - m_z * other.m_y,
			m_z * other.m_x - m_x * other.m_z,
			m_x * other.m_y - m_y * other.m_x
		);
	}

	float length() const
	{
		return sqrt(m_x * m_x + m_y * m_y + m_z * m_z);
	}

	Vector3D operator*(float num)
	{
		return Vector3D(m_x * num, m_y * num, m_z * num);
	}


	Vector3D operator+(Vector3D vector)
	{
		return Vector3D(m_x + vector.m_x, m_y + vector.m_y, m_z + vector.m_z);
	}

	Vector3D operator-(const Vector3D& other) const
	{
		return Vector3D(m_x - other.m_x, m_y - other.m_y, m_z - other.m_z);
	}

	~Vector3D()
	{

	}
public:
	float m_x, m_y, m_z;
};