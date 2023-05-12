#pragma once
#include <Importer.hpp>
#include <scene.h>
#include <postprocess.h>
#include "shader.h"
#include "mesh.h"
#include <vector>
#include <map>
#include <string>
#include <glm/glm.hpp>

using namespace std;
using namespace Assimp;

class Model
{
public:
	Model(string path)
	{
		Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
			return;
		}
		
		directory = path.substr(0, path.find_last_of("/"));
		processNode(scene->mRootNode, scene);
	}
	void draw(const Shader& shader)
	{
		for (size_t i = 0; i < meshes.size(); i++)
		{
			meshes[i].draw(shader);
		}
	}
private:
	map<string, Texture> cachedTextureMap;
	vector<Mesh> meshes;
	string directory;
	void processNode(const aiNode* aNode, const aiScene* scene)
	{
		for (size_t i = 0; i < aNode->mNumMeshes; i++)
		{
			//所有aiMesh实例存在scene实例中，node里的mesh只是指向aiMesh实例的索引
			size_t aMeshIndex = aNode->mMeshes[i];
			aiMesh* aMesh = scene->mMeshes[aMeshIndex];
			Mesh mesh = processMesh(aMesh, scene);
			meshes.push_back(mesh);
		}

		for (size_t i = 0; i < aNode->mNumChildren; i++)
		{
			processNode(aNode->mChildren[i], scene);
		}
	}
	Mesh processMesh(const aiMesh* aMesh, const aiScene* scene)
	{
		vector<Vertex> vertices;
		vector<unsigned int> indices;
		vector<Texture> textures;

		//1.顶点
		for (size_t i = 0; i < aMesh->mNumVertices; i++)
		{
			Vertex vertex;
			if (aMesh->HasPositions())
				vertex.position = vec3(aMesh->mVertices[i].x, aMesh->mVertices[i].y, aMesh->mVertices[i].z);
			else
				vertex.position = vec3();
			if (aMesh->HasNormals())
				vertex.normal = vec3(aMesh->mNormals[i].x, aMesh->mNormals[i].y, aMesh->mNormals[i].z);
			else
				vertex.normal = vec3();
			if (aMesh->HasTextureCoords(0))
				vertex.texCoord = vec2(aMesh->mTextureCoords[0][i].x, aMesh->mTextureCoords[0][i].y);
			else
				vertex.texCoord = vec2();
			vertices.push_back(vertex);
		}

		//2.顶点索引
		if (aMesh->HasFaces())
		{
			for (size_t i = 0; i < aMesh->mNumFaces; i++)
			{
				aiFace aFace = aMesh->mFaces[i];
				for (size_t j = 0; j < aFace.mNumIndices; j++)
				{
					indices.push_back(aFace.mIndices[j]);
				}
			}
		}

		//3.材质
		if (aMesh->mMaterialIndex >= 0)
		{
			int aMaterialIndex = aMesh->mMaterialIndex;
			aiMaterial* aMaterial = scene->mMaterials[aMaterialIndex];
			vector<Texture> diffuseTextures = loadMaterialTexture(aMaterial, aiTextureType_DIFFUSE);
			textures.insert(textures.end(), diffuseTextures.begin(), diffuseTextures.end());
			vector<Texture> specularTextures = loadMaterialTexture(aMaterial, aiTextureType_SPECULAR);
			textures.insert(textures.end(), specularTextures.begin(), specularTextures.end());
		}

		return Mesh(vertices, indices, textures);
	}
	vector<Texture> loadMaterialTexture(const aiMaterial* aMaterial, aiTextureType aTextureType)
	{
		size_t textureCount = aMaterial->GetTextureCount(aTextureType);
		vector<Texture> textures;
		for (size_t i = 0; i < textureCount; i++)
		{
			aiString aFileName;
			aMaterial->GetTexture(aTextureType, i, &aFileName);
			string fileName = aFileName.C_Str();
			Texture texture;
			if (cachedTextureMap.find(fileName) != cachedTextureMap.end())
			{
				texture = cachedTextureMap[fileName];
			}
			else
			{
				texture.id = textureFromPath(directory + '/' + fileName);
				texture.path = fileName;
				switch (aTextureType)
				{
				case aiTextureType_DIFFUSE:
					texture.type = TEXTURE_DIFFUSE;
					break;
				case aiTextureType_SPECULAR:
					texture.type = TEXTURE_SPECULAR;
					break;
				}
				cachedTextureMap.insert(pair<string, Texture>(fileName, texture));
			}
			
			textures.push_back(texture);
		}
		return textures;
	}
	unsigned int textureFromPath(string path)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);
		int width, height, channel;
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &channel, 0);
		if (data)
		{
			GLenum format;
			if (channel == 1)
				format = GL_RED;
			else if (channel == 3)
				format = GL_RGB;
			else if (channel == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << path << std::endl;
		}
		stbi_image_free(data);

		return textureID;
	}
};

