#include <glad/glad.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "types.h"
#include "graphics.h"


namespace md
{
namespace engine
{
	namespace graphics
	{
#define NUM_BONES_PER_VERTEX 4

		const std::string diffuseName	= "texture_diffuse";
		const std::string specularName	= "texture_specular";
		const std::string normalName	= "texture_normal";
		const std::string heightName	= "texture_height";

		struct VertexBoneData
		{
			u32 IDs[NUM_BONES_PER_VERTEX];
			f32 weights[NUM_BONES_PER_VERTEX];

			VertexBoneData()
			{
				Reset();
			}

			void Reset()
			{
				memset(IDs, 0, sizeof(IDs));
				memset(weights, 0, sizeof(weights));
			}

			void AddBoneData(u32 boneID, f32 weight)
			{
				for (u32 i = 0; i < sizeof(IDs) / sizeof(IDs[0]); i++) {
					if (weights[i] == 0.0) {
						IDs[i] = boneID;
						weights[i] = weight;
						return;
					}
				};
				//assert(0);
			}
		};

		struct Vertex
		{
			glm::vec3 position;
			glm::vec3 normal;
			glm::vec2 texCoords;
			glm::vec3 tangent;
			glm::vec3 bitangent;
		};

		struct BoneInfo
		{
			aiMatrix4x4 boneOffset;
			aiMatrix4x4 finalTransform;
			std::string name;
			u32 index;

			BoneInfo() : boneOffset(aiMatrix4x4()), finalTransform(aiMatrix4x4()) { }
		};


		struct Texture
		{
			u32 id;
			std::string type;
			std::string path;
		};

		class Mesh
		{
		public:
			Mesh(std::vector<Vertex> v, std::vector<u32> i, std::vector<Texture> t, std::vector<VertexBoneData> b = std::vector<VertexBoneData>()) :
				vertices(v), indices(i), textures(t), bones(b)
			{
				setupMesh();
			}

			void Draw(Shader *shader)
			{
				u32 diffuseNr	= 1;
				u32 specularNr	= 1;
				u32 normalNr	= 1;
				u32 heightNr	= 1;
				for (u32 i = 0; i < textures.size(); i++)
				{
					glActiveTexture(GL_TEXTURE0 + i);
					std::string num;
					std::string name = textures[i].type;
					if(name == diffuseName)
						num = std::to_string(diffuseNr++);
					else if(name == specularName)
						num = std::to_string(specularNr++);
					else if(name == normalName)
						num = std::to_string(normalNr++);
					else if(name == heightName)
						num = std::to_string(heightNr++);
						
					shader->setInt(name + num, i);
					glBindTexture(GL_TEXTURE_2D, textures[i].id);
				}

				glBindVertexArray(VAO);
				glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);

				glActiveTexture(GL_TEXTURE0);
			}

			std::vector<Vertex> vertices;
			std::vector<VertexBoneData> bones;
			std::vector<u32> indices;
			std::vector<Texture> textures;
			GLuint VAO;


		private:
			GLuint VBO, EBO, VBO_bones;

			void setupMesh()
			{
				glGenBuffers(1, &EBO);
				glGenBuffers(1, &VBO);
				glGenBuffers(1, &VBO_bones);
				glGenVertexArrays(1, &VAO);

				glBindVertexArray(VAO);
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * indices.size(), indices.data(), GL_STATIC_DRAW);

				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
				glEnableVertexAttribArray(2);
				glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
				glEnableVertexAttribArray(3);
				glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
				glEnableVertexAttribArray(4);

				//std::cout << "************  " << bones.size() << "  ************" << std::endl;

				/*std::fstream file("data2.txt", std::fstream::out | std::fstream::app);
				if (file.is_open())
				{
					for (auto & bone : bones)
					{
						for (u32 i = 0; i < NUM_BONES_PER_VERTEX; i++)
						{
							file << bone.IDs[i] << " " << bone.weights[i] << std::endl;
							//std::cout << bone.IDs[i] << " : " << bone.weights[i] << std::endl;
						}
						//std::cout << std::endl;
					}
				}
				file.close();*/

				glBindBuffer(GL_ARRAY_BUFFER, VBO_bones);
				glBufferData(GL_ARRAY_BUFFER, sizeof(VertexBoneData) * bones.size(), bones.data(), GL_STATIC_DRAW);
				glVertexAttribIPointer(5, 4, GL_INT, sizeof(VertexBoneData), (void*)0);
				glEnableVertexAttribArray(5);
				glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (void*)(4 * sizeof(u32)));
				glEnableVertexAttribArray(6);


				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindVertexArray(0);
			}



		};


	}
}
}