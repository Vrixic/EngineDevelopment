#include "dev_app.h"

#include <iostream>
//TODO include debug_renderer.h and pools.h and anything else you might need here
#include "Vrixic/GenericDefines.h"
#include <vector>

#include "InputManager.h"

#include "ParticlesLab.h"
#include "MatrixBehavioursLab.h"
#include "FrustumLab.h"

#include "External/ImGui/imgui.h"
#include "External/ImGui/imgui_impl_win32.h"
#include "External/ImGui/imgui_impl_dx11.h"
#include "BVHLab.h"

namespace end
{
	end::renderer_t* Renderer; 

	double delta_time = 0.0;

	float TotalTime = 0.0f;
	float TimePassed = 0;

	Vector4D GridColor = Vector4D(0, 0, 0, 1);

	/* World space transformations ONLY  -> for camera */
	Matrix4D ViewMat;
	Vector3D CameraPosition;

	bool ShowParticles = true;
	ParticlesLab* Particles = new ParticlesLab();

	MatrixBehavioursLab* MatrixBehaviours = new MatrixBehavioursLab();

	FrustumLab* Frustum = new FrustumLab();

	BVHLab* BVH;

	LabBase* CurrentLab = Frustum;


	double dev_app_t::get_delta_time()const
	{
		return delta_time;
	}

	void dev_app_t::UpdateCameraPositionByView()
	{
		CameraPosition = ViewMat[3].ToVector3D();
	}

	void dev_app_t::SetViewMatrix(float4x4_a mat)
	{
		ViewMat = reinterpret_cast<Matrix4D&>(mat);
	}

	float4x4_a dev_app_t::GetViewMatrix() const
	{
		return reinterpret_cast<float4x4_a&>(ViewMat);
	}

	dev_app_t::dev_app_t()
	{
		std::cout << "Log whatever you need here.\n"; // Don’t forget to include <iostream>
	}

	dev_app_t::~dev_app_t()
	{
		delete Particles;
		delete MatrixBehaviours;
		delete Frustum;
		delete BVH;
	}

	double calc_delta_time()
	{
		static std::chrono::time_point<std::chrono::high_resolution_clock> last_time = std::chrono::high_resolution_clock::now();

		std::chrono::time_point<std::chrono::high_resolution_clock> new_time = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed_seconds = new_time - last_time;
		last_time = new_time;

		return Math::Min(1.0 / 15.0, elapsed_seconds.count());
	}

	void DrawDebugGrid(uint32 gridWidth, uint32 gridHeight, float gridSpace, float4 color)
	{
		if (gridWidth % 2 == 0)
		{
			gridWidth++;
		}
		if (gridHeight % 2 == 0)
		{
			gridHeight++;
		}

		float GridWidth = (gridWidth / 2) * gridSpace;
		float GridHeight = (gridHeight / 2) * gridSpace;

		float StartX = GridWidth;
		float StartZ = GridHeight;

		for (uint32 i = 0; i < gridWidth; ++i)
		{
			end::debug_renderer::add_line(float3(-StartX, 0, StartZ), float3(StartX, 0, StartZ), color);
			StartZ -= gridSpace;
		}

		StartZ = GridHeight;

		for (uint32 i = 0; i < gridWidth; ++i)
		{
			end::debug_renderer::add_line(float3(StartX, 0, -StartZ), float3(StartX, 0, StartZ), color);
			StartX -= gridSpace;
		}
	}

	void SelectCurrentLab()
	{
		if (InputManager::GetInstance().GetKeyState(KEY_1))
		{
			ShowParticles = true;
			CurrentLab = Particles;
		}

		if (InputManager::GetInstance().GetKeyState(KEY_2))
		{
			ShowParticles = false;
			CurrentLab = MatrixBehaviours;
		}

		if (InputManager::GetInstance().GetKeyState(KEY_3))
		{
			ShowParticles = false;
			CurrentLab = Frustum;
		}

		if (InputManager::GetInstance().GetKeyState(KEY_4))
		{
			ShowParticles = false;
			CurrentLab = BVH;
		}
	}

	end::float3 Vector3DToFloat3(const Vector3D& v)
	{
		return end::float3(v.X, v.Y, v.Z);
	}

	void dev_app_t::update()
	{
		delta_time = calc_delta_time();
		TimePassed += delta_time;
		TotalTime += delta_time;

		GridColor += Vector3D(Math::RandomRange(1.0f, 5.0f), Math::RandomRange(1.0f, 10.0f), Math::RandomRange(1.0f, 20.0f)) * static_cast<float>(delta_time);

		Vector3D TempColor;
		TempColor.X = (sin(GridColor.X * 0.1f) + 1.0f) * 0.5f;
		TempColor.Y = (sin(GridColor.Y * 0.1f) + 1.0f) * 0.5f;
		TempColor.Z = (sin(GridColor.Z * 0.1f) + 1.0f) * 0.5f;

		// Draw Grid
		DrawDebugGrid(15, 15, 1.0f, float4(TempColor.X, TempColor.Y, TempColor.Z, 1.0f));

		// Select current lab based on user input
		SelectCurrentLab();

		// Update Camera Input
		{
			float CanControlCamera = InputManager::GetInstance().GetMouseState(MOUSE_RIGHT_DOWN);

			if (CanControlCamera)
			{
				float CameraSpeedX = (InputManager::GetInstance().GetKeyState(KEY_D) - InputManager::GetInstance().GetKeyState(KEY_A));
				float CameraSpeedZ = (InputManager::GetInstance().GetKeyState(KEY_W) - InputManager::GetInstance().GetKeyState(KEY_S));
				float CameraDeltaX = 20.0f * delta_time * CameraSpeedX;
				float CameraDeltaZ = 20.0f * delta_time * CameraSpeedZ;

				MousePoint MouseDelta = InputManager::GetInstance().GetMouseDelta();

				float TotalPitch = (Math::DegreesToRadians(65.0f) * (MouseDelta.Y / 720.0f)) * -50.0f;
				float TotalYaw = (Math::DegreesToRadians(65.0f) * (MouseDelta.X / 1280.0f)) * -50.0f;

				Vector3D CameraTranslation(CameraDeltaX, 0.0f, CameraDeltaZ);
				Matrix4D Translation = Matrix4D::Identity();
				Translation.SetTranslation(CameraTranslation);
				ViewMat = Translation * ViewMat;

				Matrix4D PitchMatrix = Matrix4D::MakeRotX(TotalPitch);
				ViewMat = PitchMatrix * ViewMat;

				Matrix4D YawMatrix = Matrix4D::Identity();
				Vector4D CamPos = ViewMat[3];
				YawMatrix = Matrix4D::MakeRotY(TotalYaw);
				ViewMat = ViewMat * YawMatrix;
				ViewMat.SetTranslation(CamPos.ToVector3D());
			}
		}

		CurrentLab->Update(delta_time);
		if (ShowParticles)
		{
			Particles->SetSortedEmitterColor(TempColor);
		}
	}

	void dev_app_t::RenderImGui()
	{
		/* Start ImGui Frame */
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		CurrentLab->RenderImGui();

		// Assemble Together draw data
		ImGui::Render();

		// Render Draw Data
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	void dev_app_t::SetRenderer(end::renderer_t* ren)
	{
		Renderer = ren;
	}

	void dev_app_t::CreateBVH()
	{
		BVH = new BVHLab(*Renderer, "../../terrain.bin");

		CurrentLab = BVH;
	}
}

