#pragma once

#include "ITransformTool.h"


class ScaleTool : public ITransformTool
{
public:

	string GetToolName() override;
	virtual string GetActiveAxis() override;

private:

	virtual void SetActiveAxis(bool keys[]) override;
	vec3 CalculateTransformCoefficients(float deltaTime) override;
	void ApplyTransform(SceneEditor* pContext, bool keys[], float deltaTime, vec3 scaleCoefficients) override;

	const string _toolName = "Scale Tool";

	// We want uniform scaling to be active by default
	Axes _activeAxis = Axes::U;

	// Controls scaling sensitivity to user input
	const float _scaleSensitivityCoefficient = 3.0f;
};
