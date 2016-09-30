#include "RenderSystem.h"

#include "Data\Model.h"
#include "Data\Mesh.h"

#include "Game\Scene.h"
#include "Game\SceneEntity.h"
#include "Game\Camera.h"
#include "Game\Shader.h"
#include "Game\Light.h"

#include "glm\glm.hpp"
#include "glm/gtc/type_ptr.hpp" 

using glm::mat4;


void RenderSystem::RenderScene(Scene& scene, Shader& shader, bool isInEditMode)
{
	shader.Use();

	// View and projection matrices are constant during any given frame
	// Thus, we only need to set them once
	SetViewAndProjectionMatrices(scene.GetCamera(), shader);

	//set our view position and our scenes lights
	setViewPosAndLights(scene.GetLightSources(), scene.GetCamera()._position, shader);

	// Get all visual components currently in the scene and render them
	auto& sceneEntities = scene.GetEntities();
	for (auto& entity : sceneEntities)
	{
		setMaterialProperties((float)entity->getMetallness(), (float)entity->getRoughness(), shader);
		setFragmentShaderColor(entity->GetBaseColor(), shader);
		SetModelMatrix(entity->GetModelMatrix(), shader);

		SetStateVariables(entity, shader, isInEditMode);

		auto model = entity->GetModel();
		RenderModel(model, shader);
	}

	//update the light variables based on its model values
	if (isInEditMode)
	{
		for (auto& light : scene.GetLightSources())
		{
			glm::vec4 newPos = light->getEntity()->GetModelMatrix() * glm::vec4(light->getPosition(), 1.0);
			light->setPosition(vec3(newPos.x, newPos.y, newPos.z));
			light->setColor(light->_lightModel->GetBaseColor());
		}

		//draw the grid
		RenderGrid(shader, scene.GetCamera());
	}
}

void RenderSystem::RenderText(string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color, GLuint screenWidth, GLuint screenHeight)
{
	_textRenderer.RenderText(text, x, y, scale, color, screenWidth, screenHeight);
}

void RenderSystem::RenderGrid(Shader& shader, Camera &camera)
{
	glUniform1i(shader.GetUniformLocation("is_selected"), false);
	glUniform1i(shader.GetUniformLocation("is_valid"), true);

	setFragmentShaderColor(vec3(0.5, 0.5, 0.5), shader);
	SetModelMatrix((Grid::getInstance())->GetModelMatrix(), shader);

	// Draw Grid
	glBindVertexArray(Grid::getInstance()->VAO);
	glDrawElements(GL_LINES, Grid::getInstance()->_indices.size(), GL_UNSIGNED_INT, 0);

	//reset the viewport
	int w, h;
	glfwGetWindowSize(_context._window, &w, &h);
	glViewport(0, 0, w, h);
}

void RenderSystem::ClearScreen()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderSystem::SwapBuffers()
{
	_context.SwapBuffers();
}

void RenderSystem::RenderModel(Model* model, Shader& shader)
{
	for (auto& submesh : model->_subMeshes)
	{
		RenderMesh(submesh, shader);
	}
}

void RenderSystem::RenderMesh(Mesh& mesh, Shader& shader)
{
	bool has_textures = mesh.MeshMaterial.DiffuseMaps.size() != 0 || mesh.MeshMaterial.SpecularMaps.size() != 0;

	if (has_textures)
	{
		glUniform4f(shader.GetUniformLocation("baseColor"), 1.0f, 1.0f, 1.0f, 1.0f);
	}

	glUniform1i(shader.GetUniformLocation("has_texture"), has_textures);

	// Bind diffuse and specular textures
	BindTextures(shader, mesh.MeshMaterial.DiffuseMaps, "diffuse", 0);
	BindTextures(shader, mesh.MeshMaterial.SpecularMaps, "specular", mesh.MeshMaterial.DiffuseMaps.size());

	// Draw
	glBindVertexArray(mesh.VAO);
	glDrawElements(GL_TRIANGLES, mesh._indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// Unbind textures
	UnBindTextures();
}

void RenderSystem::SetViewAndProjectionMatrices(Camera& camera, Shader& shader)
{
	mat4 projection = camera.GetProjectionMatrix();
	mat4 view = camera.GetViewMatrix();

	glUniformMatrix4fv(shader.GetUniformLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(shader.GetUniformLocation("view"), 1, GL_FALSE, glm::value_ptr(view));
}

void RenderSystem::SetModelMatrix(mat4& modelMatrix, Shader& shader)
{
	glUniformMatrix4fv(shader.GetUniformLocation("model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
}

void RenderSystem::SetStateVariables(SceneEntity* entity, Shader& shader, bool isInEditMode)
{
	if (isInEditMode)
	{
		glUniform1i(shader.GetUniformLocation("is_selected"), entity->IsSelected());
		glUniform1i(shader.GetUniformLocation("is_valid"), entity->IsValid());
		glUniform1i(shader.GetUniformLocation("is_light"), entity->isLight());
	}
	else
	{
		glUniform1i(shader.GetUniformLocation("is_selected"), false);
		glUniform1i(shader.GetUniformLocation("is_valid"), true);
	}
}

void RenderSystem::setFragmentShaderColor(vec3& RGB, Shader& shader)
{
	glUniform4f(shader.GetUniformLocation("baseColor"), RGB.x, RGB.y, RGB.z, 1.0f);
}

void RenderSystem::setMaterialProperties(float metalness, float roughness, Shader& shader)
{
	glUniform1f(shader.GetUniformLocation("metallness"), (float)metalness);
	glUniform1f(shader.GetUniformLocation("roughness"), (float)roughness);
}

void RenderSystem::setViewPosAndLights(std::vector<Light*>& lightSources, vec3& viewPos, Shader& shader)
{

	vec3 lightPos;
	vec3 lightColor;

	glUniform1i(shader.GetUniformLocation("numLights"), lightSources.size());
	for (int i = 0; i < lightSources.size(); ++i)
	{
		lightPos = lightSources.at(i)->getPosition();
		lightColor = lightSources.at(i)->getColor();
		glUniform3f(shader.GetUniformLocation("light[" + std::to_string(i) + "].position"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(shader.GetUniformLocation("light[" + std::to_string(i) + "].color"), lightColor.x, lightColor.y, lightColor.z);
	}

	glUniform3f(shader.GetUniformLocation("camPos"), viewPos.x, viewPos.y, viewPos.z);

}
void RenderSystem::BindTextures(Shader& shader, vector<Texture>& textures, string textureType, unsigned int startingTextureIndex)
{
	// Bind textures
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		auto textureNumber = i + startingTextureIndex;
		auto textureUniformLocation = shader.GetUniformLocation("material_" + textureType);

		glActiveTexture(GL_TEXTURE0 + textureNumber);
		glUniform1i(textureUniformLocation, textureNumber);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	// Reset active texture
	glActiveTexture(GL_TEXTURE0);
}

void RenderSystem::UnBindTextures()
{
	for (unsigned int i = 0; i <= 4; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}
