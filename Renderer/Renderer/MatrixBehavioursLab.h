#pragma once
#include "LabBase.h"
#include "Vrixic/Math/VrixicMath.h"
#include "debug_renderer.h"
#include "InputManager.h"
#include "External/ImGui/imgui.h"

class MatrixBehavioursLab : public LabBase
{
private:

	Matrix4D LookAtMatrix, TurnToMatrix, CharacterMatrix;
	Vector3D CharacterPosition;
	Vector3D TurnToRotation;
	Vector3D CameraPosition;

	const float CharcterSpeed = 5.0f;

public:
	MatrixBehavioursLab()
	{
		LookAtMatrix.SetIdentity();
		LookAtMatrix.SetTranslation(Vector3D(-10.0, 5.0f, 0.0f));
		TurnToMatrix.SetIdentity();
		TurnToMatrix.SetTranslation(Vector3D(10.0, 5.0f, 0.0f));
		CharacterMatrix.SetIdentity();

		CharacterPosition = Vector3D(0.0f, 0.0f, 0.0f);

		CharacterMatrix.SetTranslation(CharacterPosition);
		CharacterPosition.X = 0;

		TurnToRotation = Vector3D::ZeroVector();

		TurnToMatrix = VrixicMath::LookAt(TurnToMatrix[3].ToVector3D(), CharacterMatrix[3].ToVector3D(), Vector3D(0, 1, 0));
	}

	~MatrixBehavioursLab()
	{

	}

public:
	void Update(float deltaTime) override
	{
		using namespace end;

		float CharacterSpeedZ = (InputManager::GetInstance().GetKeyState(KEY_ARROW_UP) - InputManager::GetInstance().GetKeyState(KEY_ARROW_DOWN));
		float CharacterSpeedX = (InputManager::GetInstance().GetKeyState(KEY_ARROW_RIGHT) - InputManager::GetInstance().GetKeyState(KEY_ARROW_LEFT));

		CharacterPosition.Z = CharcterSpeed * deltaTime * CharacterSpeedZ;
		//CharacterPosition.X += CharcterSpeed * deltaTime * CharacterSpeedX;

		// Update CharacterMatrix with input 
		if (CharacterSpeedZ != 0.0f)
		{
			Matrix4D TransMat = Matrix4D::Identity();
			TransMat.TranslateMatrix(CharacterPosition);
			CharacterMatrix = TransMat * CharacterMatrix;
		}

		/* rotate */
		if (CharacterSpeedX != 0.0f)
		{
			CharacterMatrix = Matrix4D::MakeRotY(50.0f * deltaTime * CharacterSpeedX) * CharacterMatrix;
		}		

		// Update TurnToMatrix to turn toward character
		TurnToMatrix = VrixicMath::TurnTo(deltaTime, 1.25f, CharacterMatrix[3].ToVector3D(), TurnToMatrix);

		LookAtMatrix = VrixicMath::LookAt(LookAtMatrix[3].ToVector3D(), CharacterMatrix[3].ToVector3D(), Vector3D(0, 1, 0));

		const float LineLength = 3.0f;

		// Draw LootAtMatrix
		Vector4D P = LookAtMatrix[3];

		Vector4D R = P + (LookAtMatrix[0] * LineLength);
		Vector4D Y = P + (LookAtMatrix[1] * LineLength);
		Vector4D F = P + (LookAtMatrix[2] * LineLength);

		debug_renderer::add_line(float3(P.X, P.Y, P.Z), float3(R.X, R.Y, R.Z), float4(1, 0, 0, 1));
		debug_renderer::add_line(float3(P.X, P.Y, P.Z), float3(Y.X, Y.Y, Y.Z), float4(0, 1, 0, 1));
		debug_renderer::add_line(float3(P.X, P.Y, P.Z), float3(F.X, F.Y, F.Z), float4(0, 0, 1, 1));

		// Draw TurnToMatrix
		P = TurnToMatrix[3];
		R = P + (TurnToMatrix[0] * LineLength);
		Y = P + (TurnToMatrix[1] * LineLength);
		F = P + (TurnToMatrix[2] * LineLength);

		/* DEBUG -> Draw Target Norm */
		Vector3D ToTarget = P.ToVector3D() + ((CharacterMatrix[3].ToVector3D() - P.ToVector3D()).Normalize() * LineLength);
		debug_renderer::add_line(float3(P.X, P.Y, P.Z), float3(ToTarget.X, ToTarget.Y, ToTarget.Z), float4(1, 1, 0, 1));

		debug_renderer::add_line(float3(P.X, P.Y, P.Z), float3(R.X, R.Y, R.Z), float4(1, 0, 0, 1));
		debug_renderer::add_line(float3(P.X, P.Y, P.Z), float3(Y.X, Y.Y, Y.Z), float4(0, 1, 0, 1));
		debug_renderer::add_line(float3(P.X, P.Y, P.Z), float3(F.X, F.Y, F.Z), float4(0, 0, 1, 1));

		// Draw CharacterMatrix
		P = CharacterMatrix[3];
		R = P + (CharacterMatrix[0] * LineLength);
		Y = P + (CharacterMatrix[1] * LineLength);
		F = P + (CharacterMatrix[2] * LineLength);

		debug_renderer::add_line(float3(P.X, P.Y, P.Z), float3(R.X, R.Y, R.Z), float4(1, 0, 0, 1));
		debug_renderer::add_line(float3(P.X, P.Y, P.Z), float3(Y.X, Y.Y, Y.Z), float4(0, 1, 0, 1));
		debug_renderer::add_line(float3(P.X, P.Y, P.Z), float3(F.X, F.Y, F.Z), float4(0, 0, 1, 1));
	}

	void RenderImGui() override
	{
		// Create ImGui Test Window
		//ImGui::Begin("MatrixBehaviours");

		//ImGui::End();
	}
};
