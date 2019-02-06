#ifndef ANIMATOR_H
#define ANIMATOR_H

#include <iostream>

#include "objects.h"

namespace md
{
	namespace engine
	{
		class Animator
		{
		public:
			Animator();
			Animator(GameObject *obj);

			enum TransitionType
			{
				BILATERAL,
				ONESIDED
			};

			void AddAnimation(std::string const &name, std::string const &path, b8 hasExitTime = true) const;
			void AddTransition(std::string const &firstAnim, std::string const &secondAnim, f32 time = 10.1f, TransitionType transType = ONESIDED);
			void PlayAnimation(std::string const &name) const;

		private:
			Graphics *m_Graphics;

		};
	}
}


#endif // !ANIMATOR_H
