#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <fstream>
#include <iostream>
#include <map>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/quaternion.hpp>
#include <gtx/matrix_interpolation.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "graphics.h"
#include "mesh.h"
#include "types.h"

#include "md_load_texture.h"

namespace md
{
namespace engine
{
	namespace graphics
	{
		typedef std::vector<Texture> Textures;

		class Model
		{
		public:
			Model() { }

			Model(std::string const &path, bool gamma = false) :
				m_BoneCount(0), m_VerticesCount(0), m_hasBones(false), m_Path(path)
			{
				loadModel(path);
			}
			
			virtual ~Model() { }

			void DrawModel(Shader *shader)
			{
				for (auto & i : meshes)
					i.Draw(shader);
			}

			Textures texturesLoaded;
			std::vector<Mesh> meshes;
			std::vector<BoneInfo> bonesInfo;

			std::string dir;
			b8 gammaCorrection;

		protected:
			b8 m_hasBones;
			std::string m_Path;

			u32 FindPosition(f32 animTime, const aiNodeAnim *nodeAnim)
			{
				for (u32 i = 0; i < nodeAnim->mNumPositionKeys - 1; i++)
				{
					if (animTime < (f32)nodeAnim->mPositionKeys[i + 1].mTime)
						return i;
				}

				assert(0);

				return 0;
			}

			u32 FindRotation(f32 animTime, const aiNodeAnim *nodeAnim)
			{
				assert(nodeAnim->mNumRotationKeys > 0);
				for (u32 i = 0; i < nodeAnim->mNumRotationKeys - 1; i++)
				{
					if (animTime < (f32)nodeAnim->mRotationKeys[i + 1].mTime)
						return i;
				}

				assert(0);

				return 0;
			}

			u32 FindScaling(f32 animTime, const aiNodeAnim *nodeAnim)
			{
				assert(nodeAnim->mNumScalingKeys > 0);

				for (u32 i = 0; i < nodeAnim->mNumScalingKeys - 1; i++)
				{
					if (animTime < (f32)nodeAnim->mScalingKeys[i + 1].mTime)
						return i;
				}

				assert(0);

				return 0;
			}

			void CalcInterpolatedPosition(glm::vec3 &out, f32 animTime, const aiNodeAnim *nodeAnim)
			{
				if (nodeAnim->mNumPositionKeys == 1)
				{
					out = reinterpret_cast<glm::vec3&>(nodeAnim->mPositionKeys[0].mValue);
					return;
				}

				u32 posIndex = FindPosition(animTime, nodeAnim);
				u32 nextPosIndex = posIndex + 1;
				assert(nextPosIndex < nodeAnim->mNumPositionKeys);
				f32 deltaTime = (f32)(nodeAnim->mPositionKeys[nextPosIndex].mTime - nodeAnim->mPositionKeys[posIndex].mTime);
				f32 factor = (animTime - (f32)nodeAnim->mPositionKeys[posIndex].mTime) / deltaTime;
				assert(factor >= 0.f && factor <= 1.f);
				const glm::vec3 &start = reinterpret_cast<glm::vec3&>(nodeAnim->mPositionKeys[posIndex].mValue);
				const glm::vec3 &end = reinterpret_cast<glm::vec3&>(nodeAnim->mPositionKeys[nextPosIndex].mValue);
				glm::vec3 delta = end - start;

				out = start + (factor * delta);
			}

			void CalcInterploatedRotation(glm::quat &out, f32 animTime, const aiNodeAnim *nodeAnim)
			{
				if (nodeAnim->mNumRotationKeys == 1)
				{
					out = reinterpret_cast<glm::quat&>(nodeAnim->mRotationKeys[0].mValue);
					out.convertToGLM();
					return;
				}

				u32 rotationIndex = FindRotation(animTime, nodeAnim);
				u32 nextRotationIndex = rotationIndex + 1;
				assert(nextRotationIndex < nodeAnim->mNumRotationKeys);
				f32 deltaTime = (f32)(nodeAnim->mRotationKeys[nextRotationIndex].mTime - nodeAnim->mRotationKeys[rotationIndex].mTime);
				f32 factor = (animTime - (f32)nodeAnim->mRotationKeys[rotationIndex].mTime) / deltaTime;
				assert(factor >= 0.f && factor <= 1.f);
				glm::quat &startRotationQ(reinterpret_cast<glm::quat&>(nodeAnim->mRotationKeys[rotationIndex].mValue));
				glm::quat &endRotationQ(reinterpret_cast<glm::quat&>(nodeAnim->mRotationKeys[nextRotationIndex].mValue));
				//startRotationQ.convertToGLM();
				//endRotationQ.convertToGLM();

				out = glm::slerp(startRotationQ, endRotationQ, factor);
				out = glm::normalize(out);
				//out.convertToGLM();
			}

			void CalcInterpolatedScaling(glm::vec3 &out, f32 animTime, const aiNodeAnim *nodeAnim)
			{
				if (nodeAnim->mNumScalingKeys == 1)
				{
					out = reinterpret_cast<glm::vec3&>(nodeAnim->mScalingKeys[0].mValue);
					return;
				}

				u32 scalingIndex = FindScaling(animTime, nodeAnim);
				u32 nextScalingIndex = scalingIndex + 1;
				assert(nextScalingIndex < nodeAnim->mNumScalingKeys);
				f32 deltaTime = (f32)(nodeAnim->mScalingKeys[nextScalingIndex].mTime - nodeAnim->mScalingKeys[scalingIndex].mTime);
				f32 factor = (animTime - (f32)nodeAnim->mScalingKeys[scalingIndex].mTime) / deltaTime;
				assert(factor >= 0.f && factor <= 1.f);
				const glm::vec3 &start = reinterpret_cast<glm::vec3&>(nodeAnim->mScalingKeys[scalingIndex].mValue);
				const glm::vec3 &end = reinterpret_cast<glm::vec3&>(nodeAnim->mScalingKeys[nextScalingIndex].mValue);
				glm::vec3 delta = end - start;

				out = start + (factor * delta);
			}

			void ReadNodeHierarchy(f32 animTime, const aiNode *node, const glm::mat4 &parentTransform)
			{
				std::string nodeName(node->mName.data);

				const aiAnimation *pAnim = m_Scene->mAnimations[0];
				glm::mat4 nodeTransformation = (glm::mat4&)node->mTransformation;

				const aiNodeAnim *nodeAnim = FindNodeAnim(pAnim, nodeName);

				if ((int)animTime % 23 == 0 && (int)animTime / 23 == 1 && nodeName == "forearm.L")
					std::cout << "test";

				if (nodeAnim)
				{
					glm::vec3 scaling;
					CalcInterpolatedScaling(scaling, animTime, nodeAnim);
					glm::mat4 scalingM;
					scalingM = glm::initScale(scalingM, scaling);

					glm::quat rotationQ;
					CalcInterploatedRotation(rotationQ, animTime, nodeAnim);
					glm::quat rotQ = reinterpret_cast<glm::quat&>(rotationQ);
					rotQ.convertToGLM();
					glm::mat4 rotationM = glm::mat4(glm::ai_mat3_cast(rotQ));

					
					glm::vec3 translation;
					CalcInterpolatedPosition(translation, animTime, nodeAnim);
					glm::mat4 translationM;
					translationM = glm::initTranslation(translationM, translation);

					nodeTransformation = translationM * rotationM * scalingM;
					//nodeTransformation = rotationM * scalingM;
				}

				
				glm::mat4 globalTransformation = parentTransform * nodeTransformation;

				auto it = std::find_if(bonesInfo.begin(), bonesInfo.end(), [&]
				(const BoneInfo & ref) { return ref.name == nodeName; });

				if (it != bonesInfo.end())
				{
					BoneInfo *bone = &bonesInfo.at(std::distance(bonesInfo.begin(), it));
					u32 boneIndex = bone->index;
					glm::mat4 globalInverseTransform = reinterpret_cast<glm::mat4&>(m_Scene->mRootNode->mTransformation);
					globalInverseTransform = glm::inverse(globalInverseTransform);
					bone->finalTransform = globalInverseTransform * globalTransformation * bone->boneOffset;
					//std::cout << nodeName << std::endl;
				}

				for (u32 i = 0; i < node->mNumChildren; i++)
					ReadNodeHierarchy(animTime, node->mChildren[i], globalTransformation);

			}

			void BoneTransform(f32 timeInSeconds, std::vector<glm::mat4> &transforms)
			{
				glm::mat4 identity = glm::mat4();

				Assimp::Importer importer;
				m_Scene = importer.ReadFile(m_Path, aiProcess_Triangulate
					| aiProcess_GenSmoothNormals
					| aiProcess_FlipUVs
					| aiProcess_JoinIdenticalVertices);


				if (!m_Scene || m_Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_Scene->mRootNode)
				{
					std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
					return;
				}

				f32 ticksPerSec = (f32)(m_Scene->mAnimations[0]->mTicksPerSecond != 0 ? m_Scene->mAnimations[0]->mTicksPerSecond : 25.f);
				f32 timeInTicks = timeInSeconds * ticksPerSec;
				f32 animTime = fmod(timeInTicks, (f32)m_Scene->mAnimations[0]->mDuration);

				ReadNodeHierarchy(animTime, m_Scene->mRootNode, identity);

				transforms.resize(bonesInfo.size());

				u32 startIndex = 0;

				for (u32 i = 0; i < bonesInfo.size(); i++)
				{
					transforms[i] = bonesInfo[i].finalTransform;
					//transforms[i] = glm::mat4();
				}
			}


			const aiNodeAnim *FindNodeAnim(const aiAnimation *anim, const std::string &nodeName)
			{
				for (u32 i = 0; i < anim->mNumChannels; i++)
				{
					const aiNodeAnim *nodeAnim = anim->mChannels[i];

					if (std::string(nodeAnim->mNodeName.data) == nodeName)
						return nodeAnim;
				}

				return nullptr;
			}

		private:

			u32 m_BoneCount;
			u32 m_VerticesCount;
			const aiScene *m_Scene;

			void loadModel(std::string const &path)
			{
				Assimp::Importer importer;
				m_Scene = importer.ReadFile(path, aiProcess_Triangulate 
															  | aiProcess_GenSmoothNormals
															  | aiProcess_FlipUVs
															  | aiProcess_JoinIdenticalVertices);

				
				if (!m_Scene || m_Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_Scene->mRootNode)
				{
					std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
					return;
				}
				dir = path.substr(0, path.find_last_of('/'));
				
				processNode(m_Scene->mRootNode, m_Scene);
			}

			void processNode(aiNode *node, const aiScene *scene)
			{
				for (u32 i = 0; i < node->mNumMeshes; i++)
				{
					aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
					meshes.push_back(processMesh(mesh, scene));
					//processBones(mesh, baseVertex);
					m_VerticesCount += mesh->mNumVertices;
				}

				for (u32 i = 0; i < node->mNumChildren; i++)
					processNode(node->mChildren[i], scene);
			}

			void processBones(aiMesh *mesh, std::vector<VertexBoneData> *bones)
			{
				for (u32 i = 0; i < mesh->mNumBones; i++)
				{
					u32 boneIndex = 0;
					std::string boneName = mesh->mBones[i]->mName.data;

					auto it = std::find_if(bonesInfo.begin(), bonesInfo.end(), [&]
					(const BoneInfo & ref) { return ref.name == boneName; });
					if (it == bonesInfo.end())
					{
						boneIndex = m_BoneCount;
						m_BoneCount++;
						BoneInfo bi;
						bi.boneOffset = reinterpret_cast<glm::mat4&>(mesh->mBones[i]->mOffsetMatrix);
						bi.name = boneName;
						bi.index = boneIndex;


						/*std::fstream file("bones1.txt", std::fstream::out | std::fstream::app);
						if (file.is_open())
						{
							file << "Bone Name: " << bi.name << "\n";
							file << "Bone Index: " << bi.index << "\n";
							file << "Bone Matrix:\n";
							for (u32 i = 0; i < 4; i++)
							{
								for (u32 j = 0; j < 4; j++)
								{
									file << bi.boneOffset[i][j] << " ";
								}
								file << "\n";
							}
							file << "\n\n\n";
						}
						file.close();*/

						bonesInfo.push_back(bi);
						//mBonesInfo->push_back(bi);
					}
					else
					{
						boneIndex = bonesInfo.at(std::distance(bonesInfo.begin(), it)).index;
					}
					//std::cout << mesh->mBones[i]->mName.data << std::endl;

					for (u32 j = 0; j < mesh->mBones[i]->mNumWeights; j++)
					{
						u32 vertexID = mesh->mBones[i]->mWeights[j].mVertexId;
						f32 weight = mesh->mBones[i]->mWeights[j].mWeight;
						bones->at(vertexID).AddBoneData(boneIndex, weight);

					}
					std::cout << std::endl;
				}


			}

			Mesh processMesh(aiMesh *mesh, const aiScene *scene)
			{
				std::vector<Vertex> vertices;
				std::vector<VertexBoneData> bones;
				std::vector<BoneInfo> mBonesInfo;;
				std::vector<u32> indices;
				Textures textures;

				glm::quat q;

				glm::mat4 m = glm::toMat4(q);

				m = glm::mat4();

				for (u32 i = 0; i < mesh->mNumVertices; i++)
				{
					Vertex vertex;
					glm::vec3 vec;

					vec.x = mesh->mVertices[i].x;
					vec.y = mesh->mVertices[i].y;
					vec.z = mesh->mVertices[i].z;
					vertex.position = vec;

					if (mesh->mNormals != NULL)
					{
						vec.x = mesh->mNormals[i].x;
						vec.y = mesh->mNormals[i].y;
						vec.z = mesh->mNormals[i].z;
						vertex.normal = vec;
					}

					if (mesh->mTextureCoords[0])
					{
						glm::vec2 vec;
						vec.x = mesh->mTextureCoords[0][i].x;
						vec.y = mesh->mTextureCoords[0][i].y;
						vertex.texCoords = vec;
					}
					else
						vertex.texCoords = glm::vec2(0.f);

					if (mesh->mTangents != NULL)
					{
						vec.x = mesh->mTangents[i].x;
						vec.y = mesh->mTangents[i].y;
						vec.z = mesh->mTangents[i].z;
						vertex.tangent = vec;
					}

					if (mesh->mBitangents != NULL)
					{
						vec.x = mesh->mBitangents[i].x;
						vec.y = mesh->mBitangents[i].y;
						vec.z = mesh->mBitangents[i].z;
						vertex.bitangent = vec;
					}
					vertices.push_back(vertex);
				}

				if (mesh->mBones != NULL)
				{
					m_hasBones = true;
					bones.resize(mesh->mNumVertices);
					processBones(mesh, &bones);
				}

				for (u32 i = 0; i < mesh->mNumFaces; i++)
				{
					aiFace face = mesh->mFaces[i];
					for (u32 j = 0; j < face.mNumIndices; j++)
						indices.push_back(face.mIndices[j]);
				}

				aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

				Textures diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, diffuseName);
				textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

				Textures specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, specularName);
				textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

				Textures normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, normalName);
				textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

				Textures heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, heightName);
				textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
				
				return Mesh(vertices, indices, textures, bones);
			}

			std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
			{
				std::vector<Texture> textures;
				for (u32 i = 0; i < mat->GetTextureCount(type); i++)
				{
					aiString str;
					mat->GetTexture(type, i, &str);
					b8 skip = false;
					for (auto & j : texturesLoaded)
					{
						if (std::strcmp(j.path.data(), str.C_Str()) == 0)
						{
							textures.push_back(j);
							skip = true;
							break;
						}
					}
					if (skip == false)
					{
						std::string fileName = this->dir + '/' + str.C_Str();
						Texture texture;
						texture.id = mdLoadTexture(fileName);
						texture.type = typeName;
						texture.path = str.C_Str();
						textures.push_back(texture);
						texturesLoaded.push_back(texture);
						
					}

					return textures;
				}
			}

			

		};
	}
}
}

#endif // !MODEL_H
