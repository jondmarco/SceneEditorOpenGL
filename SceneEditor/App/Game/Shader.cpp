#include "Shader.h"

#include "Services\Logger.h"

#include <sstream>
#include <fstream>


Shader::Shader(const GLchar * vertexShaderPath, const GLchar * fragmentShaderPath)
{
	try
	{
		const string vertexShaderSource = ReadShaderFile(vertexShaderPath);
		const string fragmentShaderSource = ReadShaderFile(fragmentShaderPath);

		GLuint vertexShader = CompileShader(vertexShaderSource.c_str(), GL_VERTEX_SHADER);
		GLuint fragmentShader = CompileShader(fragmentShaderSource.c_str(), GL_FRAGMENT_SHADER);

		program = LinkShaderProgram(vertexShader, fragmentShader);

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}
	catch (std::runtime_error e)
	{
		string error = e.what();
		Logger::LogError(LOG_CHANNEL, "Shader initialization failed. This shader is in invalid state and cannot be used. Error: " + error);
		isValid = false;
	}
}

Shader::~Shader()
{
	glDeleteProgram(program);
}

void Shader::Use()
{
	if (isValid)
	{
		glUseProgram(program);
	}
	else
	{
		Logger::LogError(LOG_CHANNEL, "Attempt to use shader failed: shader is in invalid state");
	}
}


const string Shader::ReadShaderFile(const GLchar* shaderPath)
{
	std::ifstream shaderFile;

	// Ensures ifstream object can throw exceptions
	shaderFile.exceptions(std::ifstream::badbit);

	try
	{
		shaderFile.open(shaderPath);

		std::stringstream shaderStream;
		shaderStream << shaderFile.rdbuf();

		shaderFile.close();

		return shaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		Logger::LogError(LOG_CHANNEL, "Reading shader file failed. Path: " + string(shaderPath));
		throw std::runtime_error("Reading shader file failed");
	}
}

GLuint Shader::CompileShader(const GLchar* shaderSource, GLuint shaderType)
{
	GLuint shader = glCreateShader(shaderType);

	glShaderSource(shader, 1, &shaderSource, NULL);
	glCompileShader(shader);

	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		GLchar infoLog[512];
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		Logger::LogError(LOG_CHANNEL, "Compilation failed " + string(infoLog));
		throw std::runtime_error("Shader compilation failed");
	}

	return shader;
}

inline GLuint Shader::LinkShaderProgram(GLuint vertexShader, GLuint fragmentShader)
{
	GLuint shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	glLinkProgram(shaderProgram);

	GLint success;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

	if (!success)
	{
		GLchar infoLog[512];
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		Logger::LogError(LOG_CHANNEL, "Linking failed " + string(infoLog));
		throw std::runtime_error("Linking shader program failed");
	}

	return shaderProgram;
}

GLuint Shader::GetUniformLocation(string uniformName)
{
	return glGetUniformLocation(program, uniformName.c_str());
}
