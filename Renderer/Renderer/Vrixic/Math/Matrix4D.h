#pragma once

#include "Vector3D.h"
#include "Vector4D.h"
#include "VrixicMathHelper.h"
#include "VrixicMathDirectX.h"

#include <iostream>

struct Matrix4D
{
private:
	/* Align the matrix into one data segment */
	alignas(16) float M[4][4];

public:
	inline Matrix4D();

	inline Matrix4D(const Vector4D& a, const Vector4D& b, const Vector4D& c, const Vector4D& d);

	inline Matrix4D(float n00, float n01, float n02, float n03,
		float n10, float n11, float n12, float n13,
		float n20, float n21, float n22, float n23,
		float n30, float n31, float n32, float n33);

public:
	inline float& operator()(int i, int j)
	{
		return (M[i][j]);
	}

	inline const float& operator()(int i, int j) const
	{
		return (M[i][j]);
	}

	/* Returns row 0 - 2, no index error checks */
	inline Vector4D& operator[](int i)
	{
		return (*reinterpret_cast<Vector4D*>(M[i]));
	}

	inline const Vector4D& operator[](int i) const
	{
		return (*reinterpret_cast<const Vector4D*>(M[i]));
	}

	/* row vector multiply by matrix */
	inline Vector4D operator*(const Vector4D& v) const;

	inline Matrix4D operator*(const Matrix4D& otherM) const;

public:

	inline static Matrix4D Identity();

	inline static Matrix4D Transpose(const Matrix4D& mat);

	inline void SetIdentity();

	inline void SetTranslation(const Vector3D& translation);

	// Translate Matrix 
	inline void TranslateMatrix(const Vector3D& translation);

	// Scales the matrix
	inline void ScaleMatrix(const Vector3D& scale);

	/* Euler Angles Rotation Calculation */

	// Rotation over X - axis, returns matrix 
	inline static Matrix4D MakeRotX(float angle);
	//inline static Matrix4D MakeRotXInRadians(float radians);

	// Rotation over Y - axis, returns matrix
	inline static Matrix4D MakeRotY(float angle);
	//inline static Matrix4D MakeRotYInRadians(float radians);

	// Rotation over Z - axis, return matrix
	inline static Matrix4D MakeRotZ(float angle);

	// Makes a rotation from vector3
	inline static Matrix4D MakeRotation(const Vector3D& rotation);

	inline float Determinant() const;

	/* Inverse that only works for orthogonal matrices */
	inline Matrix4D OrthogonalInverse(Matrix4D const& m) const;

	/* An actual Matrix4D transformation inverse */
	inline Matrix4D Inverse() const;

	/* Returns the prespective projection matrix */
	inline Matrix4D GetProjectionMatrix(unsigned int windowWidth, unsigned int windowHeight, float verticalFOV, float nearPlane, float farPlane) const;

	inline Matrix4D OrthogonalInverseMatrix(Matrix4D const& m) const;

	/* Returns a look at matrix that will at as the view matrix -> used for camera (returns a view matrix)*/
	inline static Matrix4D LookAtLH(const Vector3D& eye, const Vector3D& target, const Vector3D& up);

	inline Vector3D GetEulerAngles() const;

	inline Vector3D GetLocalScale() const;
};

inline Matrix4D::Matrix4D()
	: Matrix4D(0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f) {}

inline Matrix4D::Matrix4D(float n00, float n01, float n02, float n03,
	float n10, float n11, float n12, float n13,
	float n20, float n21, float n22, float n23,
	float n30, float n31, float n32, float n33)
{
	M[0][0] = n00;
	M[0][1] = n01;
	M[0][2] = n02;
	M[0][3] = n03;

	M[1][0] = n10;
	M[1][1] = n11;
	M[1][2] = n12;
	M[1][3] = n13;

	M[2][0] = n20;
	M[2][1] = n21;
	M[2][2] = n22;
	M[2][3] = n23;

	M[3][0] = n30;
	M[3][1] = n31;
	M[3][2] = n32;
	M[3][3] = n33;
}

inline Matrix4D::Matrix4D(const Vector4D& a, const Vector4D& b, const Vector4D& c, const Vector4D& d)
{
	M[0][0] = a.X;
	M[0][1] = a.Y;
	M[0][2] = a.Z;
	M[0][3] = a.W;

	M[1][0] = b.X;
	M[1][1] = b.Y;
	M[1][2] = b.Z;
	M[1][3] = b.W;

	M[2][0] = c.X;
	M[2][1] = c.Y;
	M[2][2] = c.Z;
	M[2][3] = c.W;

	M[3][0] = d.X;
	M[3][1] = d.Y;
	M[3][2] = d.Z;
	M[3][3] = d.W;
}

inline Vector4D Matrix4D::operator*(const Vector4D& v) const
{
	/*return Vector4D
	(
		(v.X * M[0][0] + v.Y * M[1][0] + v.Z * M[2][0] + v.W * M[3][0]),
		(v.X * M[0][1] + v.Y * M[1][1] + v.Z * M[2][1] + v.W * M[3][1]),
		(v.X * M[0][2] + v.Y * M[1][2] + v.Z * M[2][2] + v.W * M[3][2]),
		(v.X * M[0][3] + v.Y * M[1][3] + v.Z * M[2][3] + v.W * M[3][3])
	);*/

	VectorRegister VR = MakeVectorRegister(v);
	VR = TransformVectorByMatrix(VR, this);

	Vector4D Result;
	StoreVectorRegister(&Result, VR);
	return Result;
}

inline Matrix4D Matrix4D::operator*(const Matrix4D& otherM) const
{
	/*return Matrix4D
	(
		(M[0][0] * otherM(0, 0) + M[0][1] * otherM(1, 0) + M[0][2] * otherM(2, 0) + M[0][3] * otherM(3, 0)),
		(M[0][0] * otherM(0, 1) + M[0][1] * otherM(1, 1) + M[0][2] * otherM(2, 1) + M[0][3] * otherM(3, 1)),
		(M[0][0] * otherM(0, 2) + M[0][1] * otherM(1, 2) + M[0][2] * otherM(2, 2) + M[0][3] * otherM(3, 2)),
		(M[0][0] * otherM(0, 3) + M[0][1] * otherM(1, 3) + M[0][2] * otherM(2, 3) + M[0][3] * otherM(3, 3)),

		(M[1][0] * otherM(0, 0) + M[1][1] * otherM(1, 0) + M[1][2] * otherM(2, 0) + M[1][3] * otherM(3, 0)),
		(M[1][0] * otherM(0, 1) + M[1][1] * otherM(1, 1) + M[1][2] * otherM(2, 1) + M[1][3] * otherM(3, 1)),
		(M[1][0] * otherM(0, 2) + M[1][1] * otherM(1, 2) + M[1][2] * otherM(2, 2) + M[1][3] * otherM(3, 2)),
		(M[1][0] * otherM(0, 3) + M[1][1] * otherM(1, 3) + M[1][2] * otherM(2, 3) + M[1][3] * otherM(3, 3)),

		(M[2][0] * otherM(0, 0) + M[2][1] * otherM(1, 0) + M[2][2] * otherM(2, 0) + M[2][3] * otherM(3, 0)),
		(M[2][0] * otherM(0, 1) + M[2][1] * otherM(1, 1) + M[2][2] * otherM(2, 1) + M[2][3] * otherM(3, 1)),
		(M[2][0] * otherM(0, 2) + M[2][1] * otherM(1, 2) + M[2][2] * otherM(2, 2) + M[2][3] * otherM(3, 2)),
		(M[2][0] * otherM(0, 3) + M[2][1] * otherM(1, 3) + M[2][2] * otherM(2, 3) + M[2][3] * otherM(3, 3)),

		(M[3][0] * otherM(0, 0) + M[3][1] * otherM(1, 0) + M[3][2] * otherM(2, 0) + M[3][3] * otherM(3, 0)),
		(M[3][0] * otherM(0, 1) + M[3][1] * otherM(1, 1) + M[3][2] * otherM(2, 1) + M[3][3] * otherM(3, 1)),
		(M[3][0] * otherM(0, 2) + M[3][1] * otherM(1, 2) + M[3][2] * otherM(2, 2) + M[3][3] * otherM(3, 2)),
		(M[3][0] * otherM(0, 3) + M[3][1] * otherM(1, 3) + M[3][2] * otherM(2, 3) + M[3][3] * otherM(3, 3))
	);*/

	Matrix4D Result;
	VectorRegisterMatrixMultiply(&Result, this, &otherM);
	return Result;
}

inline Matrix4D Matrix4D::Identity()
{
	return Matrix4D
	(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

inline Matrix4D Matrix4D::Transpose(const Matrix4D& mat)
{
	return Matrix4D(
		mat(0,0), mat(1, 0), mat(2, 0), mat(3, 0),
		mat(0,1), mat(1, 1), mat(2, 1), mat(3, 1),
		mat(0,2), mat(1, 2), mat(2, 2), mat(3, 2),
		mat(0,3), mat(1, 3), mat(2, 3), mat(3, 3)
	);
}

inline void Matrix4D::SetIdentity()
{
	M[0][0] = 1.0f;
	M[0][1] = 0.0f;
	M[0][2] = 0.0f;
	M[0][3] = 0.0f;

	M[1][0] = 0.0f;
	M[1][1] = 1.0f;
	M[1][2] = 0.0f;
	M[1][3] = 0.0f;

	M[2][0] = 0.0f;
	M[2][1] = 0.0f;
	M[2][2] = 1.0f;
	M[2][3] = 0.0f;

	M[3][0] = 0.0f;
	M[3][1] = 0.0f;
	M[3][2] = 0.0f;
	M[3][3] = 1.0f;
}

inline void Matrix4D::SetTranslation(const Vector3D& translation)
{
	M[3][0] = translation.X;
	M[3][1] = translation.Y;
	M[3][2] = translation.Z;
}

inline void Matrix4D::TranslateMatrix(const Vector3D& translation)
{
	M[3][0] += translation.X;
	M[3][1] += translation.Y;
	M[3][2] += translation.Z;
}

inline void Matrix4D::ScaleMatrix(const Vector3D& scale)
{
	M[0][0] += scale.X;
	M[1][1] += scale.Y;
	M[2][2] += scale.Z;
}

inline Matrix4D Matrix4D::MakeRotX(float degrees)
{
	float C = cos(Math::DegreesToRadians(degrees));
	float S = sin(Math::DegreesToRadians(degrees));

	return Matrix4D
	(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, C, S, 0.0f,
		0.0f, -S, C, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

inline Matrix4D Matrix4D::MakeRotY(float degrees)
{
	float C = cos(Math::DegreesToRadians(degrees));
	float S = sin(Math::DegreesToRadians(degrees));

	return Matrix4D
	(
		C, 0.0f, -S, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		S, 0.0f, C, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

inline Matrix4D Matrix4D::MakeRotZ(float degrees)
{
	float C = cos(Math::DegreesToRadians(degrees));
	float S = sin(Math::DegreesToRadians(degrees));

	return Matrix4D
	(
		C, S, 0.0f, 0.0f,
		-S, C, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

/* X -> pitch, Y -> yaw, Z -> roll*/
inline Matrix4D Matrix4D::MakeRotation(const Vector3D& rotation)
{
	Matrix4D RotX = MakeRotX(rotation.X);
	Matrix4D RotY = MakeRotY(rotation.Y);
	Matrix4D RotZ = MakeRotZ(rotation.Z);
	Matrix4D Result;

	VectorRegisterMatrixMultiply(&Result, &RotX, &RotY);
	VectorRegisterMatrixMultiply(&Result, &Result, &RotZ);

	return Result;
}

inline float Matrix4D::Determinant() const
{
	/* Non-Minor matrix calculations */
	float X0 = M[0][0] * M[1][1] - M[0][1] * M[1][0];
	float X1 = M[0][0] * M[1][2] - M[0][2] * M[1][0];
	float X2 = M[0][0] * M[1][3] - M[0][3] * M[1][0];

	float X3 = M[0][1] * M[1][2] - M[0][2] * M[1][1];
	float X4 = M[0][1] * M[1][3] - M[0][3] * M[1][1];

	float X5 = M[0][2] * M[1][3] - M[0][3] * M[1][2];

	/* Minor matrix calculations */
	float M0 = M[2][2] * M[3][3] - M[2][3] * M[3][2];

	float M1 = M[2][1] * M[3][3] - M[2][3] * M[3][1];
	float M2 = M[2][1] * M[3][2] - M[2][2] * M[3][1];

	float M3 = M[2][0] * M[3][3] - M[2][3] * M[3][0];
	float M4 = M[2][0] * M[3][2] - M[2][2] * M[3][0];
	float M5 = M[2][0] * M[3][1] - M[2][1] * M[3][0];

	// Determinant muls
	return ((X0 * M0) - (X1 * M1) + (X2 * M2) + (X3 * M3) - (X4 * M4) + (X5 * M5));
}

inline Matrix4D Matrix4D::OrthogonalInverse(Matrix4D const& m) const
{
	// Transpose matrix 3x3
	Matrix4D Result
	(
		m(0, 0), m(1, 0), m(2, 0), 0.0f,
		m(0, 1), m(1, 1), m(2, 1), 0.0f,
		m(0, 2), m(1, 2), m(2, 2), 0.0f,
		0.0f, 0.0f, 0.0f, m(3, 3)
	);

	// Take position vector from incoming matrix
	Vector4D PositionVector(m(3, 0), m(3, 1), m(3, 2), 1.0f);

	// Multiply transposed 3x3 matrix by the position vector then negate the result
	PositionVector = -(Result * PositionVector);

	// Update the transposed matrix's position vector to the negate result position vector
	Result.SetTranslation(Vector3D(PositionVector.X, PositionVector.Y, PositionVector.Z));

	return Result;
}

inline Matrix4D Matrix4D::Inverse() const
{
	/* Non-Minor matrix calculations */
	float X0 = M[0][0] * M[1][1] - M[0][1] * M[1][0];
	float X1 = M[0][0] * M[1][2] - M[0][2] * M[1][0];
	float X2 = M[0][0] * M[1][3] - M[0][3] * M[1][0];

	float X3 = M[0][1] * M[1][2] - M[0][2] * M[1][1];
	float X4 = M[0][1] * M[1][3] - M[0][3] * M[1][1];

	float X5 = M[0][2] * M[1][3] - M[0][3] * M[1][2];

	/* Minor matrix calculations */
	float M0 = M[2][2] * M[3][3] - M[2][3] * M[3][2];

	float M1 = M[2][1] * M[3][3] - M[2][3] * M[3][1];
	float M2 = M[2][1] * M[3][2] - M[2][2] * M[3][1];

	float M3 = M[2][0] * M[3][3] - M[2][3] * M[3][0];
	float M4 = M[2][0] * M[3][2] - M[2][2] * M[3][0];
	float M5 = M[2][0] * M[3][1] - M[2][1] * M[3][0];

	float Det = ((X0 * M0) - (X1 * M1) + (X2 * M2) + (X3 * M3) - (X4 * M4) + (X5 * M5));

	if (Det <= 0.0f)
	{
		std::cout << Det << "  \texit ";
		return *this;
	}


	float RDet = (1.0f / Det);

	Matrix4D Result;
	Result(0, 0) = RDet * (M[1][1] * M0 - M[1][2] * M1 + M[1][3] * M2);
	Result(0, 1) = RDet * (-M[0][1] * M0 + M[0][2] * M1 - M[0][3] * M2);
	Result(0, 2) = RDet * (M[3][3] * X3 + M[3][1] * X5 - M[3][2] * X4);
	Result(0, 3) = RDet * (M[2][3] * X3 - M[2][1] * X5 + M[2][2] * X4);
				   		  
	Result(1, 0) = RDet * (-M[1][0] * M0 + M[1][2] * M3 - M[1][3] * M4);
	Result(1, 1) = RDet * (M[0][0] * M0 - M[0][2] * M3 + M[0][3] * M4);
	Result(1, 2) = RDet * (-M[3][3] * X1 - M[3][0] * X5 + M[3][2] * X2);
	Result(1, 3) = RDet * (M[2][3] * X1 + M[2][0] * X5 - M[2][2] * X2);
				   		  
	Result(2, 0) = RDet * (M[1][0] * M1 - M[1][1] * M3 + M[1][3] * M5);
	Result(2, 1) = RDet * (-M[0][0] * M1 + M[0][1] * M3 - M[0][3] * M5);
	Result(2, 2) = RDet * (M[3][3] * X0 + M[3][0] * X4 - M[3][1] * X2);
	Result(2, 3) = RDet * (-M[2][3] * X0 - M[2][0] * X4 + M[2][1] * X2);
				   		  
	Result(3, 0) = RDet * (-M[1][0] * M2 + M[1][1] * M4 - M[1][2] * M5);
	Result(3, 1) = RDet * (M[0][0] * M2 - M[0][1] * M4 + M[0][2] * M5);
	Result(3, 2) = RDet * (-M[3][2] * X0 - M[3][0] * X3 + M[3][1] * X1);
	Result(3, 3) = RDet * (M[2][2] * X0 + M[2][0] * X3 - M[2][1] * X1);

	return Result;
}

inline Matrix4D Matrix4D::LookAtLH(const Vector3D& eye, const Vector3D& target, const Vector3D& up)
{
	Vector3D ZAxis; // normal(target - eye)
	Vector3D XAxis; // normal(cross(up, ZAxis))
	Vector3D YAxis; // cross(ZAxis, XAxis)

	ZAxis = (target - eye).Normalize();
	XAxis = Vector3D::CrossProduct(up, ZAxis).Normalize();
	YAxis = Vector3D::CrossProduct(ZAxis, XAxis);

	// [ XAxis.X			YAxis.X				ZAxis.X			0]
	// [ XAxis.Y			YAxis.Y				ZAxis.Y			0]
	// [ XAxis.Z			YAxis.Z				ZAxis.Z			0]
	// [-dot(XAxis, eye)   -dot(YAxis, eye)	   -dot(ZAxis, eye)	1]
	
	Matrix4D Result(
		XAxis.X, YAxis.X, ZAxis.X, 0.0f,
		XAxis.Y, YAxis.Y, ZAxis.Y, 0.0f,
		XAxis.Z, YAxis.Z, ZAxis.Z, 0.0f,
		-Vector3D::DotProduct(XAxis, eye), 
		-Vector3D::DotProduct(YAxis, eye),
		-Vector3D::DotProduct(ZAxis, eye),
		1.0f
	);

	return Result;
}

inline Matrix4D Matrix4D::GetProjectionMatrix(unsigned int windowWidth, unsigned int windowHeight, float verticalFOV, float nearPlane, float farPlane) const
{
	Matrix4D ProjectionMatrix;
	float AspectRatio = static_cast<float>(windowHeight) / static_cast<float>(windowWidth);
	float YScale = static_cast<float>((cos(Math::DegreesToRadians(verticalFOV * 0.5f)) / sin(Math::DegreesToRadians(verticalFOV * 0.5f))));
	float XScale = YScale * AspectRatio;

	ProjectionMatrix.SetIdentity();

	ProjectionMatrix(0, 0) = XScale;
	ProjectionMatrix(1, 1) = YScale;
	ProjectionMatrix(2, 2) = (farPlane / (farPlane - nearPlane));
	ProjectionMatrix(2, 3) = 1.0f;
	ProjectionMatrix(3, 2) = -(farPlane * nearPlane) / (farPlane - nearPlane);
	ProjectionMatrix(3, 3) = 0.0f;

	return ProjectionMatrix;
}

inline Matrix4D Matrix4D::OrthogonalInverseMatrix(Matrix4D const& m) const
{
	// Transpose matrix 3x3
	Matrix4D result
	(
		m(0, 0), m(1, 0), m(2, 0), 0.0f,
		m(0, 1), m(1, 1), m(2, 1), 0.0f,
		m(0, 2), m(1, 2), m(2, 2), 0.0f,
		0.0f, 0.0f, 0.0f, m(3, 3)
	);

	// Take position vector from incoming matrix
	Vector4D positionVector(m(3, 0), m(3, 1), m(3, 2), 1.0f);

	// Multiply transposed 3x3 matrix by the position vector then negate the result
	positionVector = -(result * positionVector);

	// Update the transposed matrix's position vector to the negate result position vector
	result.SetTranslation(Vector3D(positionVector.X, positionVector.Y, positionVector.Z));

	return result;
}

inline Vector3D Matrix4D::GetEulerAngles() const
{
	Vector3D Result;

	Result.X = atan2(M[2][1], M[2][2]);

	float r32 = M[2][1] * M[2][1];
	float r33 = M[2][2] * M[2][2];

	Result.Y = atan2(-M[2][0], sqrt(r32 + r33));
	Result.Z = atan2(M[1][0], M[0][0]);

	return Result;
}

inline Vector3D Matrix4D::GetLocalScale() const
{
	return Vector3D(M[0][0], M[1][1], M[2][2]);
}