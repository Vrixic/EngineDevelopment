#pragma once
#include "Vrixic/GenericDefines.h"
#include "Vrixic/Math/VrixicMath.h"
#include "debug_renderer.h"
#include <vector>
#include "External/ImGui/imgui.h"

enum class IntersectionResult
{
	NoIntersection = -1,
	Intersection
};

/* AABB representation */
struct AABB
{
private:
	Vector3D Center;
	Vector3D Extents;

public:
	/* Debug */
	static const uint32 VertexCount = 8;
	static const uint32 IndexCount = 24;

	Vector3D* DebugBoxVertices = nullptr;
public:
	AABB()
	{
		DebugBoxVertices = nullptr;
	}

	AABB(const Vector3D& min, const Vector3D& max)
	{
		DebugBoxVertices = nullptr;
		Create(min, max);
	}

	AABB(const AABB& other)
	{
		if (&other == this)
		{
			return;
		}

		Center = other.Center;
		Extents = other.Extents;

		if (DebugBoxVertices != nullptr)
		{
			delete[] DebugBoxVertices;
		}

		DebugBoxVertices = new Vector3D[8]{ };

		for (uint32 i = 0; i < 8; ++i)
		{
			DebugBoxVertices[i] = other.DebugBoxVertices[i];
		}
	}

	AABB& operator=(const AABB& other)
	{
		if (&other == this)
		{
			return *this;
		}

		Center = other.Center;
		Extents = other.Extents;

		if (DebugBoxVertices != nullptr)
		{
			delete[] DebugBoxVertices;
		}

		DebugBoxVertices = new Vector3D[8]{ };

		for (uint32 i = 0; i < 8; ++i)
		{
			DebugBoxVertices[i] = other.DebugBoxVertices[i];
		}

		return *this;
	}

	~AABB()
	{
		if (DebugBoxVertices == nullptr)
		{
			return;
		}

		delete[] DebugBoxVertices;
	}

	void Create(const Vector3D& min, const Vector3D& max)
	{
		/* Called Created Multiple times */
		if (DebugBoxVertices != nullptr)
		{
			std::cout << "\nAABB Create() called multiple times...\n";
			return;
		}

		Vector3D* Verts = new Vector3D[8]
		{
			min, Vector3D(min.X, min.Y, max.Z), Vector3D(max.X, min.Y, max.Z), Vector3D(max.X, min.Y, min.Z),
			Vector3D(min.X,max.Y,min.Z), Vector3D(min.X,max.Y, max.Z), max, Vector3D(max.X, max.Y, min.Z)
		};

		DebugBoxVertices = Verts;

		Center = (min + max) * 0.5f;
		Extents = max - Center;
	}

	void DrawBox(const end::float4& color, const uint32* boxIndices) const
	{
		for (uint32 i = 0; i < IndexCount; i += 2)
		{
			end::float3 Start = { DebugBoxVertices[boxIndices[i]].X, DebugBoxVertices[boxIndices[i]].Y, DebugBoxVertices[boxIndices[i]].Z };
			end::float3 End = { DebugBoxVertices[boxIndices[i + 1]].X, DebugBoxVertices[boxIndices[i + 1]].Y, DebugBoxVertices[boxIndices[i + 1]].Z };

			end::debug_renderer::add_line(Start, End, color);
		}
	}

	bool RenderImGui()
	{
		bool Render = true;

		ImGui::Begin("Box");

		if (ImGui::Button("X"))
		{
			Render = false;
		}

		//if (ImGui::DragFloat3("Min", &Min.X, 0.01f, -50.0f, 50.0f) || ImGui::DragFloat3("Max", &Max.X, 0.01f, -50.0f, 50.0f))
		//{
		//	RecalculateVerts();
		//}

		ImGui::End();

		return Render;
	}

	static void CreateAABBS(std::vector<AABB*>& aabbs, const Vector3D& axisBounds, uint32 num)
	{
		Vector3D HalfBounds = axisBounds * 0.5f;
		for (uint32 i = 0; i < num; ++i)
		{
			Vector3D Min = Vector3D(Math::RandomRange(-axisBounds.X, axisBounds.X), Math::RandomRange(-axisBounds.Y, axisBounds.Y), Math::RandomRange(-axisBounds.Z, axisBounds.Z));
			Vector3D Max = Vector3D(Min.X + Math::RandomRange(0, HalfBounds.X), Min.Y + Math::RandomRange(0, HalfBounds.Y), Min.Z + Math::RandomRange(0, HalfBounds.Z));

			aabbs.push_back(new AABB(Min, Max));
		}

		aabbs.push_back(new AABB(Vector3D::ZeroVector(), Vector3D(2.0f, 2.0f, 2.0f)));

	}

public:
	inline void SetExtent(const Vector3D& inExtent)
	{
		Extents = inExtent;

		RecalculateVerts();
	}

	inline void SetCenter(const Vector3D& inCenter)
	{
		Center = inCenter;

		RecalculateVerts();
	}

	inline void SetCenterAndExtent(const Vector3D& inCenter, const Vector3D& inExtent)
	{
		Center = inCenter;
		Extents = inExtent;

		RecalculateVerts();
	}

	inline void CalculateCenterAndExtent(const Vector3D& inNewMin, const Vector3D& inNewMax)
	{
		Vector3D CurrentMin = Center - Extents;
		Vector3D CurrentMax = Center + Extents;

		Vector3D Positions[4] = { CurrentMin, inNewMin, CurrentMax, inNewMax };

		float MinX = FLT_MAX;
		float MinY = FLT_MAX;
		float MinZ = FLT_MAX;

		float MaxX = FLT_MIN;
		float MaxY = FLT_MIN;
		float MaxZ = FLT_MIN;

		for (uint32 i = 0; i < 4; ++i)
		{
			Vector3D Position = Positions[i];

			if (Position.X < MinX)
			{
				MinX = Position.X;
			}
			if (Position.Y < MinY)
			{
				MinY = Position.Y;
			}
			if (Position.Z < MinZ)
			{
				MinZ = Position.Z;
			}

			if (Position.X > MaxX)
			{
				MaxX = Position.X;
			}
			if (Position.Y > MaxY)
			{
				MaxY = Position.Y;
			}
			if (Position.Z > MaxZ)
			{
				MaxZ = Position.Z;
			}
		}

		Vector3D Min(MinX, MinY, MinZ);
		Vector3D Max(MaxX, MaxY, MaxZ);

		Center = (Max + Min) * 0.5f;
		Extents = (Max - Center);

		RecalculateVerts();
	}

	inline const Vector3D& GetCenter() const
	{
		return Center;
	}

	inline const Vector3D& GetExtent() const
	{
		return Extents;
	}

	inline Vector3D GetMin() const
	{
		return Center - Extents;
	}

	inline Vector3D GetMax() const
	{
		return Center + Extents;
	}

	inline static IntersectionResult IntersectAABBToAABB(const AABB& a, const AABB& b)
	{
		Vector3D AMin = a.GetMin();
		Vector3D AMax = a.GetMax();

		Vector3D BMin = b.GetMin();
		Vector3D BMax = b.GetMax();

		if (AMax.X < BMin.X || AMin.X > BMax.X) { return IntersectionResult::NoIntersection; }
		if (AMax.Y < BMin.Y || AMin.Y > BMax.Y) { return IntersectionResult::NoIntersection; }
		if (AMax.Z < BMin.Z || AMin.Z > BMax.Z) { return IntersectionResult::NoIntersection; }

		return IntersectionResult::Intersection;
	}

private:
	void RecalculateVerts()
	{
		Vector3D Min = Center - Extents;
		Vector3D Max = Center + Extents;

		DebugBoxVertices[0] = Min;
		DebugBoxVertices[1] = Vector3D(Min.X, Min.Y, Max.Z);
		DebugBoxVertices[2] = Vector3D(Max.X, Min.Y, Max.Z);
		DebugBoxVertices[3] = Vector3D(Max.X, Min.Y, Min.Z);
		DebugBoxVertices[4] = Vector3D(Min.X, Max.Y, Min.Z);
		DebugBoxVertices[5] = Vector3D(Min.X, Max.Y, Max.Z);
		DebugBoxVertices[6] = Max;
		DebugBoxVertices[7] = Vector3D(Max.X, Max.Y, Min.Z);
	}
};
