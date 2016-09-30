#include "RotationTool.h"

#include "SceneEditor.h"

#include "Game\SceneEntity.h"

#include "GL\glew.h"
#include "GLFW\glfw3.h"


vec3 RotationTool::CalculateTransformCoefficients(float deltaTime)
{
	vec3 rotationAxis(0.0f);

	switch (_activeAxis)
	{
	case Axes::X:
		rotationAxis.x = 1.0f;
		break;
	case Axes::Y:
		rotationAxis.y = 1.0f;
		break;
	case Axes::Z:
		rotationAxis.z = 1.0f;
		break;
	}

	return rotationAxis;
}

void RotationTool::ApplyTransform(SceneEditor * pContext, bool keys[], float deltaTime, vec3 rotationAxis)
{
	auto selectedEntity = pContext->GetSelectedEntity();

	if (selectedEntity != nullptr)
	{
		float rotationAngle = _rotationSensitivityCoefficient * deltaTime;

		if (keys[GLFW_KEY_UP] || keys[GLFW_KEY_RIGHT])
		{
			selectedEntity->Rotate(rotationAngle, rotationAxis.x, rotationAxis.y, rotationAxis.z);
		}
		else if (keys[GLFW_KEY_DOWN] || keys[GLFW_KEY_LEFT])
		{
			selectedEntity->Rotate(rotationAngle, -rotationAxis.x, -rotationAxis.y, -rotationAxis.z);
		}

		// Propagate transform to children
		int parentId = pContext->GetSelectedEntityIndex();

		for each(int children in selectedEntity->GetChildren()) {
			pContext->SelectEntityAtIndex(children);
			ApplyTransform(pContext, keys, deltaTime, rotationAxis);
		}
		pContext->SelectEntityAtIndex(parentId);
	}
}

string RotationTool::GetToolName()
{

	return _toolName;
}

string RotationTool::GetActiveAxis()
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
	}

	return activeAxis;
}
