#pragma once

#include "_internal\TextRenderer.h"
#include "../../Game/Grid.h"

#include "Base\OpenGLApp.h"

#include "GL\glew.h"
#include "glm\glm.hpp"

#include <vector>


class Light;
class Scene;
class Shader;
class Mesh;
class Model;
class Camera;
class SceneEntity;
struct Texture;

using std::vector;
using glm::mat4;
using glm::vec3;


class RenderSystem
{
public:

	RenderSystem(OpenGLApp& context) : _context(context) { };

	void RenderScene(Scene& scene, Shader& shader, bool isInEditMode);
	void RenderText(string text, GLfloat x, GLfloat y, GLfloat scale, vec3 color, GLuint screenWidth, GLuint screenHeight);
	void RenderGrid(Shader& shader, Camera &camera);

	void ClearScreen();
	void SwapBuffers();

private:

	void RenderModel(Model* model, Shader& shader);
	void RenderMesh(Mesh& model, Shader& shader);

	void SetStateVariables(SceneEntity* entity, Shader& shader, bool isInEditMode);
	void SetModelMatrix(mat4& modelMatrix, Shader& shader);
	void SetViewAndProjectionMatrices(Camera& camera, Shader& shader);
	void setFragmentShaderColor(vec3& RGB, Shader& shader);
	void setMaterialProperties(float metalness, float roughness, Shader& shader);
	void setViewPosAndLights(vector<Light*>& lightSources, vec3& viewPos, Shader& shader);
	void BindTextures(Shader& shader, vector<Texture>& textures, string textureType, unsigned int startingTextureIndex);
	void UnBindTextures();

	OpenGLApp& _context;
	TextRenderer _textRenderer;

	GLuint depthMapFBO;
	GLuint depthMap;
};
