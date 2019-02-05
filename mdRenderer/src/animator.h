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

			void AddAnimation(std::string const &name, std::string const &path) const;
			void AddAnimation(std::string const &name, std::string const &path, b8 hasExitTime, f32 transDuration) const;
			void AddTransition(std::string const &firstAnim, std::string const &secondAnim, f32 time = 5.1f);
			void PlayAnimation(std::string const &name) const;

		private:
			Graphics *m_Graphics;

		};
	}
}


#endif // !ANIMATOR_H
