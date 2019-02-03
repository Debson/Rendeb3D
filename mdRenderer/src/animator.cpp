#include "animator.h"

namespace md
{
	engine::Animator::Animator() { }
	engine::Animator::Animator(GameObject *obj) 
	{ 
		m_Graphics = obj->graphics; 
	}

	void engine::Animator::AddAnimation(std::string const &name, std::string const &path) const
	{
		m_Graphics->GetModelController()->LoadAnimation(name, path);
	}

	void engine::Animator::AddAnimation(std::string const &name, std::string const &path, b8 hasExitTime, f32 transDuration) const
	{
		m_Graphics->GetModelController()->LoadAnimation(name, path);
		m_Graphics->GetModelController()->SetExitTransition(name, hasExitTime);
		m_Graphics->GetModelController()->SetTransitionDuration(name, transDuration);
	}

	void engine::Animator::PlayAnimation(std::string const &name) const
	{
		m_Graphics->GetModelController()->SetCurrentAnimation(name);
	}
}