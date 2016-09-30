#pragma once
#include "GameStorage.h"
#include <fstream>
#include <iostream>
#include <sstream>

#include "Game/SceneEntity.h"
#include "Data/Model.h"
#include "Game/Light.h"

#include "glm\gtc\matrix_transform.hpp"
#include "glm\glm.hpp"
#include "glm\gtc\type_ptr.hpp"

using namespace std;

Scene* GameStorage::load()
{
	Scene* scene = new Scene();
	
	std::ifstream infile(_inputManager.SelectFile());

	if (infile.is_open())
	{
		string line;
		getline(infile, line);
		
		while (line != "")
		{
			while (line.substr(0, 5) == "path:")
			{
				double arrTrans[16];
				glm::vec3 color;
				float roughness;
				float metalness;
				string filePath;
				stringstream ss(line.substr(5));
				
				ss >> filePath;
				getline(infile, line);
					
				
				if (line.substr(0, 6) == "trans:")
				{
					stringstream ss(line.substr(6));

					for (int i = 0; i < 16; ++i)
					{
						ss >> arrTrans[i];
					}
				}
				getline(infile, line);
				
				if (line.substr(0, 6) == "color:")
				{
					stringstream ss(line.substr(6));
					ss >> color.x;
					ss >> color.y;
					ss >> color.z;
				}
				getline(infile, line);

				if (line.substr(0, 10) == "roughness:")
				{
					stringstream ss(line.substr(10));
					ss >> roughness;
				}
				getline(infile, line);

				if (line.substr(0, 6) == "metal:")
				{
					stringstream ss(line.substr(6));
					ss >> metalness;
				}

				auto model = _modelLoader.LoadMesh(filePath);
				auto physicsComponent = _physicsSystem->CreatePhysicsComponent(model);
				SceneEntity* entity = new SceneEntity(model, physicsComponent);

				entity->SetTransformationMatrix(glm::make_mat4(arrTrans));
				entity->setBaseColor(color.x, color.y, color.z);
				entity->setMetallness(metalness);
				entity->setRoughness(roughness);
				scene->AddEntity(entity);

				getline(infile, line);
			}

			while (line.substr(0, 6) == "light:")
			{
				vec3 lightpos;
				vec3 lightcolor;
				getline(infile, line);

				if (line.substr(0, 6) == "color:")
				{
					stringstream ss(line.substr(6));
					ss >> lightcolor.x;
					ss >> lightcolor.y;
					ss >> lightcolor.z;
				}
				getline(infile, line);

				if (line.substr(0, 9) == "position:")
				{
					stringstream ss(line.substr(9));
					ss >> lightpos.x;
					ss >> lightpos.y;
					ss >> lightpos.z;
				}

				Light* light = new Light(lightpos, lightcolor);
				scene->AddLight(light);

				getline(infile, line);
			}

			getline(infile, line);
		}
	}

	infile.close();
	
	return scene;
}

void GameStorage::save(Scene* scene)
{
	std::ofstream outfile(_inputManager.SaveFile());

	for (SceneEntity* elem : scene->GetEntities())
	{
		//dont save the light models
		if (elem->isLight())
		{
			continue;
		}

		//store the path of the model
		outfile << "path: " << elem->GetModel()->modelFilePath << std::endl;

		//store the transformation matrix
		mat4 transMatrix = elem->_transformationMatrix;
		double arrTrans[16];

		const float* pSource = (const float*)&(transMatrix);
		for (int i = 0; i < 16; ++i)
			arrTrans[i] = pSource[i];

		outfile << "trans: ";

		for (int i = 0; i < 16; ++i)
		{
			outfile << arrTrans[i] << " ";
		}

		outfile << std::endl;

		//store the base color
		outfile << "color: " << elem->GetBaseColor().x << " " << elem->GetBaseColor().y << " " << elem->GetBaseColor().z << std::endl;

		//store the roughness
		outfile << "roughness: " << elem->getRoughness() << std::endl;

		//store the metallic properties
		outfile << "metal: " << elem->getMetallness() << std::endl;

	}

	for (Light* light : scene->GetLightSources())
	{
		outfile << "light: " << std::endl;

		outfile << "color: " << light->getColor().x << " " << light->getColor().y << " " << light->getColor().z << std::endl;
		outfile << "position: " << light->getPosition().x << " " << light->getPosition().y << " " << light->getPosition().z << std::endl;
	}
	outfile.close();
}