#pragma once
#include "Grid.h"
#include "Data/Model.h"
#include "glm\gtc\matrix_transform.hpp"

#define _FILE_AXIS_ARROWS "OpenGL_APP\\Assets\\Models\\Arrow\\arrow.obj"
#define spacebtwn 1.0f

float scaleX, scaleY, scaleZ;


Grid* Grid::_instance;

Grid::~Grid()
{
}

Grid::Grid()
{
}

Grid::Grid(Grid &grid)
{
}

Grid* Grid::getInstance()
{
	if (_instance == nullptr)
	{
		_instance = new Grid();
		_instance->buildGrid();
	}
	return _instance;
}

void Grid::buildGrid()
{
	buildGrid(26, 26);
}

void Grid::buildGrid(int sizeX, int sizeY)
{
	vector<Vertex> translationPoints;
	vector<Vertex> gridPoints;

	scaleX = 0.3f;
	scaleY = 0.3f;
	scaleZ = 0.3f;

	float offsetX = 0 - ((spacebtwn * sizeX) / 2.0f);
	float offsetY = 0 - ((spacebtwn * sizeY) / 2.0f);
	float offsetZ = offsetY;

	//populate the points for the line we will translate sizeX times to create our grid
	for (int i = 0; i < sizeY; ++i)
	{
		Vertex temp;
		temp.Position.x = 0 + offsetX;
		temp.Position.y = spacebtwn * i + offsetY;
		temp.Position.z = offsetZ;
		translationPoints.push_back(temp);
	}

	//translate the points to populate the grid points and indices
	for (int i = 0; i < sizeY; ++i)
	{
		for (int j = 0; j < sizeX; ++j)
		{
			//add the point coordinates
			Vertex temp;
			temp.Position.x = translationPoints.at(i).Position.x  + spacebtwn * j;
			temp.Position.y = translationPoints.at(i).Position.y;
			
			_vertices.push_back(temp);

			//add horizontal line index
			if (j == 0 || j == sizeX - 1) 
			{
				_indices.push_back((i*sizeX) + j);
			}
			else
			{
				_indices.push_back((i*sizeX) + j);
				_indices.push_back((i*sizeX) + j);
			}

		}
	}
	for (int i = 0; i < sizeY; ++i)
	{
		for (int j = 0; j < sizeX; ++j)
		{
			//add vertical line index
			if (i != sizeY - 1)
			{
				_indices.push_back((i*sizeX) + j);
				_indices.push_back((i*sizeX) + j + sizeX);
			}
		}
	}

	initializeModelMatrix();
	ConstructRenderData();
}

void Grid::ConstructRenderData()
{
	// Generate buffers
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Bind VAO
	glBindVertexArray(VAO);

	// Copy vertex data to VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vertex), &_vertices[0], GL_STATIC_DRAW);

	// Copy indices to EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(GLuint), &_indices[0], GL_STATIC_DRAW);

	// Configure vertex position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);

	// Configure vertex normals attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));

	// Configure texture coordinates attribute
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));

	// Unbind all buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Grid::initializeModelMatrix()
{
	_modelMatrix = mat4(1.0f);

	auto angleInRadians = glm::radians(90.0f);
	_modelMatrix = glm::rotate(_modelMatrix, angleInRadians, glm::vec3(1.0f, 0.0f, 0.0f));
}

mat4& Grid::GetModelMatrix()
{
	return _modelMatrix;
}
