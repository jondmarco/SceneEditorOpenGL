#pragma once

#include "GL/glew.h"

#include <string>

using std::string;


struct Texture {
	GLuint id;
	string type;
	string path;
};
