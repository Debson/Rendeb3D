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
		m_Graphics->GetModel()->LoadAnimation(name, path);
	}

	void engine::Animator::PlayAnimation(std::string const &name) const
	{
		m_Graphics->GetModel()->ChangeAnimation(name);
	}
}