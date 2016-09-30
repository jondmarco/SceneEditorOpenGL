#pragma once

#include "ITransformTool.h"


class RotationTool : public ITransformTool
{
public:

	string GetToolName() override;
	virtual string GetActiveAxis() override;

private:

	vec3 CalculateTransformCoefficients(float deltaTime) override;
	void ApplyTransform(SceneEditor* pContext, bool keys[], float deltaTime, vec3 rotationAxis) override;

	const string _toolName = "Rotation Tool";

	// Controls rotation sensitivity to user input
	const float _rotationSensitivityCoefficient = 150.0f;
};
