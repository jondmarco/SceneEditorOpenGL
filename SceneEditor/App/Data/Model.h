#pragma once

#include "Mesh.h"

#include <vector>
#include <numeric>

using std::vector;


class Model
{
public:

	friend class ModelLoader;
	friend class RenderSystem;
	friend class GameStorage;

	vec3 GetMaxPoint()
	{
		return _maxPoint;
	}

	vec3 GetMinPoint()
	{
		return _minPoint;
	}

private:

	Model() {}

	vector<Mesh> _subMeshes;

	vec3 _minPoint = vec3(std::numeric_limits<float>::max());
	vec3 _maxPoint = vec3(std::numeric_limits<float>::min());

	string modelFilePath;
};
