#ifndef MODEL_CONTROLLER_H
#define MODEL_CONTROLLER_H

#include "model.h"
#include "graphics_types.h"

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


			/*	If any of the animations is playing, and any of parameters is being set, and any paremteres exists, 
				check current scene's transition's conditions

			*/

			ModelController(std::string const &path);

			void AnimateBones(f64 timeInSeconds, std::vector<glm::mat4> &transforms);

			void LoadAnimation(std::string const &name, std::string const &path);

			void AddParameter(std::string const &name);

			void SetParameter(std::string const &name, f32 val);

			void SetParameter(std::string const &name, s32 val);

			void SetParameter(std::string const &name, b8 val, b8 isTrigger = false);

			void CreateTransition(std::string const &firstAnim, std::string const &secondAnim, f32 time);

			void SetTransitionExitTimeState(std::string const &firstAnim, std::string const &secondAnim, b8 val);

			void SetTransitionDuration(std::string const &name, f32 dur);
			// **Set Float condition**
			void SetTransitionCondition(graphics::transition_t *trans, std::string const &paramName, int condition, f32 val);
			// **Set Integer condition**
			void SetTransitionCondition(graphics::transition_t *trans, std::string const &paramName, int condition, s32 val);
			// **Set Boolean condition**
			void SetTransitionCondition(graphics::transition_t *trans, std::string const &paramName, int condition, b8 val);
			// **Set Trigger condition**
			void SetTransitionCondition(graphics::transition_t *trans, std::string const &paramName, int condition);

			void DrawModel(Shader *shader);

			void SetCurrentAnimation(std::string const &name);

			anim_t *GetAnimation(std::string const &name);

			transition_t *GetTransition(std::string const &first, std::string const &second);

			b8 IsAnimated();

		private:

			param_t *FindParam(std::string const &name);


		};




	}
}
}


#endif // !MODEL_CONTROLLER_H
