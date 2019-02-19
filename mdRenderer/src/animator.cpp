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

	void engine::Animator::SetBool(std::string const &name, b8 val)
	{
		m_Graphics->GetModelController()->SetParameter(name, val);
	}


	void engine::Animator::SetTrigger(std::string const &name)
	{
		m_Graphics->GetModelController()->SetParameter(name, true, true);
	}

	engine::graphics::transition_t *engine::Animator::GetTransition(std::string const &first, std::string const &second)
	{
		return m_Graphics->GetModelController()->GetTransition(first, second);
	}

	void engine::Animator::AddTransition(std::string const &firstAnim, std::string const &secondAnim, const f32 &time, graphics::TransitionType transType)
	{
		switch (transType)
		{
		case graphics::TransitionType::ONESIDED: {
			m_Graphics->GetModelController()->CreateTransition(firstAnim, secondAnim, time, transType);
			break;
		}
		case graphics::TransitionType::BILATERAL: {
			m_Graphics->GetModelController()->CreateTransition(firstAnim, secondAnim, time, transType);
			m_Graphics->GetModelController()->CreateTransition(secondAnim, firstAnim, time, transType);
			break;
		}
		}
	}

	void engine::Animator::AddTransitionCondition(std::string const &firstTransName, std::string const &secondTransName, std::string const &paramName, b8 condition)
	{
		m_Graphics->GetModelController()->SetTransitionCondition(GetTransition(firstTransName, secondTransName), paramName, condition);
	}

	void engine::Animator::AddTransitionCondition(std::string const &firstTransName, std::string const &secondTransName, std::string const &paramName, int condition)
	{
		m_Graphics->GetModelController()->SetTransitionCondition(GetTransition(firstTransName, secondTransName), paramName, condition);

	}

	void engine::Animator::SetTransitionExitTimeState(std::string const &firstTransName, std::string const &secondTransName, b8 val)
	{
		m_Graphics->GetModelController()->SetTransitionExitTimeState(firstTransName, secondTransName, val);
	}

	void engine::Animator::PlayAnimation(std::string const &name) const
	{
		m_Graphics->GetModelController()->SetCurrentAnimation(name);
	}
}