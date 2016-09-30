#pragma once

#include "glm\glm.hpp"

using glm::vec2;
using glm::vec3;


struct Vertex 
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};
