#pragma once

#include "Data/Core/Texture.h"

#include <vector>

using std::vector;


struct Material
{
	vector<Texture> SpecularMaps;
	vector<Texture> DiffuseMaps;
};
