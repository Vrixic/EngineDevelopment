#pragma once
#include <iostream>
#include "Vector3D.h"

/*
* XYZ for the normal of the plane
* Distance is the distance of the plane -> often called distance from the origin
*/

struct Plane
{
	float X;
	float Y;
	float Z;
	float Distance;

	inline Plane() : X(0.0f), Y(0.0f), Z(0.0f), Distance(0.0f) { }

	inline Plane(float x, float y, float z, float distance)
		: X(x), Y(y), Z(z), Distance(distance) { }

	inline Plane(Vector3D& normal, float distance)
		: X(normal.X), Y(normal.Y), Z(normal.Z), Distance(distance) { }

	inline const Vector3D GetNormal() const
	{
		return Vector3D(X, Y, Z);
	}

	inline Vector3D AbsNormal()
	{
		return Vector3D(std::abs(X), std::abs(Y), std::abs(Z));
	}

	/*
	* The dot product between a point and a plane is equal to the signed
	*	perpendicular distance between them
	*
	* When the normal is unit length, The dot product between a point and a plane
	*	is equal to the length of the projection of p onto n
	*
	* The sign of the distance given by The dot product between a point and a plane
	*	depends on the which side of the plane the point lies.
	*/
	inline static float Dot(const Plane& p, const Vector3D& v)
	{
		return p.X * v.X + p.Y * v.Y + p.Z * v.Z;
	}

	inline void Normalize()
	{
		float Magnitude = (X * X + Y * Y + Z * Z);
		float R = 1 / (Magnitude + EPSILON);

		X *= R;
		Y *= R;
		Z *= R;
		Distance *= R;
	}
};
