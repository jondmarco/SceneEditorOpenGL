#pragma once

#include <string>
#include "glm\glm.hpp"

using std::string;
using glm::vec3;

class SceneEditor;


class ITransformTool
{
public:

	void ProcessToolInput(SceneEditor* pContext, bool keys[], float deltaTime);
	virtual string GetToolName() = 0;
	virtual string GetActiveAxis() = 0;

protected:

	virtual void SetActiveAxis(bool keys[]);
	virtual vec3 CalculateTransformCoefficients(float deltaTime) = 0;
	virtual void ApplyTransform(SceneEditor* pContext, bool keys[], float deltaTime, vec3 transformParameters) = 0;

	// Manipulation axes (U stands for Uniform)
	enum Axes { X, Y, Z, U };

	// X is active by default for the majority of tools
	Axes _activeAxis = Axes::X;

	const string _toolName = "TransformTool Interface";
};
