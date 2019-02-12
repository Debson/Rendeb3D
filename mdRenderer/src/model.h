#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <map>
#include <fstream>
#include <iostream>
#include <map>
#include <algorithm>

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
#include "graphics_types.h"
#include "debug.h"

#include "md_load_texture.h"

//#define DEBUG

namespace md
{
namespace engine
{
	namespace graphics
	{
		typedef std::vector<Texture>		Textures;
		
		
		struct param_t
		{
			param_t() : mIsTrigger(false) { }
			template <class T>
			T getValue(u32 type)
			{
				T val;
				switch (type)
				{
				case MD_INT: {
					val = mVal.i;
					break;
				}
				case MD_FLOAT: {
					val = mVal.f;
					break;
				}
				case MD_BOOLEAN: {
					val = mVal.b;
					break;
				}
				}

				return val;
			}

			std::string mName;
			b8 mIsTrigger;
			type_t mVal;
		};

		struct condition_t
		{
			param_t *mParam;
			u32 mConditionType;
			type_t mConditionVal;
			u32 mConditionValType;
		};

		struct transition_t
		{
			transition_t(std::string const &nextAnimName, f32 transDur) : mNextAnimName(nextAnimName), 
																		  mDuration(transDur), 
																		  mTimeElapsed(0.f),
																		  mHasExitTime(true) { }
			void Reset() { mTimeElapsed = 0.f; }
			void AddCondition(param_t *p, u32 type, type_t conditionVal, u32 conditionValType)
			{
				condition_t con;
				con.mParam = p;
				conditionValType == MD_TRIGGER ? p->mIsTrigger = true : 0;
				con.mConditionType = type;
				con.mConditionVal = conditionVal;
				con.mConditionValType = conditionValType;
				mConditions.push_back(con);
			}

			std::string mNextAnimName;
			f32 mDuration;
			f32 mTimeElapsed;
			b8 mHasExitTime;
			std::vector<condition_t> mConditions;
			
		};

		struct anim_t
		{
			anim_t(std::string const &name, std::string const &path) : mTimeElapsed(0.f), mStartTime(0.f), mScene(nullptr), mInterp(0.f)
			{ 
				mScene = mImporter.ReadFile(path, aiProcess_Triangulate
					| aiProcess_GenSmoothNormals
					| aiProcess_FlipUVs
					| aiProcess_JoinIdenticalVertices);

				if (!mScene || !mScene->mRootNode)
				{
					std::cout << "ERROR::ASSIMP:: " << mImporter.GetErrorString() << std::endl;
					return;
				}

				mDuration = mScene->mAnimations[0]->mDuration;
				mName = name;
			}

			void Reset() { mTimeElapsed = 0.f; mStartTime = time::Time(); mInterp = 0.f; }

			transition_t *FindTransition(std::string const &nextAnimName)
			{
				for (u32 i = 0; i < mTransitions.size(); i++)
				{
					if (mTransitions[i].mNextAnimName == nextAnimName)
					{
						mTransitions[i].Reset();
						return &mTransitions[i];
					}
				}

				return nullptr;
			}

			void AddTransition(transition_t trans)
			{
				mTransitions.push_back(trans);
			}

			Assimp::Importer mImporter;
			const aiScene *mScene;

			std::string mName;
			f64 mTimeElapsed;	// Current time of animation in ticks
			f64 mStartTime;		// Time when animation started in ticks
			f64 mDuration;

			f32 mCurrTime;
			f32 mInterp;

			std::vector<transition_t> mTransitions;
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
				for (auto & anim : m_AnimationsLoaded)
				{
					delete anim.second;
				}
			}

			void Draw(Shader *shader)
			{
				for (auto & i : m_Meshes)
					i.Draw(shader);
			}

		protected:
			typedef std::vector<param_t> Parameters;

			Parameters m_Parameters;
			std::vector<transition_t> m_TranstionStack;


			void loadAnim(std::string const &name, std::string const &path)
			{
				anim_t *anim = new anim_t(name, path);
				
				m_AnimationsLoaded.insert(std::make_pair(name, anim));
			}

			/*	@param name		-> name of next animation
				@param trans	-> param that induced animation change
				
			*/
			void ChangeAnimation(std::string const &name)
			{
				if (m_AnimationsLoaded.find(name) != m_AnimationsLoaded.end())
				{
					m_CurrentScene->mCurrTime = m_CurrentScene->mTimeElapsed;
					m_PreviousScene = m_CurrentScene;
					m_CurrentScene = m_AnimationsLoaded[name];
					m_CurrentScene->Reset();
					if(m_PreviousScene->mName != m_CurrentScene->mName)
						m_CurrentTransition = m_PreviousScene->FindTransition(m_CurrentScene->mName);
					assert(m_CurrentScene != nullptr);
					//assert(m_CurrentTransition != nullptr);
				}
				else
					// Add md_error
					std::cout << "ERROR:: Couldn't find animation: " + name << std::endl;
			}

			void BoneTransform(f64 timeInSeconds, std::vector<glm::mat4> &transforms)
			{
				aiMatrix4x4 identity = aiMatrix4x4();

				updateCurrentAnimation();
				
				ReadNodeHierarchy(m_CurrentScene->mTimeElapsed, m_CurrentScene->mScene->mRootNode, identity);

				transforms.resize(m_BonesInfo.size());
				for (u32 i = 0; i < m_BonesInfo.size(); i++)
				{
					transforms[i] = mdEngine::math::aiMatrix4x4ToGlm(m_BonesInfo[i].finalTransform);
				}
			}

			// Change properties's names to be in accordance with naming convention

			Textures m_TexturesLoaded;
			Animations m_AnimationsLoaded;
			std::vector<Mesh> m_Meshes;
			std::vector<BoneInfo> m_BonesInfo;

			std::string dir;
			b8 gammaCorrection;

			// Create get methods 
			b8 m_hasBones;
			std::string m_Path;

		private:

			/*	@param 'scene' : either m_CurrentScene or m_PreviousScene
				returns : a pointer to the animation, which should be used as a next animation
			*/
				
			anim_t *checkTransitionConditions(anim_t *scene)
			{
				/* Animation doesn't have any transitions, loop it. */
				if (scene->mTransitions.empty())
					return scene;
				
				anim_t *ret = scene;
				u32 conditionCounter = 0;
				for (auto & i : scene->mTransitions)
				{
					conditionCounter += i.mConditions.size();
					for (auto & j : i.mConditions)
					{
						switch (j.mConditionType)
						{
						case MD_GREATER: {
							switch (j.mConditionValType)
							{
							case MD_INT: {
								if (j.mParam->mVal.i > j.mConditionVal.i)
									ret = m_AnimationsLoaded[i.mNextAnimName];
								break;
							}
							case MD_FLOAT: {
								//std::cout << j.mParam->mVal.f << std::endl;
								if (j.mParam->mVal.f > j.mConditionVal.f)
									ret = m_AnimationsLoaded[i.mNextAnimName];
								break;
							}
							}
							break;
						}
						case MD_LESS: {
							switch (j.mConditionValType)
							{
							case MD_INT: {
								if (j.mParam->mVal.i < j.mConditionVal.i)
									ret = m_AnimationsLoaded[i.mNextAnimName];
								break;
							}
							case MD_FLOAT: {
								if (j.mParam->mVal.f < j.mConditionVal.f)
									ret = m_AnimationsLoaded[i.mNextAnimName];
								break;
							}
							}
							break;
						}
						case MD_BOOLEAN: {
							if (j.mParam->mVal.b == j.mConditionVal.b)
								ret = m_AnimationsLoaded[i.mNextAnimName];
							break;
						}
						case MD_TRIGGER: {
							if (j.mParam->mVal.b == j.mConditionVal.b)
								ret = m_AnimationsLoaded[i.mNextAnimName];
							break;
						}
						}

						// Condition that cause change of animation is met, break out off the loop
						if (ret != scene)
							return ret;
					}
				}

				/* */
				if (conditionCounter == 0 && ret == scene && !scene->mTransitions.empty())
				{
					if (scene->mTimeElapsed + 1.f >= scene->mDuration && scene->mTransitions[0].mHasExitTime)
					{
						ret = m_AnimationsLoaded[scene->mTransitions[0].mNextAnimName];
					}
				}

				return ret;
			}

			void updateCurrentAnimation()
			{
				anim_t *playAnim;
				playAnim = checkTransitionConditions(m_CurrentScene);

				if (playAnim != m_CurrentScene)
				{
					ChangeAnimation(playAnim->mName);
				}

				if (m_CurrentScene->mTimeElapsed + 1.f >= m_CurrentScene->mDuration)
				{
					debug::log("Animation changed to: " + playAnim->mName);

					ChangeAnimation(playAnim->mName);
					
				}
				else
				{
					m_CurrentScene->mTimeElapsed = time::Time() - m_CurrentScene->mStartTime;
					m_CurrentScene->mTimeElapsed *= m_CurrentScene->mScene->mAnimations[0]->mTicksPerSecond;
				}
			}

			void updateCurrentTransition()
			{
				if (m_CurrentTransition->mTimeElapsed > m_CurrentTransition->mDuration)
				{
					m_CurrentTransition = nullptr;
					m_PreviousScene->mInterp = 0.f;
				}
				else
				{
					
					m_CurrentTransition->mTimeElapsed += time::DeltaTime;
					/* This line gives much smoother transition(Unity style transition), where
						========\==\				First animation timeline
								 \  \					transition
								  \  \					transition
								   \===\=====		Second animation timeline 

					*/
					m_CurrentScene->mCurrTime += (time::DeltaTime * m_CurrentScene->mScene->mAnimations[0]->mTicksPerSecond);
					//debug::log(m_CurrentScene->mCurrTime);
					m_PreviousScene->mInterp = 1.f / m_CurrentTransition->mDuration * m_CurrentTransition->mTimeElapsed;
					if (m_PreviousScene->mInterp > 1.f)
						m_PreviousScene->mInterp = 1.f;
				}
			}
			
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

			/****************************/
#ifndef DEBUG
			aiMatrix4x4 CalcInterpolatedPositionTransition(f32 animTime, const aiNodeAnim *nodeAnim, const aiNodeAnim *prevNodeAnim)
			{

				aiVector3D translation;

				if (prevNodeAnim->mNumPositionKeys == 1)
				{
					translation = prevNodeAnim->mPositionKeys[0].mValue;
				}
				else
				{
					u32 frameIndex = 0;
					for (uint32_t i = 0; i < nodeAnim->mNumPositionKeys - 1; i++)
					{
						if (animTime < (float)nodeAnim->mPositionKeys[i + 1].mTime)
						{
							frameIndex = i;
							break;
						}
					}

					u32 prevFrameIndex = 0;
					for (uint32_t i = 0; i < prevNodeAnim->mNumPositionKeys - 1; i++)
					{
						if (m_PreviousScene->mCurrTime < (float)prevNodeAnim->mPositionKeys[i + 1].mTime)
						{
							prevFrameIndex = i;
							break;
						}
					}
					
					aiVectorKey prevFrame = prevNodeAnim->mPositionKeys[prevFrameIndex];;
					aiVectorKey nextFrame = nodeAnim->mPositionKeys[frameIndex];;

					const aiVector3D& start = prevFrame.mValue;
					const aiVector3D& end = nextFrame.mValue;

					translation = (start + m_PreviousScene->mInterp * (end - start));

					//translation = m_PreviousScene->mCurrTrans;
				}

				aiMatrix4x4 mat;
				aiMatrix4x4::Translation(translation, mat);
				return mat;
			}

			aiMatrix4x4 CalcInterploatedRotationTransition(f32 animTime, const aiNodeAnim *nodeAnim, const aiNodeAnim *prevNodeAnim)
			{
				aiQuaternion rotation;

				if (prevNodeAnim->mNumRotationKeys == 1)
				{
					rotation = prevNodeAnim->mRotationKeys[0].mValue;
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

					u32 prevFrameIndex = 0;
					for (uint32_t i = 0; i < prevNodeAnim->mNumRotationKeys - 1; i++)
					{
						if (m_PreviousScene->mCurrTime < (float)prevNodeAnim->mRotationKeys[i + 1].mTime)
						{
							prevFrameIndex = i;
							break;
						}
					}

					aiQuatKey prevFrame = prevNodeAnim->mRotationKeys[prevFrameIndex];;
					aiQuatKey nextFrame = nodeAnim->mRotationKeys[frameIndex];


					const aiQuaternion& start = prevFrame.mValue;
					const aiQuaternion& end = nextFrame.mValue;

					aiQuaternion::Interpolate(rotation, start, end, m_PreviousScene->mInterp);
					rotation.Normalize();

					//rotation = m_PreviousScene->mCurrRot;
				}

				aiMatrix4x4 mat(rotation.GetMatrix());
				return mat;
			}

			aiMatrix4x4 CalcInterpolatedScalingTransition(f32 animTime, const aiNodeAnim *nodeAnim, const aiNodeAnim *prevNodeAnim)
			{
				aiVector3D scale;

				if (prevNodeAnim->mNumScalingKeys == 1)
				{
					scale = prevNodeAnim->mScalingKeys[0].mValue;
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

					u32 prevFrameIndex = 0;
					for (uint32_t i = 0; i < prevNodeAnim->mNumScalingKeys - 1; i++)
					{
						if (m_PreviousScene->mCurrTime < (float)prevNodeAnim->mScalingKeys[i + 1].mTime)
						{
							prevFrameIndex = i;
							break;
						}
					}

					aiVectorKey prevFrame = prevNodeAnim->mScalingKeys[prevFrameIndex];;
					aiVectorKey nextFrame = nodeAnim->mScalingKeys[frameIndex];

					const aiVector3D& start = prevFrame.mValue;
					const aiVector3D& end = nextFrame.mValue;

					scale = (start + m_PreviousScene->mInterp * (end - start));

					//scale = m_PreviousScene->mCurrScale;
				}

				aiMatrix4x4 mat;
				aiMatrix4x4::Scaling(scale, mat);
				return mat;
			}
#else
			
			aiMatrix4x4 CalcInterpolatedPositionTransition(f32 animTime, f32 prevAnimTime, const aiNodeAnim *nodeAnim, const aiNodeAnim *prevNodeAnim)
			{

				aiVector3D translation;

				if (prevNodeAnim->mNumPositionKeys == 1)
				{
					translation = prevNodeAnim->mPositionKeys[0].mValue;
				}
				else
				{
					u32 frameIndex = 0;
					for (uint32_t i = 0; i < nodeAnim->mNumPositionKeys - 1; i++)
					{
						if (animTime < (float)nodeAnim->mPositionKeys[i + 1].mTime)
						{
							frameIndex = i;
							break;
						}
					}

					u32 prevFrameIndex = 0;
					for (uint32_t i = 0; i < prevNodeAnim->mNumPositionKeys - 1; i++)
					{
						if (prevAnimTime < (float)prevNodeAnim->mPositionKeys[i + 1].mTime)
						{
							prevFrameIndex = i;
							break;
						}
					}

					aiVectorKey prevFrame = prevNodeAnim->mPositionKeys[prevFrameIndex];;
					aiVectorKey nextFrame = nodeAnim->mPositionKeys[frameIndex];;

					const aiVector3D& start = prevFrame.mValue;
					const aiVector3D& end = nextFrame.mValue;

					translation = (start + m_PreviousScene->mInterp * (end - start));

					//translation = m_PreviousScene->mCurrTrans;
				}

				aiMatrix4x4 mat;
				aiMatrix4x4::Translation(translation, mat);
				return mat;
			}

			aiMatrix4x4 CalcInterploatedRotationTransition(f32 animTime, f32 prevAnimTime, const aiNodeAnim *nodeAnim, const aiNodeAnim *prevNodeAnim)
			{
				aiQuaternion rotation;

				if (prevNodeAnim->mNumRotationKeys == 1)
				{
					rotation = prevNodeAnim->mRotationKeys[0].mValue;
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

					u32 prevFrameIndex = 0;
					for (uint32_t i = 0; i < prevNodeAnim->mNumRotationKeys - 1; i++)
					{
						if (prevAnimTime < (float)prevNodeAnim->mRotationKeys[i + 1].mTime)
						{
							prevFrameIndex = i;
							break;
						}
					}

					aiQuatKey prevFrame = prevNodeAnim->mRotationKeys[prevFrameIndex];;
					aiQuatKey nextFrame = nodeAnim->mRotationKeys[frameIndex];


					const aiQuaternion& start = prevFrame.mValue;
					const aiQuaternion& end = nextFrame.mValue;

					aiQuaternion::Interpolate(rotation, start, end, m_PreviousScene->mInterp);
					rotation.Normalize();

					//rotation = m_PreviousScene->mCurrRot;
				}

				aiMatrix4x4 mat(rotation.GetMatrix());
				return mat;
			}

			aiMatrix4x4 CalcInterpolatedScalingTransition(f32 animTime, f32 prevAnimTime, const aiNodeAnim *nodeAnim, const aiNodeAnim *prevNodeAnim)
			{
				aiVector3D scale;

				if (prevNodeAnim->mNumScalingKeys == 1)
				{
					scale = prevNodeAnim->mScalingKeys[0].mValue;
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

					u32 prevFrameIndex = 0;
					for (uint32_t i = 0; i < prevNodeAnim->mNumScalingKeys - 1; i++)
					{
						if (prevAnimTime < (float)prevNodeAnim->mScalingKeys[i + 1].mTime)
						{
							prevFrameIndex = i;
							break;
						}
					}

					aiVectorKey prevFrame = prevNodeAnim->mScalingKeys[prevFrameIndex];;
					aiVectorKey nextFrame = nodeAnim->mScalingKeys[frameIndex];

					const aiVector3D& start = prevFrame.mValue;
					const aiVector3D& end = nextFrame.mValue;

					scale = (start + m_PreviousScene->mInterp * (end - start));

					//scale = m_PreviousScene->mCurrScale;
				}

				aiMatrix4x4 mat;
				aiMatrix4x4::Scaling(scale, mat);
				return mat;
			}
#endif
			/***************************/


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

					//delta = m_CurrentScene->mTimeElapsed / m_CurrentScene->mDuration;

					const aiVector3D& start = currentFrame.mValue;
					const aiVector3D& end = nextFrame.mValue;

					translation = (start + delta * (end - start));
					//std::cout << delta << std::endl;

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
					//delta = m_CurrentScene->mTimeElapsed / m_CurrentScene->mDuration;

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
					//delta = m_CurrentScene->mTimeElapsed / m_CurrentScene->mDuration;

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
				const aiNodeAnim *nodeAnim = FindNodeAnim(pAnim, nodeName);
				const aiAnimation *pPrevAnim = m_PreviousScene->mScene->mAnimations[0];
				const aiNodeAnim *prevNodeAnim = FindNodeAnim(pPrevAnim, nodeName);

				aiMatrix4x4 nodeTransformation = node->mTransformation;


				aiMatrix4x4 matScale;
				aiMatrix4x4 matRotation;
				aiMatrix4x4 matTranslation;
#ifndef DEBUG
				if (nodeAnim && prevNodeAnim && m_CurrentTransition)
#endif
				{
#ifndef DEBUG
						matScale = CalcInterpolatedScalingTransition(animTime, nodeAnim, prevNodeAnim);

						matRotation = CalcInterploatedRotationTransition(animTime, nodeAnim, prevNodeAnim);

						matTranslation = CalcInterpolatedPositionTransition(animTime, nodeAnim, prevNodeAnim);
						updateCurrentTransition();

						nodeTransformation = matTranslation * matRotation * matScale;
#else

						ImGui::Begin("TEST");
						static float currAnimTime = 25;
						static float prevAnimTime = 405;
						ImGui::SliderFloat("currAnimTime", &currAnimTime, 0.f, 43.f);
						ImGui::SameLine();
						ImGui::SliderFloat("prevAnimTime", &prevAnimTime, 0.f, 500.f);

						matScale = CalcInterpolatedScalingTransition(currAnimTime, prevAnimTime, nodeAnim, prevNodeAnim);

						matRotation = CalcInterploatedRotationTransition(currAnimTime, prevAnimTime, nodeAnim, prevNodeAnim);

						matTranslation = CalcInterpolatedPositionTransition(currAnimTime, prevAnimTime, nodeAnim, prevNodeAnim);
						ImGui::End();

						/*if (input::IsKeyDown(input::KeyCode::H))
						{
							matScale = CalcInterpolatedScaling(currAnimTime, nodeAnim);

							matRotation = CalcInterploatedRotation(currAnimTime, nodeAnim);

							matTranslation = CalcInterpolatedPosition(currAnimTime, nodeAnim);
						}
						else
						{
							matScale = CalcInterpolatedScaling(prevAnimTime, prevNodeAnim);

							matRotation = CalcInterploatedRotation(prevAnimTime, prevNodeAnim);

							matTranslation = CalcInterpolatedPosition(prevAnimTime, prevNodeAnim);
						}*/
#endif
				}
#ifndef DEBUG
				else if(nodeAnim && m_CurrentTransition == nullptr)
				{
					matScale = CalcInterpolatedScaling(animTime, nodeAnim);

					matRotation = CalcInterploatedRotation(animTime, nodeAnim);

					matTranslation = CalcInterpolatedPosition(animTime, nodeAnim);

					nodeTransformation = matTranslation * matRotation * matScale;
				}
#endif

				
				aiMatrix4x4 globalTransformation = parentTransform * nodeTransformation;

				auto it = std::find_if(m_BonesInfo.begin(), m_BonesInfo.end(), [&]
				(const BoneInfo & ref) { return ref.name == nodeName; });

				if (it != m_BonesInfo.end())
				{
					BoneInfo *bone = &m_BonesInfo.at(std::distance(m_BonesInfo.begin(), it));
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

			u32				m_BoneCount;
			u32				m_VerticesCount;
			anim_t			*m_CurrentScene;
			anim_t			*m_PreviousScene;
			transition_t	*m_CurrentTransition;

			void loadModel(std::string const &path)
			{
				loadAnim("Start", path);

				// prev == current
				m_CurrentScene = m_AnimationsLoaded["Start"];
				m_PreviousScene = m_CurrentScene;

				dir = path.substr(0, path.find_last_of('/'));

				processNode(m_CurrentScene->mScene->mRootNode, m_CurrentScene->mScene);
			}

			void processNode(aiNode *node, const aiScene *scene)
			{
				for (u32 i = 0; i < node->mNumMeshes; i++)
				{
					aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
					m_Meshes.push_back(processMesh(mesh, scene));
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

					auto it = std::find_if(m_BonesInfo.begin(), m_BonesInfo.end(), [&]
					(const BoneInfo & ref) { return ref.name == boneName; });
					if (it == m_BonesInfo.end())
					{
						boneIndex = m_BoneCount;
						m_BoneCount++;
						BoneInfo bi;
						bi.boneOffset = mesh->mBones[i]->mOffsetMatrix;
						bi.name = boneName;
						bi.index = boneIndex;

						m_BonesInfo.push_back(bi);
					}
					else
					{
						boneIndex = m_BonesInfo.at(std::distance(m_BonesInfo.begin(), it)).index;
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
					for (auto & j : m_TexturesLoaded)
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
						m_TexturesLoaded.push_back(texture);
						
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
