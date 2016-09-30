#pragma once
#include "Data/Core/Vertex.h"
#include <vector>

#include "GL/glew.h"
#include "glm\glm.hpp"

#include "SceneEntity.h"
#include "Services/ModelLoader.h"
#include "Game/Camera.h"

using std::vector;
using glm::mat4;

class Grid
{
public:
	friend class RenderSystem;

	static Grid* getInstance();
	~Grid();
	
	mat4& GetModelMatrix();

private:

	Grid();
	Grid(Grid &grid);
	static Grid* _instance;

	void buildGrid();
	void buildGrid(int sizeX, int sizeY);
	void ConstructRenderData();
	void initializeModelMatrix();

	//Grid data
	vector<Vertex> _vertices;
	vector<GLuint> _indices;

	mat4 _modelMatrix;
	ModelLoader _modelLoader;

	//Render data
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
};
