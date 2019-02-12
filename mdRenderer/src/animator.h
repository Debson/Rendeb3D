#ifndef ANIMATOR_H
#define ANIMATOR_H

#include <iostream>

#include "objects.h"
#include "graphics_types.h"

namespace md
{
	namespace engine
	{
		class Animator
		{
		public:
			Animator();
			Animator(GameObject *obj);

			enum TransitionType : u8
			{
				BILATERAL,
				ONESIDED
			};

			void AddAnimation(std::string const &name, std::string const &path, b8 hasExitTime = true) const;
			void AddParameter(std::string const &name);
			void SetFloat(std::string const &name, f32 &val);
			void SetInt(std::string const &name, s32 &val);
			void SetBool(std::string const &name, b8 val);
			void SetTrigger(std::string const &name);
			void AddTransition(std::string const &firstAnim, std::string const &secondAnim, const f32 &time = 10.1f, TransitionType transType = ONESIDED);
			// Set Float, Int condition
			template <typename T>
			void AddTransitionCondition(std::string const &firstTransName, std::string const &secondTransName, std::string const &paramName, int condition, T val)
			{
				m_Graphics->GetModelController()->SetTransitionCondition(GetTransition(firstTransName, secondTransName), paramName, condition, val);
			}
			// Set Boolean condition
			void AddTransitionCondition(std::string const &firstTransName, std::string const &secondTransName, std::string const &paramName, b8 condition);
			// Set trigger condition
			void AddTransitionCondition(std::string const &firstTransName, std::string const &secondTransName, std::string const &paramName, int condition);
			
			void SetTransitionExitTimeState(std::string const &firstTransName, std::string const &secondTransName, b8 val);

			void PlayAnimation(std::string const &name) const;

		private:
			graphics::transition_t *GetTransition(std::string const &first, std::string const &second);
			Graphics *m_Graphics;

		};
	}
}


#endif // !ANIMATOR_H
