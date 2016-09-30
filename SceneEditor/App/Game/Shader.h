#pragma once

#include "GL\glew.h"

#include <string>

using std::string;


class Shader
{
public:

	Shader(const GLchar* vertexShaderPath, const GLchar* fragmentShaderPath);
	~Shader();

	void Use();
	GLuint GetUniformLocation(string uniformName);

private:

	GLuint program;
	GLboolean isValid = true;

	const string ReadShaderFile(const GLchar* shaderPath);
	GLuint CompileShader(const GLchar* shaderSource, GLuint shaderType);
	GLuint LinkShaderProgram(GLuint vertexShader, GLuint fragmentShader);

	const string LOG_CHANNEL = "Shader";
};

