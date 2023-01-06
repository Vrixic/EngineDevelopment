#pragma once

#include "VrixicMathHelper.h"

#include "Vector3D.h"
#include "Vector4D.h"
#include "Matrix4D.h"
#include "Plane.h"

struct VrixicMath
{
public:
	inline static Matrix4D LookAt(const Vector3D& eye, const Vector3D& target, const Vector3D& up)
	{
		Vector3D ZAxis; // normal(target - eye)
		Vector3D XAxis; // normal(cross(up, ZAxis))
		Vector3D YAxis; // cross(ZAxis, XAxis)

		ZAxis = (target - eye).Normalize();
		XAxis = Vector3D::CrossProduct(up, ZAxis).Normalize();
		YAxis = Vector3D::CrossProduct(ZAxis, XAxis);

		// [ XAxis.X			XAxis.Y				XAxis.Z			0]
		// [ YAxis.X			YAxis.Y				YAxis.Z			0]
		// [ ZAxis.X			ZAxis.Y				ZAxis.Z			0]
		// [ eye.X				eye.Y				eye.Z			1]

		Matrix4D Result(
			XAxis.X, XAxis.Y, XAxis.Z, 0.0f,
			YAxis.X, YAxis.Y, YAxis.Z, 0.0f,
			ZAxis.X, ZAxis.Y, ZAxis.Z, 0.0f,
			eye.X, eye.Y, eye.Z, 1.0f
		);

		return Result;
	}

	inline static Matrix4D OrthoNormalizeMatrix(const Matrix4D& mat)
	{
		Vector3D Z = mat[2].ToVector3D().Normalize();
		Vector3D X = Vector3D::CrossProduct(Vector3D(0, 1, 0), Z).Normalize(); /* Use World Up to get X vector */
		Vector3D Y = Vector3D::CrossProduct(Z, X).Normalize();

		return Matrix4D(Vector4D(X, 0.0f), Vector4D(Y, 0.0f), Vector4D(Z, 0.0f), mat[3]);
	}

	inline static Matrix4D TurnTo(float deltaTime, float speed, const Vector3D& target, const Matrix4D& mat)
	{
		/* Vector to the target */
		Vector3D ToTarget = (target - mat[3].ToVector3D()).Normalize();

		/* DotY -> the dot of ToTarget to the right vector */
		float DotY = Vector3D::DotProduct(ToTarget, mat[0].ToVector3D());

		/* DotX -> the dot of ToTarget to the up vector */
		float DotX = Vector3D::DotProduct(ToTarget, mat[1].ToVector3D());

		Matrix4D RotX = Matrix4D::MakeRotX(Math::RadiansToDegrees(-DotX) * deltaTime * speed);
		Matrix4D RotY = Matrix4D::MakeRotY(Math::RadiansToDegrees(DotY) * deltaTime * speed);
		Matrix4D Result = RotY * RotX * mat;

		return OrthoNormalizeMatrix(Result);
	}

	inline static PlaneIntersectionResult IntersectSphereOnPlane(const Vector3D& center, const float radius, const Plane& plane)
	{
		float SphereCenterOffset = Plane::Dot(plane, center);
		float SphereSignedDistance = SphereCenterOffset - plane.Distance;

		if (SphereSignedDistance < -radius)
		{
			return PlaneIntersectionResult::Back;
		}
		else if (SphereSignedDistance > radius)
		{
			return PlaneIntersectionResult::Front;
		}

		return PlaneIntersectionResult::Straddling;
	}

	inline static PlaneIntersectionResult IntersectAABBOnPlane(const Vector3D& inCenter, const Vector3D& inExtents, Plane& inPlane)
	{
		float SphereProjectedRadius = Vector3D::DotProduct(inPlane.AbsNormal(), inExtents);
		return IntersectSphereOnPlane(inCenter, SphereProjectedRadius, inPlane);
	}

	inline static float ManhattanDistance(const Vector3D& v1, const Vector3D& v2)
	{
		float DeltaX = std::abs(v2.X - v1.X);
		float DeltaY = std::abs(v2.Y - v1.Y);
		float DeltaZ = std::abs(v2.Z - v1.Z);

		return DeltaX + DeltaY + DeltaZ;
	}
};
