#pragma once
#include "Vrixic/Math/Vector4D.h"
#include "Vrixic/GenericDefines.h"

struct Particle
{
	Vector3D Position;
	Vector3D PrevPosition;
	Vector4D Color;

public:
	Particle(Vector3D pos, Vector4D color)
		: Position(pos), PrevPosition(pos), Color(color) { }

	Particle(const Particle& other)
	{
		if (&other == this)
		{
			return;
		}

		Position = other.Position;
		PrevPosition = other.PrevPosition;
		Color = other.Color;
	}

public:
	void operator=(const Particle& other)
	{
		if (&other == this)
		{
			return;
		}

		Position = other.Position;
		PrevPosition = other.PrevPosition;
		Color = other.Color;
	}

};

//template<class T, uint32 N>
//class SortedPool
//{
//private:
//	T PooledObjects[N];
//
//public:
//	SortedPool() { }
//
//	~Sorte
//};
