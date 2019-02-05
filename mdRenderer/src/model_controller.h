#ifndef MODEL_CONTROLLER_H
#define MODEL_CONTROLLER_H

#include "model.h"

namespace md
{
namespace engine
{
	namespace graphics
	{
		class ModelController : private Model
		{
		public:
			// BoneTransform, LoadAnimation, ChangeAnimation, SetExitTransition
			// SetTransitionDuration, DrawModel
			// SetAnimation
			// IsAnimated, 
			// Animation getter
			ModelController(std::string const &path);

			void AnimateBones(f64 timeInSeconds, std::vector<glm::mat4> &transforms);

			void LoadAnimation(std::string const &name, std::string const &path);

			void CreateTransition(std::string const &firstAnim, std::string const &secondAnim, f32 time);

			void SetExitTransition(std::string const &name, b8 val);

			void SetTransitionDuration(std::string const &name, f32 dur);

			void DrawModel(Shader *shader);

			void SetCurrentAnimation(std::string const &name);

			anim_t *GetAnimation(std::string const &name);

			b8 IsAnimated();

		private:

		};




	}
}
}


#endif // !MODEL_CONTROLLER_H
