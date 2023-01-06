#pragma once

struct Vector2D
{
	float X;

	float Y;

public:
	inline Vector2D() : X(0.0f), Y(0.0f) { }

	inline Vector2D(float inX, float inY);

	inline Vector2D(float inValue);

	inline float LengthSquared() const;
};

inline Vector2D::Vector2D(float inX, float inY) : X(inX), Y(inY) { }

inline Vector2D::Vector2D(float inValue) : X(inValue), Y(inValue) { }

inline float Vector2D::LengthSquared() const
{
	return X * X + Y * Y;
}