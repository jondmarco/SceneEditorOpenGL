#pragma once

#include "ITransformTool.h"


class TranslationTool : public ITransformTool
{
public:

	string GetToolName() override;
	virtual string GetActiveAxis() override;

private:

	vec3 CalculateTransformCoefficients(float deltaTime) override;
	void ApplyTransform(SceneEditor* pContext, bool keys[], float delataTime, vec3 translationCoefficients) override;

	const string _toolName = "Translation Tool";
};
