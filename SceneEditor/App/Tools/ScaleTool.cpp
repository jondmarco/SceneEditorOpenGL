#include "ScaleTool.h"

#include "SceneEditor.h"

#include "Game\SceneEntity.h"

#include "GL\glew.h"
#include "GLFW\glfw3.h"


void ScaleTool::SetActiveAxis(bool keys[])
{
	if (keys[GLFW_KEY_X])
	{
		_activeAxis = Axes::X;
	}
	else if (keys[GLFW_KEY_Y])
	{
		_activeAxis = Axes::Y;
	}
	else if (keys[GLFW_KEY_Z])
	{
		_activeAxis = Axes::Z;
	}
	else if (keys[GLFW_KEY_U])
	{
		_activeAxis = Axes::U;
	}
}

vec3 ScaleTool::CalculateTransformCoefficients(float deltaTime)
{
	vec3 scaleCoefficients(0.0f);

	auto scaleMagnitude = _scaleSensitivityCoefficient * deltaTime;

	switch (_activeAxis)
	{
	case Axes::X:
		scaleCoefficients.x =scaleMagnitude;
		break;
	case Axes::Y:
		scaleCoefficients.y = scaleMagnitude;
		break;
	case Axes::Z:
		scaleCoefficients.z = scaleMagnitude;
		break;
	case Axes::U:
		scaleCoefficients.x = scaleMagnitude;
		scaleCoefficients.y = scaleMagnitude;
		scaleCoefficients.z = scaleMagnitude;
		break;
	}

	return scaleCoefficients;
}

void ScaleTool::ApplyTransform(SceneEditor * pContext, bool keys[], float deltaTime, vec3 scaleCoefficients)
{
	auto selectedEntity = pContext->GetSelectedEntity();

	if (selectedEntity != nullptr)
	{
		if (keys[GLFW_KEY_UP] || keys[GLFW_KEY_RIGHT])
		{
			selectedEntity->Scale(1 + scaleCoefficients.x, 1 + scaleCoefficients.y, 1 + scaleCoefficients.z);
		}
		else if (keys[GLFW_KEY_DOWN] || keys[GLFW_KEY_LEFT])
		{
			selectedEntity->Scale(1 - scaleCoefficients.x, 1 - scaleCoefficients.y, 1 - scaleCoefficients.z);
		}

		// Propagate transform to children
		int parentId = pContext->GetSelectedEntityIndex();

		for each(int children in selectedEntity->GetChildren()) {
			pContext->SelectEntityAtIndex(children);
			ApplyTransform(pContext, keys, deltaTime, scaleCoefficients);
		}
		pContext->SelectEntityAtIndex(parentId);
	}
}

string ScaleTool::GetToolName()
{
	return _toolName;
}

string ScaleTool::GetActiveAxis()
{
	string activeAxis;

	switch (_activeAxis)
	{
	case Axes::X:
		activeAxis = "X";
		break;
	case Axes::Y:
		activeAxis = "Y";
		break;
	case Axes::Z:
		activeAxis = "Z";
		break;
	case Axes::U:
		activeAxis = "Uniform";
		break;
	}

	return activeAxis;
}
