#include "TranslationTool.h"

#include "SceneEditor.h"

#include "Game\SceneEntity.h"

#include "GL\glew.h"
#include "GLFW\glfw3.h"


vec3 TranslationTool::CalculateTransformCoefficients(float deltaTime)
{
	vec3 translationCoefficients(0.0f);

	switch (_activeAxis)
	{
	case Axes::X:
		translationCoefficients.x = deltaTime;
		break;
	case Axes::Y:
		translationCoefficients.y = deltaTime;
		break;
	case Axes::Z:
		translationCoefficients.z = deltaTime;
		break;
	}

	return translationCoefficients;
}

void TranslationTool::ApplyTransform(SceneEditor * pContext, bool keys[], float deltaTime, vec3 translationCoefficients)
{
	auto selectedEntity = pContext->GetSelectedEntity();

	if (selectedEntity != nullptr)
	{
		if (keys[GLFW_KEY_UP] || keys[GLFW_KEY_RIGHT])
		{
			selectedEntity->Translate(translationCoefficients.x, translationCoefficients.y, translationCoefficients.z);
		}
		else if (keys[GLFW_KEY_DOWN] || keys[GLFW_KEY_LEFT])
		{
			selectedEntity->Translate(-translationCoefficients.x, -translationCoefficients.y, -translationCoefficients.z);
		}

		// Propagate transform to children
		int parentId = pContext->GetSelectedEntityIndex();

		for each(int children in selectedEntity->GetChildren()) {
			pContext->SelectEntityAtIndex(children);
			ApplyTransform(pContext, keys, deltaTime, translationCoefficients);
		}
		pContext->SelectEntityAtIndex(parentId);
	}
}

string TranslationTool::GetToolName()
{
	return _toolName;
}

string TranslationTool::GetActiveAxis()
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
