#pragma once
#include "LabBase.h"
#include "FileHelper.h"
#include "../../bvh.h"

class BVHLab : public LabBase
{
	uint32 VertexCount;
	Model* TerrainModel;

	std::vector<AABB> TerrainQuadAABBS;
	//std::vector<uint32> in;

	/* Set the box indices once */
	uint32 BoxIndices[24] =
	{
		0, 1, 1, 2, 2, 3, 3, 0,
		4, 5, 5, 6, 6, 7, 7, 4,
		0, 4, 1, 5, 2, 6, 3, 7
	};

	end::bounding_volume_hierarchy_t BVH;

	void(BVHLab::* OnAABBCollisionCallback)(const AABB& collider);

	AABB TranslateBox;
	Vector3D TranslateBoxCenter;

	const float CharcterSpeed = 5.0f;

	Vector4D CollisionColor;

public:
	BVHLab() { }
	BVHLab(end::renderer_t& inRenderer, const char* inFilePath)
	{
		std::vector<TerrainVertex> TerrainVertices;
		std::vector<uint32> TerrainIndices;

		FileHelper::ReadTerrainFile(inFilePath, VertexCount, TerrainVertices);

		TerrainIndices.resize(VertexCount);
		for (uint32 i = 0; i < VertexCount; ++i)
		{
			TerrainIndices[i] = i;
		}

		TerrainModel = new Model(inRenderer.GetDeviceHandle(), VertexCount, VertexCount, "shaders/vs_terrain.hlsl", "shaders/ps_terrain.hlsl",
			TerrainVertices, TerrainIndices);

		inRenderer.AddModel(TerrainModel);

		/* Each triangle has its own aabb */
		uint32 AABBCount = VertexCount / 3;

		//TerrainQuadAABBS.resize(AABBCount);
		//for (uint32 i = 0; i < AABBCount; i++)
		//{
		//	CalcAABB(i, TerrainQuadAABBS[i]);
		//	//BVH.insert(TerrainQuadAABBS[i], i);
		//}

		//TerrainIndices.resize(24);

		BVH.TopDownTreeCreation(TerrainVertices, TerrainIndices);

		TranslateBoxCenter = Vector3D(30, 12.5f, 0);
		TranslateBox.Create(Vector3D::ZeroVector(), Vector3D::ZeroVector());
		TranslateBox.SetCenterAndExtent(TranslateBoxCenter, Vector3D(1, 5, 1));

		CollisionColor = Vector4D(1, 0, 1, 1);
	}

	virtual ~BVHLab() override
	{
		delete TerrainModel;
	}

public:
	virtual void Update(float deltaTime) override
	{
		TerrainModel->CanRender = true;

		//for (uint32 i = 0; i < BVH.node_count(); i++)
		//{
		//	BVH.node_at(0)->aabb()->DrawBox(end::float4(1, 0, 0, 1), BoxIndices);
		//}

		/*for (uint32 i = 0; i < in.size(); i += 3)
		{
			end::debug_renderer::add_line(Vector3DToFloat3(TerrainModel->Vertices[in[i]].Position),
				Vector3DToFloat3(TerrainModel->Vertices[in[i + 1]].Position), end::float4(1, 0,0,1));

			end::debug_renderer::add_line(Vector3DToFloat3(TerrainModel->Vertices[in[i + 1]].Position),
				Vector3DToFloat3(TerrainModel->Vertices[in[i + 2]].Position), end::float4(1, 0, 0, 1));

			end::debug_renderer::add_line(Vector3DToFloat3(TerrainModel->Vertices[in[i + 2]].Position),
				Vector3DToFloat3(TerrainModel->Vertices[in[i]].Position), end::float4(1, 0, 0, 1));
		}

		end::debug_renderer::add_line(end::float3(0,0,0),
			end::float3(0, 100.0f, 0.0f), end::float4(1, 0, 1, 1));

		end::debug_renderer::add_line(end::float3(0, 0, 0),
			end::float3(0.0f, 100.0f, 1.0f), end::float4(1, 1, 1, 1));*/

		BVH.CheckCollision(0, TranslateBox, *this, &BVHLab::OnAABBCollision);

		float CharacterSpeedZ = (InputManager::GetInstance().GetKeyState(KEY_ARROW_UP) - InputManager::GetInstance().GetKeyState(KEY_ARROW_DOWN));
		float CharacterSpeedX = (InputManager::GetInstance().GetKeyState(KEY_ARROW_RIGHT) - InputManager::GetInstance().GetKeyState(KEY_ARROW_LEFT));

		TranslateBoxCenter.Z += CharcterSpeed * deltaTime * CharacterSpeedZ;
		TranslateBoxCenter.X += CharcterSpeed * deltaTime * CharacterSpeedX;

		//if (CharacterSpeedX + CharacterSpeedZ != 0.0f)
		//{
		TranslateBox.SetCenter(TranslateBoxCenter);
		//}
		TranslateBox.DrawBox(end::float4(1, 1, 0, 1), BoxIndices);
	}

	void OnAABBCollision(const AABB& collider)
	{
		uint32 BoxIndices[24] =
		{
			0, 1, 1, 2, 2, 3, 3, 0,
			4, 5, 5, 6, 6, 7, 7, 4,
			0, 4, 1, 5, 2, 6, 3, 7
		};

		collider.DrawBox((end::float4&)(CollisionColor.X), BoxIndices);
	}

	virtual void RenderImGui() override
	{
		// Create ImGui Test Window
		ImGui::Begin("BVH");

		ImGui::DragFloat4("Collision Color", &CollisionColor.X, 0.01f, 0.01, 1.0f);

		if (ImGui::DragFloat3("Box Center", &TranslateBoxCenter.X, 0.01f, -100.0f, 100.0f))
		{
			TranslateBox.SetCenter(TranslateBoxCenter);
		}

		ImGui::End();

		TerrainModel->CanRender = false;
	}

private:
	void CalcAABB(const uint32 inTriangleIndex, AABB& outAABB)
	{
		float MinX = FLT_MAX;
		float MinY = FLT_MAX;
		float MinZ = FLT_MAX;

		float MaxX = FLT_MIN;
		float MaxY = FLT_MIN;
		float MaxZ = FLT_MIN;

		uint32 StartIndex = inTriangleIndex * 3;
		for (uint32 i = 0; i < 3; ++i)
		{
			Vector3D Position = TerrainModel->Vertices[StartIndex + i].Position;

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

		outAABB.Create(Vector3D(MinX, MinY, MinZ), Vector3D(MaxX, MaxY, MaxZ));
	}

	end::float3 Vector3DToFloat3(const Vector3D& v)
	{
		return end::float3(v.X, v.Y, v.Z);
	}
};
