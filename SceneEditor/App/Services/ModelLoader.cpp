#include "ModelLoader.h"

#include "Data\Model.h"
#include "Data\Material.h"
#include "Data\Core\Vertex.h"

#include "Services\Logger.h"

#include "assimp\Importer.hpp"
#include "assimp\scene.h"

#include "SOIL\soil.h"



#include <algorithm>


Model* ModelLoader::LoadMesh(string filePath)
{
	//Logger::LogInfo(LOG_CHANNEL, "Starting loading model " + filePath);

	Model* resultModel = new Model();

	// Load scene from file using Assimp
	Importer* importer = new Importer();
	auto scene = LoadAssimpScene(importer, filePath);

	// TODO: Report failure during scene loading
	if (scene == nullptr)
	{
		throw MeshLoaderException("Failed to read object file " + filePath);
	}
	

	// Retrieve the directory path for constructing relative paths to referenced resources
	string fileDirectory = filePath.substr(0, filePath.find_last_of('\\'));

	// Process scene nodes starting with root node
	ProcessAssimpNode(scene->mRootNode, scene, fileDirectory, resultModel);

	delete(importer);

	Logger::LogInfo(LOG_CHANNEL, "Model loaded");

	resultModel->modelFilePath = filePath;
	return resultModel;
}

const aiScene* ModelLoader::LoadAssimpScene(Importer* importer, string filePath)
{
	// Read scene from file
	const aiScene* scene = importer->ReadFile(filePath, _defaultImportOptions);

	// Check for errors
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		Logger::LogError(LOG_CHANNEL, "Assimp failure: " + string(importer->GetErrorString()));
		return nullptr;
	}

	return scene;
}

// Processes scene nodes recursively
void ModelLoader::ProcessAssimpNode(aiNode* node, const aiScene* scene, string fileDirectory, Model* resultModel)
{
	// Process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* aiSubMesh = scene->mMeshes[node->mMeshes[i]];
		Mesh submesh = ProcessAssimpMesh(aiSubMesh, scene, fileDirectory, resultModel);

		resultModel->_subMeshes.push_back(submesh);
	}

	// Process child nodes
	for (GLuint i = 0; i < node->mNumChildren; i++)
	{
		ProcessAssimpNode(node->mChildren[i], scene, fileDirectory, resultModel);
	}
}

Mesh ModelLoader::ProcessAssimpMesh(aiMesh* mesh, const aiScene* scene, string fileDirectory, Model* resultModel)
{
	vector<Vertex> vertices = ProcessVertices(mesh, resultModel);
	vector<GLuint> indices = ProcessIndices(mesh);
	Material material = ProcessMaterial(mesh, scene, fileDirectory);

	return Mesh(vertices, indices, material);
}

vector<Vertex> ModelLoader::ProcessVertices(aiMesh* mesh, Model* resultModel)
{
	vector<Vertex> vertices;
    
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		
		// Process vertex position
		vertex.Position.x = mesh->mVertices[i].x;
		vertex.Position.y = mesh->mVertices[i].y;
		vertex.Position.z = mesh->mVertices[i].z;

		UpdateMinMax(vertex.Position, resultModel);
	
		// Process vertex normals
		if (mesh->HasNormals())
		{
			vertex.Normal.x = mesh->mNormals[i].x;
			vertex.Normal.y = mesh->mNormals[i].y;
			vertex.Normal.z = mesh->mNormals[i].z;
		}

		// Process texture coordinates
		// Note: a vertex can have up to 8 UV channels. Currently, we only process the first one. 
		if (mesh->HasTextureCoords(0))
		{
			vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
			vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
		}

		vertices.push_back(vertex);
	}

	return vertices;
}

vector<GLuint> ModelLoader::ProcessIndices(aiMesh* mesh)
{
	vector<GLuint> indices;

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	return indices;
}

Material ModelLoader::ProcessMaterial(aiMesh* mesh, const aiScene* scene, string fileDirectory)
{
	Material material;

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* aiMaterial = scene->mMaterials[mesh->mMaterialIndex];

		// Load Diffuse maps
		material.DiffuseMaps = LoadMaterialTextures(aiMaterial, aiTextureType_DIFFUSE, fileDirectory);

		// Load Specular maps
		material.SpecularMaps = LoadMaterialTextures(aiMaterial, aiTextureType_SPECULAR, fileDirectory);
	}

	return material;
}

vector<Texture> ModelLoader::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, string fileDirectory)
{
	vector<Texture> textures;

	for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString texturePath;
		mat->GetTexture(type, i, &texturePath);

		// Skip texture if it was loaded before
		auto compareTexturePathLambda = [&texturePath](const Texture& t) {return t.path == texturePath.C_Str(); };
		auto resultIterator = std::find_if(_loadedTextures.begin(), _loadedTextures.end(), compareTexturePathLambda);
		if (resultIterator != _loadedTextures.end())
		{
			textures.push_back(*resultIterator);
			continue;
		}

		Texture texture;
		texture.id = LoadTextureFromFile(string(texturePath.C_Str()), fileDirectory);
		texture.path = texturePath.C_Str();

		textures.push_back(texture);
		_loadedTextures.push_back(texture); 
	}

	return textures;
}

GLuint ModelLoader::LoadTextureFromFile(string relativePath , string rootDirectory)
{
	string filename = rootDirectory + '\\' + relativePath;

	// Load texture in OpenGL memory and return its id
	GLuint textureID;
	glGenTextures(1, &textureID);

	// Load raw image data
	int width, height;
	unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);

	// Copy texture data into OpenGL data structure
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

	glGenerateMipmap(GL_TEXTURE_2D);

	// Set texture parameters

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	// Free raw image data
	SOIL_free_image_data(image);

	return textureID;
}

void ModelLoader::UpdateMinMax(vec3& vertexPosition, Model* resultModel)
{
	auto& maxPoint = resultModel->_maxPoint;
	auto& minPoint = resultModel->_minPoint;

	if (vertexPosition.x > maxPoint.x) maxPoint.x = vertexPosition.x;
	if (vertexPosition.x < minPoint.x) minPoint.x = vertexPosition.x;

	if (vertexPosition.y > maxPoint.y) maxPoint.y = vertexPosition.y;
	if (vertexPosition.y < minPoint.y) minPoint.y = vertexPosition.y;

	if (vertexPosition.z > maxPoint.z) maxPoint.z = vertexPosition.z;
	if (vertexPosition.z < minPoint.z) minPoint.z = vertexPosition.z;
}
