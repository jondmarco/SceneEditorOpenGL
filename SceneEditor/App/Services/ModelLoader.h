#pragma once

#include "Data\Core\Texture.h"

#include "GL\glew.h"
#include "assimp\postprocess.h"
#include "glm\glm.hpp"

#include <string>
#include <vector>
#include <exception>

class Model;
class Mesh;
struct Vertex;
struct Material;

struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMaterial;
enum aiTextureType;

namespace Assimp
{
	class Importer;
}

using std::string;
using std::vector;
using std::exception;
using Assimp::Importer;
using glm::vec3;


class ModelLoader
{
public:

	Model* LoadMesh(string filePath);

private:

	const aiScene* LoadAssimpScene(Importer* importer, string filePath);
	
	void ProcessAssimpNode(aiNode* node, const aiScene* scene, string fileDirectory, Model* resultModel);
	Mesh ProcessAssimpMesh(aiMesh* mesh, const aiScene* scene, string fileDirectory, Model* resultModel);

	vector<Vertex> ProcessVertices(aiMesh* mesh, Model* resultModel);
	vector<GLuint> ProcessIndices(aiMesh* mesh);
	Material ProcessMaterial(aiMesh* mesh, const aiScene* scene, string fileDirectory);

	void UpdateMinMax(vec3& vertexPosition, Model* resultModel);

	vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, string fileDirectory);
	GLuint LoadTextureFromFile(string relativePath, string rootDirectory);

	vector<Texture> _loadedTextures;

	unsigned const int _defaultImportOptions = aiProcess_Triangulate | aiProcess_FlipUVs ;

	const string LOG_CHANNEL = "ModelLoader";
};


struct MeshLoaderException : public exception
{
public:

	MeshLoaderException(string message) : _message(message) {};

	const char * what() const throw ()
	{
		return _message.c_str();
	}

private:

	string _message;
};
