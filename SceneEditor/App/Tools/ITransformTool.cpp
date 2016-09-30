#include "ITransformTool.h"

#include "GL\glew.h"
#include "GLFW\glfw3.h"


// Making use of Template Method pattern
void ITransformTool::ProcessToolInput(SceneEditor* pContext, bool keys[], float deltaTime)
{
	SetActiveAxis(keys);
	vec3 transformParameters = CalculateTransformCoefficients(deltaTime);
	ApplyTransform(pContext, keys, deltaTime, transformParameters);
}

void ITransformTool::SetActiveAxis(bool keys[])
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
}
