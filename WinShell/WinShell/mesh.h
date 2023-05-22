#pragma once
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <glad.h>
#include <glfw3.h>
#include "shader.h"

using namespace std;
using namespace glm;

enum TextureType
{
	TEXTURE_DIFFUSE,
	TEXTURE_SPECULAR
};

struct Vertex
{
	vec3 position;
	vec3 normal;
	vec2 texCoord;
};

struct Texture
{
	unsigned int id;
	string path;
	TextureType type;
};

class Mesh
{
public:
	unsigned int VAO, VBO, EBO;
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;
		setupMesh();
	}
	void draw(const Shader& shader, unsigned int instanceCount)
	{
		int diffuseIndex = 0;
		int specularIndex = 0;
		for (size_t i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);

			stringstream name;
			switch (textures[i].type)
			{
			case TEXTURE_DIFFUSE:
			{
				name << "diffuseTexture[" << diffuseIndex++ << "]";
				break;
			}
			case TEXTURE_SPECULAR:
			{
				name << "specularTexture[" << specularIndex++ << "]";
				break;
			}
			}
			shader.setInt("material." + name.str(), i);
		}

		glBindVertexArray(VAO);
		if (instanceCount == 0)
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		else
			glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, instanceCount);
		glBindVertexArray(0);
	}
private:
	void setupMesh()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, position)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, normal)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, texCoord)));
	
		glBindVertexArray(0);
	}
};