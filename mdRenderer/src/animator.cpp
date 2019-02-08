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

	void engine::Animator::AddParameter(std::string const &name)
	{
		m_Graphics->GetModelController()->AddParameter(name);
	}

	void engine::Animator::SetFloat(std::string const &name, f32 &val)
	{
		m_Graphics->GetModelController()->SetParameter(name, val);
	}

	void engine::Animator::SetInt(std::string const &name, s32 &val)
	{
		m_Graphics->GetModelController()->SetParameter(name, val);
	}

	void engine::Animator::SetBool(std::string const &name, b8 &val)
	{
		m_Graphics->GetModelController()->SetParameter(name, val);
	}

	engine::graphics::transition_t *engine::Animator::GetTransition(std::string const &first, std::string const &second)
	{
		return m_Graphics->GetModelController()->GetTransition(first, second);
	}

	void engine::Animator::AddTransition(std::string const &firstAnim, std::string const &secondAnim, const f32 &time, TransitionType transType)
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

	/*template <typename T>
	void engine::Animator::AddTransitionCondition(std::string const &firstTransName, 
												  std::string const &secondTransName, 
												  std::string const &paramName, 
												  int condition,
												  T val)
	{
		m_Graphics->GetModelController()->SetTransitionCondition(GetTransition(firstTransName, secondTransName), paramName, condition, val);
	}*/

	void engine::Animator::PlayAnimation(std::string const &name) const
	{
		m_Graphics->GetModelController()->SetCurrentAnimation(name);
	}
}