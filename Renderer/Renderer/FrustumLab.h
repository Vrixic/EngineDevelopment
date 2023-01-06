#pragma once
#include <string>
#include <vector>
#include "LabBase.h"
#include "Box.h"
#include "debug_renderer.h"
#include "Vrixic/Math/Frustum.h"
#include "InputManager.h"
#include "External/ImGui/imgui.h"

class FrustumLab : public LabBase
{
private:
	Matrix4D FrustumMatrix;
	Vector3D FrustumPosition;

	const float CharcterSpeed = 5.0f;

	Frustum CameraFrustum;

	bool AllowFrustumControl = false;

	/* Set the box indices once */
	uint32 BoxIndices[24] =
	{
		0, 1, 1, 2, 2, 3, 3, 0,
		4, 5, 5, 6, 6, 7, 7, 4,
		0, 4, 1, 5, 2, 6, 3, 7
	};

	std::vector<AABB*> AABBS;

	AABB* BoxImGui = nullptr;

public:
	FrustumLab()
	{
		FrustumMatrix.SetIdentity();
		CameraFrustum = Frustum(1280.0f / 720.0f, 5.0f, 1.0f, 50.0f);
		AABB::CreateAABBS(AABBS, Vector3D(25, 12.5, 25), 10);

		//Vector3D Min(1, 0, 0);
		//Vector3D Max(2, 1, 1);
		//AABB* AABB1 = new AABB(Min, Max);
		//AABB* AABB2 = new AABB(Min + Max * 2, Max + Max * 4);
		//AABB* AABB3 = new AABB(Min + Vector3D(2, 0, 0), Max + Vector3D(2, 0, 0));
		//AABB* AABB4 = new AABB(Min, Max);
		//AABB4->CalculateCenterAndExtent(Min + Vector3D(2, 0, 0), Max + Vector3D(2, 0, 0));
		//
		//AABBS.push_back(AABB1);
		//AABBS.push_back(AABB2);
		//AABBS.push_back(AABB3);
		//AABBS.push_back(AABB4);
	}

	virtual ~FrustumLab() override 
	{
		for (uint32 i = 0; i < AABBS.size(); ++i)
		{
			delete AABBS[i];
		}
	}

public:
	end::float3 Vector3DToFloat3(const Vector3D& v)
	{
		return end::float3(v.X, v.Y, v.Z);
	}

	void Update(float deltaTime) override
	{
		using namespace end;

		float CharacterSpeedZ = (InputManager::GetInstance().GetKeyState(KEY_ARROW_UP) - InputManager::GetInstance().GetKeyState(KEY_ARROW_DOWN));
		float CharacterSpeedX = (InputManager::GetInstance().GetKeyState(KEY_ARROW_RIGHT) - InputManager::GetInstance().GetKeyState(KEY_ARROW_LEFT));

		FrustumPosition.Z += CharcterSpeed * deltaTime * CharacterSpeedZ;
		FrustumPosition.X += CharcterSpeed * deltaTime * CharacterSpeedX;

		/* DEBUG */

		{
			float CanControlFrustum = InputManager::GetInstance().GetMouseState(MOUSE_LEFT_DOWN);

			if (AllowFrustumControl && CanControlFrustum)
			{
				float CameraSpeedX = (InputManager::GetInstance().GetKeyState(KEY_D) - InputManager::GetInstance().GetKeyState(KEY_A));
				float CameraSpeedZ = (InputManager::GetInstance().GetKeyState(KEY_W) - InputManager::GetInstance().GetKeyState(KEY_S));
				float CameraDeltaX = 5.0f * deltaTime * CameraSpeedX;
				float CameraDeltaZ = 5.0f * deltaTime * CameraSpeedZ;

				MousePoint MouseDelta = InputManager::GetInstance().GetMouseDelta();

				float TotalPitch = (Math::DegreesToRadians(65.0f) * (MouseDelta.Y / 720.0f)) * -50.0f;
				float TotalYaw = (Math::DegreesToRadians(65.0f) * (MouseDelta.X / 1280.0f)) * -50.0f;

				Vector3D CameraTranslation(CameraDeltaX, 0.0f, CameraDeltaZ);
				Matrix4D Translation = Matrix4D::Identity();
				Translation.SetTranslation(CameraTranslation);
				FrustumMatrix = Translation * FrustumMatrix;

				Matrix4D PitchMatrix = Matrix4D::MakeRotX(TotalPitch);
				FrustumMatrix = PitchMatrix * FrustumMatrix;

				Matrix4D YawMatrix = Matrix4D::Identity();
				Vector4D CamPos = FrustumMatrix[3];
				YawMatrix = Matrix4D::MakeRotY(TotalYaw);
				FrustumMatrix = FrustumMatrix * YawMatrix;
				FrustumMatrix.SetTranslation(CamPos.ToVector3D());
			}
		}

		/* DEBUG */

		// Update CharacterMatrix with input 
		//FrustumMatrix.SetTranslation(FrustumPosition);
		CameraFrustum.CreateFrustum(FrustumMatrix);

		/* Draw the Frustum */
		debug_renderer::add_line(Vector3DToFloat3(CameraFrustum.NearPlaneTopLeft), Vector3DToFloat3(CameraFrustum.NearPlaneTopRight), float4(1, 1, 1, 1));
		debug_renderer::add_line(Vector3DToFloat3(CameraFrustum.NearPlaneTopRight), Vector3DToFloat3(CameraFrustum.NearPlaneBottomRight), float4(1, 1, 1, 1));
		debug_renderer::add_line(Vector3DToFloat3(CameraFrustum.NearPlaneBottomRight), Vector3DToFloat3(CameraFrustum.NearPlaneBottomLeft), float4(1, 1, 1, 1));
		debug_renderer::add_line(Vector3DToFloat3(CameraFrustum.NearPlaneBottomLeft), Vector3DToFloat3(CameraFrustum.NearPlaneTopLeft), float4(1, 1, 1, 1));

		debug_renderer::add_line(Vector3DToFloat3(CameraFrustum.FarPlaneTopLeft), Vector3DToFloat3(CameraFrustum.FarPlaneTopRight), float4(1, 1, 1, 1));
		debug_renderer::add_line(Vector3DToFloat3(CameraFrustum.FarPlaneTopRight), Vector3DToFloat3(CameraFrustum.FarPlaneBottomRight), float4(1, 1, 1, 1));
		debug_renderer::add_line(Vector3DToFloat3(CameraFrustum.FarPlaneBottomRight), Vector3DToFloat3(CameraFrustum.FarPlaneBottomLeft), float4(1, 1, 1, 1));
		debug_renderer::add_line(Vector3DToFloat3(CameraFrustum.FarPlaneBottomLeft), Vector3DToFloat3(CameraFrustum.FarPlaneTopLeft), float4(1, 1, 1, 1));

		debug_renderer::add_line(Vector3DToFloat3(CameraFrustum.NearPlaneTopLeft), Vector3DToFloat3(CameraFrustum.FarPlaneTopLeft), float4(1, 1, 1, 1));
		debug_renderer::add_line(Vector3DToFloat3(CameraFrustum.NearPlaneTopRight), Vector3DToFloat3(CameraFrustum.FarPlaneTopRight), float4(1, 1, 1, 1));
		debug_renderer::add_line(Vector3DToFloat3(CameraFrustum.NearPlaneBottomRight), Vector3DToFloat3(CameraFrustum.FarPlaneBottomRight), float4(1, 1, 1, 1));
		debug_renderer::add_line(Vector3DToFloat3(CameraFrustum.NearPlaneBottomLeft), Vector3DToFloat3(CameraFrustum.FarPlaneBottomLeft), float4(1, 1, 1, 1));

		/* Draw Normals of the frustum */
		Vector3D Normal = Vector3D::ZeroVector();
		Vector3D StartPos = Vector3D::ZeroVector();
		Vector3D EndPos = Vector3D::ZeroVector();

		uint32 Vert = 0;
		for (uint32 i = 0; i < 6; ++i)
		{
			Normal = CameraFrustum.Planes[i].GetNormal();
			StartPos = CameraFrustum.PlaneCenters[i];
			EndPos = StartPos + (Normal * 2.0f);

			debug_renderer::add_line(Vector3DToFloat3(StartPos), Vector3DToFloat3(EndPos), float4(1, 0, 0, 1), float4(0, 1, 0, 1));
		}

		/* Draw AABBS */
		for (uint32 i = 0; i < AABBS.size(); ++i)
		{
			bool InFrustum = CameraFrustum.TestAABB(AABBS[i]->GetCenter(), AABBS[i]->GetExtent()) == PlaneIntersectionResult::Front;

			if (InFrustum)
			{
				AABBS[i]->DrawBox(float4(1, 0, 0, 1), BoxIndices);
			}
			else
			{
				AABBS[i]->DrawBox(float4(0, 1, 0, 1), BoxIndices);
			}
		}
	}

	void RenderImGui() override
	{
		// Create ImGui Test Window
		ImGui::Begin("Frustum");

		if (ImGui::DragFloat("Aspect Ratio", &CameraFrustum.AspectRatio, 0.01f, 0.1f, 5.0f) || ImGui::DragFloat("Scale", &CameraFrustum.WidthMultiplier, 0.01f, 0.1f, 1000.0f))
		{
			CameraFrustum.SetFrustumInternals(CameraFrustum.AspectRatio, CameraFrustum.WidthMultiplier, CameraFrustum.NearPlaneDist, CameraFrustum.FarPlaneDist);
		}

		ImGui::DragFloat("Far Plane Distance", &CameraFrustum.FarPlaneDist, 0.01f, 0.1f, 1000.0f);
		ImGui::DragFloat("Near Plane Distance", &CameraFrustum.NearPlaneDist, 0.01f, 0.1f, 1000.0f);

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();

		std::string AABBCountText = "AABBs: ";
		AABBCountText += std::to_string(AABBS.size());
		ImGui::Text(AABBCountText.c_str());

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Checkbox("Control Frustum", &AllowFrustumControl);

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();

		for (uint32 i = 0; i < AABBS.size(); ++i)
		{
			std::string ButtonName = "AABB ";
			ButtonName.append(std::to_string(i + 1));

			if (ImGui::Button(ButtonName.c_str()))
			{
				BoxImGui = AABBS[i];
				break;
			}
		}

		if (BoxImGui)
		{
			if (!BoxImGui->RenderImGui())
			{
				BoxImGui = nullptr;
			}
		}

		ImGui::End();
	}
};
