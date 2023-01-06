#pragma once
#include <string>
#include <vector>
#include "LabBase.h"
#include "Vrixic/Math/VrixicMath.h"
#include "debug_renderer.h"
#include "pools.h"
#include "External/ImGui/imgui.h"

namespace ParticleSystem
{
	struct Particle
	{
	private:
		float Gravity;
		float StartLifeTime;
		float LifeTime;

		Vector3D Position;
		Vector3D PrevPosition;

		Vector3D Velocity;

		Vector4D StartColor;
		Vector4D EndColor;
		Vector4D Color;

	public:
		Particle() { }

		Particle(Vector3D pos, Vector4D startColor, Vector4D endColor, Vector3D startVelocity, float lifeTime, float gravity = 9.8f)
			: Position(pos), PrevPosition(pos), StartColor(startColor), EndColor(endColor), Velocity(startVelocity),
			LifeTime(lifeTime), StartLifeTime(lifeTime), Gravity(gravity) { }

		Particle(const Particle& other)
		{
			if (&other == this)
			{
				return;
			}

			Position = other.Position;
			PrevPosition = other.PrevPosition;
			Color = other.Color;
			Velocity = other.Velocity;
			Gravity = other.Gravity;
			LifeTime = other.LifeTime;
		}

	public:
		Particle& operator=(const Particle& other)
		{
			if (&other == this)
			{
				return *this;
			}

			Position = other.Position;
			PrevPosition = other.PrevPosition;
			Color = other.Color;
			StartColor = other.StartColor;
			EndColor = other.EndColor;
			Velocity = other.Velocity;
			Gravity = other.Gravity;
			LifeTime = other.LifeTime;
			StartLifeTime = other.StartLifeTime;

			return *this;
		}

	public:

		void AddVelocity(Vector3D velocity)
		{
			Velocity += velocity;
		}

		void Update(float deltaTime)
		{
			LifeTime -= deltaTime;
			PrevPosition = Position;

			/* Kinematic Equation for finding the final velocity vf = vi + at */
			Vector3D FinalVelocity = Velocity + Gravity * deltaTime;

			/* Displacement of the particle */
			Vector3D Displacement = Vector3D::ZeroVector();

			/* Apply gravity to the particle */
			Displacement.Y -= Gravity * deltaTime;

			/* Add the calculated displacement of the particle vi * t + 1/2 * a * t^2 */
			Displacement += (FinalVelocity * deltaTime + 0.5f * Gravity * (deltaTime * deltaTime));

			Velocity -= FinalVelocity * deltaTime;

			Position += Displacement;

			Color = Vector4D::Lerp(StartColor, EndColor, 1.0f - (LifeTime / StartLifeTime));
		}

		void Render()
		{
			end::debug_renderer::add_line(end::float3(PrevPosition.X, PrevPosition.Y, PrevPosition.Z),
				end::float3(Position.X, Position.Y, Position.Z),
				end::float4(Color.X, Color.Y, Color.Z, Color.W));
		}

		bool IsAlive()
		{
			return LifeTime > 0.0f;
		}

	};

	struct ParticleEmitter
	{
	public:
		Vector3D SpawnPosition;
		Vector4D SpawnColor;
		Vector4D EndColor;

	private:
		end::sorted_pool_t<int16, 512> Indices;
		end::pool_t<ParticleSystem::Particle, 2048>* ParticlePool;

	public:
		ParticleEmitter() { }

		ParticleEmitter(end::pool_t<ParticleSystem::Particle, 2048>* particlePool, Vector3D spawnPosition, Vector4D color, Vector4D endColor)
		{
			ParticlePool = particlePool;

			SpawnPosition = spawnPosition;
			SpawnColor = color;
			EndColor = endColor;
		}

		~ParticleEmitter() { }

		//ParticleEmitter(const ParticleEmitter& other) = delete;
		//ParticleEmitter& operator=(const ParticleEmitter& other) = delete;

	public:
		void Update(float deltaTime)
		{
			for (uint32 j = 0; j < Indices.size(); ++j)
			{
				ParticleSystem::Particle* P = &(*ParticlePool)[Indices[j]];
				if (P->IsAlive())
				{
					P->Update(deltaTime);

					/* @TODO Move render to its correct location */
					P->Render();
				}
				else
				{
					ParticlePool->free(Indices[j]);
					Indices.free(j);
				}
			}
		}

		bool RenderImGui()
		{
			bool Render = true;

			ImGui::Begin("Emitter");
			
			if (ImGui::Button("X"))
			{
				Render = false;
			}

			ImGui::DragFloat3("Spawn Position", &SpawnPosition.X, 0.1f);
			ImGui::DragFloat4("Start Color", &SpawnColor.X, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat4("End Color", &EndColor.X, 0.01f, 0.0f, 1.0f);

			ImGui::End();

			return Render;
		}

		void SpawnParticle(const Vector3D& pos, const Vector3D& velocity, float lifeTime)
		{
			/* Find a spot in the particle pool is its available */
			uint32 ParticleIndex = ParticlePool->alloc();

			if (ParticleIndex != -1)
			{
				/* find a spot inside the sorted indicies */
				uint32 IndexToParticle = Indices.alloc();

				if (IndexToParticle != -1)
				{
					ParticleSystem::Particle P(SpawnPosition + pos, SpawnColor, EndColor, velocity, lifeTime);
					/* Add this particle to the particle pool */
					(*ParticlePool)[ParticleIndex] = P;
					/* Change the index inside the sorted indicies to point to the particle in the particles pool */
					Indices[IndexToParticle] = ParticleIndex;
				}
			}
			else
			{
				ParticlePool->free(ParticleIndex);
			}
		}

	public:
		uint32 GetActiveCount() const
		{
			return Indices.size();
		}

		/* @TODO implement cone particle spawn*/
	};

	struct SortedParticleEmitter
	{
	public:
		Vector3D SpawnPosition;
		Vector4D SpawnColor;

	private:
		end::sorted_pool_t<Particle, 512> Indices;

	public:
		SortedParticleEmitter() { }

		SortedParticleEmitter(Vector3D spawnPosition, Vector4D color)
		{
			SpawnPosition = spawnPosition;
			SpawnColor = color;
		}

		~SortedParticleEmitter() { }

	public:
		void Update(float deltaTime)
		{
			for (uint32 j = 0; j < Indices.size(); ++j)
			{
				ParticleSystem::Particle* P = &Indices[j];
				if (P->IsAlive())
				{
					P->Update(deltaTime);

					/* @TODO Move render to its correct location */
					P->Render();
				}
				else
				{
					Indices.free(j);
				}
			}
		}

		void SpawnParticle(const Vector3D& pos, const Vector3D& velocity, const Vector4D& endColor, float lifeTime)
		{
			/* Find a spot in the particle pool is its available */
				/* find a spot inside the sorted indicies */
			uint32 IndexToParticle = Indices.alloc();

			if (IndexToParticle != -1)
			{
				ParticleSystem::Particle P(SpawnPosition + pos, endColor, endColor, velocity, lifeTime);
				/* Change the index inside the sorted indicies to point to the particle in the particles pool */
				Indices[IndexToParticle] = P;
			}
		}

	public:
		uint32 GetActiveCount() const
		{
			return Indices.size();
		}
	};
}

class ParticlesLab : public LabBase
{
private:
	float ParticleSpawnTime = 0.05f;

	end::pool_t<ParticleSystem::Particle, 2048> ParticlePool;

	/* Emitter that will use the free pool */
	std::vector<ParticleSystem::ParticleEmitter> Emitters;

	ParticleSystem::SortedParticleEmitter SortedEmitterOnly;
	Vector4D SortedEmitterColor;

	float TimePassed = 0.0f;

	/* ImGii */
	uint32 MaxParticlesCount = 2560;
	uint32 ActiveParticlesCount = 0;
	uint32 ActiveEmittersCount = 5;

	ParticleSystem::ParticleEmitter* EmitterGui = nullptr;

public:
	ParticlesLab()
	{
		Emitters.resize(4);
		Emitters[0] = ParticleSystem::ParticleEmitter(&ParticlePool, Vector3D(0.0f, 0.0f, 10.0f), Vector4D(1.0f, 0, 0, 1), Vector4D(0.0f, 0, 1, 1));
		Emitters[1] = ParticleSystem::ParticleEmitter(&ParticlePool, Vector3D(-10.0f, 0.0f, 0.0f), Vector4D(0.0f, 1, 0, 1), Vector4D(0.0f, 1, 0, 1));
		Emitters[2] = ParticleSystem::ParticleEmitter(&ParticlePool, Vector3D(10.0f, 0.0f, 0.0f), Vector4D(0.0f, 0, 1, 1), Vector4D(1.0f, 0, 0, 1));
		Emitters[3] = ParticleSystem::ParticleEmitter(&ParticlePool, Vector3D(0.0f, 0.0f, -10.0f), Vector4D(1.0f, 1, 0, 1), Vector4D(1.0f, 0, 1, 1));

		SortedEmitterOnly = ParticleSystem::SortedParticleEmitter(Vector3D(0.0f, 15.0f, 0.0f), Vector4D(1, 1, 1, 1));
		SortedEmitterColor = Vector4D(0.5f, 0.0f, 0.0f, 1.0f);
	}

	~ParticlesLab()
	{

	}

public:
	void Update(float deltaTime) override
	{
		TimePassed += deltaTime;

		for (uint32 i = 0; i < Emitters.size(); ++i)
		{
			if (TimePassed > ParticleSpawnTime)
			{
				Emitters[i].SpawnParticle(Vector3D::ZeroVector(), Vector3D(Math::RandomRange(-3, 3), 30.0f, Math::RandomRange(-3, 3)), 4.0f);
			}
		}

		ActiveParticlesCount = 0;
		for (uint32 i = 0; i < Emitters.size(); ++i)
		{
			Emitters[i].Update(deltaTime);
			ActiveParticlesCount += Emitters[i].GetActiveCount();
		}

		SortedEmitterOnly.Update(deltaTime);
		ActiveParticlesCount += SortedEmitterOnly.GetActiveCount();

		if (TimePassed > ParticleSpawnTime)
		{
			SortedEmitterOnly.SpawnParticle(Vector3D::ZeroVector(), Vector3D(Math::RandomRange(-3, 3), 30.0f, Math::RandomRange(-3, 3)), SortedEmitterColor, 5.0f);

			TimePassed = 0.0f;
		}
	}

	void RenderImGui() override
	{
		// Create ImGui Test Window
		ImGui::Begin("Particles Lab");
		ImGui::Text("Max Particles: %u", MaxParticlesCount);
		ImGui::Text("Active Particles: %u", ActiveParticlesCount);
		ImGui::Text("Active Emitters: %u", ActiveEmittersCount);

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::DragFloat("Spawn Time", &ParticleSpawnTime, 0.001f);

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();

		for (uint32 i = 0; i < Emitters.size(); ++i)
		{
			std::string ButtonName = "Emitter ";
			ButtonName.append(std::to_string(i + 1));

			if (ImGui::Button(ButtonName.c_str()))
			{
				EmitterGui = &Emitters[i];
				break;
			}
		}

		if (EmitterGui != nullptr)
		{
			if (!EmitterGui->RenderImGui())
			{
				EmitterGui = nullptr;
			}
		}
		
		ImGui::End();
	}

	void SetSortedEmitterColor(const Vector4D& color)
	{
		SortedEmitterColor = color;
	}
};
