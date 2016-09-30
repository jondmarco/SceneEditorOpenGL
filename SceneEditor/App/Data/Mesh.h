#pragma once

#include "Data\Material.h"
#include "Data/Core/Vertex.h"

#include "GL/glew.h"

#include <vector>

class Shader;
struct Texture;

using std::vector;


class Mesh
{
public:

	friend class RenderSystem;

	Mesh(vector<Vertex>& vertices, vector<GLuint>& indices, Material& material);
	
	Material MeshMaterial;

private:

	void ConstructRenderData();

	// Mesh Data
	vector<Vertex> _vertices;
	vector<GLuint> _indices;

	// Render data 
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
};
