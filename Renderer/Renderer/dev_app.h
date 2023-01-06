#pragma once
#include <cstdint>
#include <chrono>

#include "debug_renderer.h"
#include "Vrixic/Math/VrixicMath.h"
#include "pools.h"
#include "math_types.h"
#include "Vrixic/Math/Vector2D.h"
#include "Vrixic/Math/Frustum.h"
#include <vector>

#include "renderer.h"

//struct Box
//{
//public:
//	Vector3D Min;
//	Vector3D Max;
//
//public:
//	/* Debug */
//	static const uint32 VertexCount = 8;
//	static const uint32 IndexCount = 24;
//
//	Vector3D* DebugBoxVertices;
//public:
//	Box(Vector3D min, Vector3D max)
//		: Min(min), Max(max)
//	{
//		Vector3D* Verts = new Vector3D[8]
//		{
//			Min, Vector3D(Min.X, Min.Y, Max.Z), Vector3D(Max.X, Min.Y, Max.Z), Vector3D(Max.X, Min.Y, Min.Z),
//			Vector3D(Min.X,Max.Y,Min.Z), Vector3D(Min.X,Max.Y, Max.Z), Max, Vector3D(Max.X, Max.Y, Min.Z)
//		};
//
//		DebugBoxVertices = Verts;
//	}
//
//	Box(const Box& other)
//	{
//		if (&other == this)
//		{
//			return;
//		}
//
//		DebugBoxVertices = other.DebugBoxVertices;
//	}
//
//	~Box()
//	{
//		delete DebugBoxVertices;
//	}
//
//	void DrawBox(end::float4 color, uint32* boxIndices)
//	{
//		for (uint32 i = 0; i < IndexCount; i += 2)
//		{
//			end::float3 Start = { DebugBoxVertices[boxIndices[i]].X, DebugBoxVertices[boxIndices[i]].Y, DebugBoxVertices[boxIndices[i]].Z };
//			end::float3 End = { DebugBoxVertices[boxIndices[i + 1]].X, DebugBoxVertices[boxIndices[i + 1]].Y, DebugBoxVertices[boxIndices[i + 1]].Z };
//
//			end::debug_renderer::add_line(Start, End, color);
//		}
//	}
//
//	static void CreateAABBS(std::vector<Box*>& aabbs, Vector3D axisBounds, uint32 num)
//	{
//		Vector3D HalfBounds = axisBounds * 0.5f;
//		for (uint32 i = 0; i < num; ++i)
//		{
//			Vector3D Min = Vector3D(Math::RandomRange(-axisBounds.X, axisBounds.X), Math::RandomRange(-axisBounds.Y, axisBounds.Y), Math::RandomRange(-axisBounds.Z, axisBounds.Z));
//			Vector3D Max = Vector3D(Min.X + Math::RandomRange(-HalfBounds.X, HalfBounds.X), Min.Y + Math::RandomRange(-HalfBounds.Y, HalfBounds.Y), Min.Z + Math::RandomRange(-HalfBounds.Z, HalfBounds.Z));
//
//			aabbs.push_back(new Box(Min, Max));
//		}
//
//			aabbs.push_back(new Box(Vector3D::ZeroVector(), Vector3D(2.0f, 2.0f, 2.0f)));
//
//	}
//};
//
//namespace ParticleSystem
//{
//	struct Particle
//	{
//	private:
//		float Gravity;
//		float StartLifeTime;
//		float LifeTime;
//
//		Vector3D Position;
//		Vector3D PrevPosition;
//
//		Vector3D Velocity;
//
//		Vector4D StartColor;
//		Vector4D EndColor;
//		Vector4D Color;
//
//	public:
//		Particle() { }
//
//		Particle(Vector3D pos, Vector4D color, Vector4D endColor, Vector3D startVelocity, float lifeTime, float gravity = 9.8f)
//			: Position(pos), PrevPosition(pos), StartColor(color), EndColor(endColor), Velocity(startVelocity),
//			LifeTime(lifeTime), StartLifeTime(lifeTime), Gravity(gravity) { }
//
//		Particle(const Particle& other)
//		{
//			if (&other == this)
//			{
//				return;
//			}
//
//			Position = other.Position;
//			PrevPosition = other.PrevPosition;
//			Color = other.Color;
//			Velocity = other.Velocity;
//			Gravity = other.Gravity;
//			LifeTime = other.LifeTime;
//		}
//
//	public:
//		Particle& operator=(const Particle& other)
//		{
//			if (&other == this)
//			{
//				return *this;
//			}
//
//			Position = other.Position;
//			PrevPosition = other.PrevPosition;
//			Color = other.Color;
//			StartColor = other.StartColor;
//			EndColor = other.EndColor;
//			Velocity = other.Velocity;
//			Gravity = other.Gravity;
//			LifeTime = other.LifeTime;
//			StartLifeTime = other.StartLifeTime;
//
//			return *this;
//		}
//
//	public:
//
//		void AddVelocity(Vector3D velocity)
//		{
//			Velocity += velocity;
//		}
//
//		void Update(float deltaTime)
//		{
//			LifeTime -= deltaTime;
//			PrevPosition = Position;
//
//			/* Kinematic Equation for finding the final velocity vf = vi + at */
//			Vector3D FinalVelocity = Velocity + Gravity * deltaTime;
//
//			/* Displacement of the particle */
//			Vector3D Displacement = Vector3D::ZeroVector();
//
//			/* Apply gravity to the particle */
//			Displacement.Y -= Gravity * deltaTime;
//
//			/* Add the calculated displacement of the particle vi * t + 1/2 * a * t^2 */
//			Displacement += (FinalVelocity * deltaTime + 0.5f * Gravity * (deltaTime * deltaTime));
//
//			Velocity -= FinalVelocity * deltaTime;
//
//			Position += Displacement;
//
//			Color = Vector4D::Lerp(StartColor, EndColor, LifeTime / StartLifeTime);
//		}
//
//		void Render()
//		{
//			end::debug_renderer::add_line(end::float3(PrevPosition.X, PrevPosition.Y, PrevPosition.Z),
//				end::float3(Position.X, Position.Y, Position.Z),
//				end::float4(Color.X, Color.Y, Color.Z, Color.W));
//		}
//
//		bool IsAlive()
//		{
//			return LifeTime > 0.0f;
//		}
//
//	};
//
//	struct ParticleEmitter
//	{
//	public:
//		Vector3D SpawnPosition;
//		Vector4D SpawnColor;
//		Vector4D EndColor;
//
//	private:
//		end::sorted_pool_t<int16, 512> Indices;
//		end::pool_t<ParticleSystem::Particle, 2048>* ParticlePool;
//
//	public:
//		ParticleEmitter() { }
//
//		ParticleEmitter(end::pool_t<ParticleSystem::Particle, 2048>* particlePool, Vector3D spawnPosition, Vector4D color, Vector4D endColor)
//		{
//			ParticlePool = particlePool;
//
//			SpawnPosition = spawnPosition;
//			SpawnColor = color;
//			EndColor = endColor;
//		}
//
//		~ParticleEmitter() { }
//
//		//ParticleEmitter(const ParticleEmitter& other) = delete;
//		//ParticleEmitter& operator=(const ParticleEmitter& other) = delete;
//
//	public:
//		void Update(float deltaTime)
//		{
//			for (uint32 j = 0; j < Indices.size(); ++j)
//			{
//				ParticleSystem::Particle* P = &(*ParticlePool)[Indices[j]];
//				if (P->IsAlive())
//				{
//					P->Update(deltaTime);
//
//					/* @TODO Move render to its correct location */
//					P->Render();
//				}
//				else
//				{
//					ParticlePool->free(Indices[j]);
//					Indices.free(j);
//				}
//			}
//		}
//
//		void SpawnParticle(const Vector3D& pos, const Vector3D& velocity, float lifeTime)
//		{
//			/* Find a spot in the particle pool is its available */
//			uint32 ParticleIndex = ParticlePool->alloc();
//
//			if (ParticleIndex != -1)
//			{
//				/* find a spot inside the sorted indicies */
//				uint32 IndexToParticle = Indices.alloc();
//
//				if (IndexToParticle != -1)
//				{
//					ParticleSystem::Particle P(SpawnPosition + pos, SpawnColor, EndColor, velocity, lifeTime);
//					/* Add this particle to the particle pool */
//					(*ParticlePool)[ParticleIndex] = P;
//					/* Change the index inside the sorted indicies to point to the particle in the particles pool */
//					Indices[IndexToParticle] = ParticleIndex;
//				}
//			}
//			else
//			{
//				ParticlePool->free(ParticleIndex);
//			}
//		}
//
//		/* @TODO implement cone particle spawn*/
//	};
//
//	struct SortedParticleEmitter
//	{
//	public:
//		Vector3D SpawnPosition;
//		Vector4D SpawnColor;
//
//	private:
//		end::sorted_pool_t<Particle, 512> Indices;
//
//	public:
//		SortedParticleEmitter() { }
//
//		SortedParticleEmitter(Vector3D spawnPosition, Vector4D color)
//		{
//			SpawnPosition = spawnPosition;
//			SpawnColor = color;
//		}
//
//		~SortedParticleEmitter() { }
//
//	public:
//		void Update(float deltaTime)
//		{
//			for (uint32 j = 0; j < Indices.size(); ++j)
//			{
//				ParticleSystem::Particle* P = &Indices[j];
//				if (P->IsAlive())
//				{
//					P->Update(deltaTime);
//
//					/* @TODO Move render to its correct location */
//					P->Render();
//				}
//				else
//				{
//					Indices.free(j);
//				}
//			}
//		}
//
//		void SpawnParticle(const Vector3D& pos, const Vector3D& velocity, const Vector4D& endColor, float lifeTime)
//		{
//			/* Find a spot in the particle pool is its available */
//				/* find a spot inside the sorted indicies */
//			uint32 IndexToParticle = Indices.alloc();
//
//			if (IndexToParticle != -1)
//			{
//				ParticleSystem::Particle P(SpawnPosition + pos, endColor, endColor, velocity, lifeTime);
//				/* Change the index inside the sorted indicies to point to the particle in the particles pool */
//				Indices[IndexToParticle] = P;
//			}
//		}
//	};
//}


namespace end
{
	// Simple app class for development and testing purposes
	struct dev_app_t
	{

		void update();

		dev_app_t();

		~dev_app_t();

		double get_delta_time()const;

		void RenderImGui();

		void UpdateCameraPositionByView();
		void SetViewMatrix(float4x4_a mat);
		float4x4_a GetViewMatrix() const;

		void SetRenderer(end::renderer_t* ren);
		void CreateBVH();
	};
}