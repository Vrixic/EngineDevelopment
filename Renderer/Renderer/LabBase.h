#pragma once

class LabBase
{
public:
	virtual void Update(float deltaTime) = 0;

	virtual void RenderImGui() = 0;

	virtual ~LabBase() { }
};
