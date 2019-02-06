#include "animator.h"

namespace md
{
	engine::Animator::Animator() { }
	engine::Animator::Animator(GameObject *obj) 
	{ 
		m_Graphics = obj->graphics; 
	}

	

	void engine::Animator::AddAnimation(std::string const &name, std::string const &path, b8 hasExitTime) const
	{
		m_Graphics->GetModelController()->LoadAnimation(name, path);
		m_Graphics->GetModelController()->SetExitTransition(name, hasExitTime);
	}


	void engine::Animator::AddTransition(std::string const &firstAnim, std::string const &secondAnim, f32 time, TransitionType transType)
	{
		switch (transType)
		{
		case ONESIDED: {
			m_Graphics->GetModelController()->CreateTransition(firstAnim, secondAnim, time);
			break;
		}
		case BILATERAL: {
			m_Graphics->GetModelController()->CreateTransition(firstAnim, secondAnim, time);
			m_Graphics->GetModelController()->CreateTransition(secondAnim, firstAnim, time);
			break;
		}
		}
	}

	void engine::Animator::PlayAnimation(std::string const &name) const
	{
		m_Graphics->GetModelController()->SetCurrentAnimation(name);
	}
}