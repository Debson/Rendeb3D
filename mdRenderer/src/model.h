#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <map>
#include <fstream>
#include <iostream>
#include <map>

/*#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_interpolation.hpp>
*/
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

#include "graphics.h"
#include "mesh.h"
#include "types.h"
#include "math.h"
#include "time.h"

#include "md_load_texture.h"

namespace md
{
namespace engine
{
	namespace graphics
	{
		typedef std::vector<Texture> Textures;
		

		/*	It must contain information about animation's length(in ticks), 
			current animation time(in ticks).
			Make it as a class. Create public methods like SetExitTime, GetAnimProgression, 
			
		*/

		struct anim_t
		{
			anim_t() : mTimeElapsed(0.f), mStartTime(0.f), mHasExitTime(true), mScene(nullptr) { }
			void Reset() { mTimeElapsed = 0.f; mStartTime = 0.f; }

			Assimp::Importer mImporter;
			const aiScene *mScene;

			f64 mTimeElapsed;	// Current time of animation in ticks
			f64 mStartTime;		// Time when animation started in ticks
			b8 mHasExitTime;	// 
			f32 mTransDuration;	//

		};

		typedef std::map<std::string, anim_t*> Animations;
		
		class Model
		{
		public:

			Model(std::string const &path, bool gamma = false) :
				m_BoneCount(0), m_VerticesCount(0), m_hasBones(false), m_Path(path)
			{
				loadModel(path);
				std::cout << "Model\n";
			}
			
			virtual ~Model() 
			{ 
				for (auto & anim : animationsLoaded)
				{
					delete anim.second;
				}
			}

			void Draw(Shader *shader)
			{
				for (auto & i : meshes)
					i.Draw(shader);
			}

		protected:
			void loadAnim(std::string const &name, std::string const &path)
			{
				anim_t *anim = new anim_t();
				anim->mScene = anim->mImporter.ReadFile(path, aiProcess_Triangulate
					| aiProcess_GenSmoothNormals
					| aiProcess_FlipUVs
					| aiProcess_JoinIdenticalVertices);

				if (!anim->mScene || !anim->mScene->mRootNode)
				{
					std::cout << "ERROR::ASSIMP:: " << anim->mImporter.GetErrorString() << std::endl;
					return;
				}

				animationsLoaded.insert(std::make_pair(name, anim));

				/*if (name == "Header")
					m_CurrentScene = animationsLoaded["Header"];*/
			}

			void ChangeAnimation(std::string const &name)
			{
				if (animationsLoaded.find(name) != animationsLoaded.end())
					m_CurrentScene = animationsLoaded[name];
				else
					// Add md_error
					std::cout << "ERROR:: Couldn't find animation: " + name << std::endl;
			}

			void BoneTransform(f64 timeInSeconds, std::vector<glm::mat4> &transforms)
			{
				aiMatrix4x4 identity = aiMatrix4x4();

				f32 ticksPerSec = (f32)(m_CurrentScene->mScene->mAnimations[0]->mTicksPerSecond != 0 ? m_CurrentScene->mScene->mAnimations[0]->mTicksPerSecond : 25.f);
				f32 timeInTicks = timeInSeconds * ticksPerSec;
				f32 animTime = fmod(timeInTicks, (f32)m_CurrentScene->mScene->mAnimations[0]->mDuration);

				ReadNodeHierarchy(animTime, m_CurrentScene->mScene->mRootNode, identity);

				transforms.resize(bonesInfo.size());
				for (u32 i = 0; i < bonesInfo.size(); i++)
				{
					transforms[i] = mdEngine::math::aiMatrix4x4ToGlm(bonesInfo[i].finalTransform);
				}
			}


			// Change properties's names to be in accordance with naming convention

			Textures texturesLoaded;
			Animations animationsLoaded;
			std::vector<Mesh> meshes;
			std::vector<BoneInfo> bonesInfo;

			std::string dir;
			b8 gammaCorrection;

			// Create get methods 
			b8 m_hasBones;
			std::string m_Path;

		private:
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

			aiMatrix4x4 CalcInterpolatedPosition(f32 animTime, const aiNodeAnim *nodeAnim)
			{

				aiVector3D translation;

				if(nodeAnim->mNumPositionKeys == 1)
				{
						translation = nodeAnim->mPositionKeys[0].mValue;
				}
				else
				{
					u32 frameIndex = 0;
					for (uint32_t i = 0; i < nodeAnim->mNumPositionKeys - 1; i++)
					{
						if(animTime < (float)nodeAnim->mPositionKeys[i + 1].mTime)
						{
							frameIndex = i;
							break;
						}
				}

					aiVectorKey currentFrame = nodeAnim->mPositionKeys[frameIndex];
					aiVectorKey nextFrame = nodeAnim->mPositionKeys[(frameIndex + 1) % nodeAnim->mNumPositionKeys];

					float delta = (animTime - (float)currentFrame.mTime) / (float)(nextFrame.mTime - currentFrame.mTime);

					const aiVector3D& start = currentFrame.mValue;
					const aiVector3D& end = nextFrame.mValue;

					translation = (start + delta * (end - start));
				}

				aiMatrix4x4 mat;
				aiMatrix4x4::Translation(translation, mat);
				return mat;
			}

			aiMatrix4x4 CalcInterploatedRotation(f32 animTime, const aiNodeAnim *nodeAnim)
			{
				aiQuaternion rotation;

				if (nodeAnim->mNumRotationKeys == 1)
				{
					rotation = nodeAnim->mRotationKeys[0].mValue;
				}
				else
				{
					uint32_t frameIndex = 0;
					for (uint32_t i = 0; i < nodeAnim->mNumRotationKeys - 1; i++)
					{
						if (animTime < (float)nodeAnim->mRotationKeys[i + 1].mTime)
						{
							frameIndex = i;
							break;
						}
					}

					aiQuatKey currentFrame = nodeAnim->mRotationKeys[frameIndex];
					aiQuatKey nextFrame = nodeAnim->mRotationKeys[(frameIndex + 1) % nodeAnim->mNumRotationKeys];

					float delta = (animTime - (float)currentFrame.mTime) / (float)(nextFrame.mTime - currentFrame.mTime);

					const aiQuaternion& start = currentFrame.mValue;
					const aiQuaternion& end = nextFrame.mValue;

					aiQuaternion::Interpolate(rotation, start, end, delta);
					rotation.Normalize();
				}

				aiMatrix4x4 mat(rotation.GetMatrix());
				return mat;
			}

			aiMatrix4x4 CalcInterpolatedScaling(f32 animTime, const aiNodeAnim *nodeAnim)
			{
				aiVector3D scale;

				if (nodeAnim->mNumScalingKeys == 1)
				{
					scale = nodeAnim->mScalingKeys[0].mValue;
				}
				else
				{
					uint32_t frameIndex = 0;
					for (uint32_t i = 0; i < nodeAnim->mNumScalingKeys - 1; i++)
					{
						if (animTime < (float)nodeAnim->mScalingKeys[i + 1].mTime)
						{
							frameIndex = i;
							break;
						}
					}

					aiVectorKey currentFrame = nodeAnim->mScalingKeys[frameIndex];
					aiVectorKey nextFrame = nodeAnim->mScalingKeys[(frameIndex + 1) % nodeAnim->mNumScalingKeys];

					float delta = (animTime - (float)currentFrame.mTime) / (float)(nextFrame.mTime - currentFrame.mTime);

					const aiVector3D& start = currentFrame.mValue;
					const aiVector3D& end = nextFrame.mValue;

					scale = (start + delta * (end - start));
				}

				aiMatrix4x4 mat;
				aiMatrix4x4::Scaling(scale, mat);
				return mat;
			}

			void ReadNodeHierarchy(f32 animTime, const aiNode *node, const aiMatrix4x4 &parentTransform)
			{
				std::string nodeName(node->mName.data);

				//const aiAnimation *pAnim = m_Scene->mAnimations[0];
				const aiAnimation *pAnim = m_CurrentScene->mScene->mAnimations[0];

				aiMatrix4x4 nodeTransformation = node->mTransformation;

				const aiNodeAnim *nodeAnim = FindNodeAnim(pAnim, nodeName);

				if (nodeAnim)
				{
					aiMatrix4x4 matScale = CalcInterpolatedScaling(animTime, nodeAnim);

					aiMatrix4x4 matRotation = CalcInterploatedRotation(animTime, nodeAnim);
					
					aiMatrix4x4 matTranslation = CalcInterpolatedPosition(animTime, nodeAnim);

					nodeTransformation = matTranslation * matRotation * matScale;
				}

				
				aiMatrix4x4 globalTransformation = parentTransform * nodeTransformation;

				auto it = std::find_if(bonesInfo.begin(), bonesInfo.end(), [&]
				(const BoneInfo & ref) { return ref.name == nodeName; });

				if (it != bonesInfo.end())
				{
					BoneInfo *bone = &bonesInfo.at(std::distance(bonesInfo.begin(), it));
					aiMatrix4x4 globalInverseTransform = m_CurrentScene->mScene->mRootNode->mTransformation;
					//aiMatrix4x4 globalInverseTransform = animationsLoaded["test"].mRootNode->mTransformation;
					globalInverseTransform.Inverse();
					bone->finalTransform = globalInverseTransform * globalTransformation * bone->boneOffset;
				}

				for (u32 i = 0; i < node->mNumChildren; i++)
					ReadNodeHierarchy(animTime, node->mChildren[i], globalTransformation);

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

			u32 m_BoneCount;
			u32 m_VerticesCount;
			anim_t *m_CurrentScene;

			void loadModel(std::string const &path)
			{
				anim_t *first = new anim_t();
				first->mScene = first->mImporter.ReadFile(path, aiProcess_Triangulate
															  | aiProcess_GenSmoothNormals
															  | aiProcess_FlipUVs
															  | aiProcess_JoinIdenticalVertices);

				if (!first->mScene || first->mScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !first->mScene->mRootNode)
				{
					std::cout << "ERROR::ASSIMP:: " << first->mImporter.GetErrorString() << std::endl;
					return;
				}

				// Save default animation on model load
				animationsLoaded.insert(std::make_pair("Start", first));

				// Set default animation
				m_CurrentScene = animationsLoaded["Start"];

				dir = path.substr(0, path.find_last_of('/'));

				processNode(m_CurrentScene->mScene->mRootNode, m_CurrentScene->mScene);
			}

			void processNode(aiNode *node, const aiScene *scene)
			{
				for (u32 i = 0; i < node->mNumMeshes; i++)
				{
					aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
					meshes.push_back(processMesh(mesh, scene));
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
						bi.boneOffset = mesh->mBones[i]->mOffsetMatrix;
						bi.name = boneName;
						bi.index = boneIndex;

						bonesInfo.push_back(bi);
					}
					else
					{
						boneIndex = bonesInfo.at(std::distance(bonesInfo.begin(), it)).index;
					}

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

					//return textures;
				}
                return textures;
			}

			

		};
	}
}
}

#endif // !MODEL_H
