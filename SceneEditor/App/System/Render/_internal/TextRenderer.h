#pragma once

#include "ft2build.h"
#include FT_FREETYPE_H

#include "GL\glew.h"
#include "glm\glm.hpp"

#include <map>
#include <string>

class Shader;

using std::map;
using std::string;

using glm::vec3;
using glm::ivec2;


class TextRenderer
{
public:

	~TextRenderer();

	void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, vec3 color, GLuint screenWidth, GLuint screenHeight);

private:

	struct Character
	{
		GLuint TextureID;
		ivec2 Size;
		ivec2 Bearing;
		GLuint Advance;
	};

	void Initialize();
	void LoadCharacters(FT_Face face);
	void CreateRenderData();

	map<char, Character> Characters;
	Shader* textShader = nullptr;
	GLuint VAO, VBO;

	bool initialized = false;

	const string LOG_CHANNEL = "TextRenderer";
};
