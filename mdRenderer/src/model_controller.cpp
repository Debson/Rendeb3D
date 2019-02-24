#include "model_controller.h"


namespace md
{
namespace engine
{
	namespace graphics
	{
		
	}


	graphics::ModelController::ModelController(std::string const &path) : Model(path) { }

	void graphics::ModelController::AnimateBones(f64 timeInSeconds, std::vector<glm::mat4> &transforms)
	{
		this->BoneTransform(timeInSeconds, transforms);
	}

	void graphics::ModelController::LoadAnimation(std::string const &name, std::string const &path)
	{
		this->loadAnim(name, path);
	}

	void graphics::ModelController::AddParameter(std::string const &name)
	{
		param_t param;
		param.mName = name;
		m_Parameters.push_back(param);
	}

	void graphics::ModelController::SetParameter(std::string const &name, f32 val)
	{
		FindParam(name)->mVal.f = val;
	}

	void graphics::ModelController::SetParameter(std::string const &name, s32 val)
	{
		FindParam(name)->mVal.i = val;
	}

	void graphics::ModelController::SetParameter(std::string const &name, b8 val, b8 isTrigger)
	{
		if (isTrigger)
		{
			FindParam(name)->mVal.b = true;
			FindParam(name)->mIsTrigger = true;
		}
		else
			FindParam(name)->mVal.b = val;
	}

	void graphics::ModelController::CreateTransition(std::string const &firstAnim, std::string const &secondAnim, f32 time, graphics::TransitionType type)
	{
		std::shared_ptr<graphics::anim_t> anim = GetAnimation(firstAnim);
		if(anim != nullptr)
			GetAnimation(firstAnim)->AddTransition(transition_t(secondAnim, time, type));
		else
			md_log("Transition: | %s --> %s | could not be created.", firstAnim.c_str(), secondAnim.c_str());;
	}

	void graphics::ModelController::SetTransitionExitTimeState(std::string const &firstAnim, std::string const &secondAnim, b8 val)
	{
		transition_t *trans = nullptr;
		if((trans = GetAnimation(firstAnim)->FindTransition(secondAnim)) != nullptr)
			GetAnimation(firstAnim)->FindTransition(secondAnim)->mHasExitTime = val;
		else
			md_log("Exit time for animation: %s and transition: %s could not be set.", firstAnim.c_str(), secondAnim.c_str());;
	}

	void graphics::ModelController::SetTransitionDuration(std::string const &name, f32 dur)
	{
		//animationsLoaded[name]->mTransDuration = dur;
	}

	void graphics::ModelController::SetTransitionCondition(graphics::transition_t *trans, std::string const &paramName, int condition, f32 val)
	{
		if (trans != nullptr)
		{
			type_t type;
			type.f = val;
			trans->AddCondition(FindParam(paramName), condition, type, MD_FLOAT);
		}
	}

	void graphics::ModelController::SetTransitionCondition(graphics::transition_t *trans, std::string const &paramName, int condition, s32 val)
	{
		if (trans != nullptr)
		{
			type_t type;
			type.i = val;
			trans->AddCondition(FindParam(paramName), condition, type, MD_INT);
		}
	}

	void graphics::ModelController::SetTransitionCondition(graphics::transition_t *trans, std::string const &paramName, int condition, b8 val)
	{
		if (trans != nullptr)
		{
			type_t type;
			type.b = val;
			// Little bit hacky. Don't have to specify the condition(initially it was MD_TRUE/MD_FALSE).
			trans->AddCondition(FindParam(paramName), MD_BOOLEAN, type, MD_BOOLEAN);
		}
	}

	void graphics::ModelController::SetTransitionCondition(graphics::transition_t *trans, std::string const &paramName, int condition)
	{
		if (trans != nullptr)
		{
			type_t type;
			type.b = true;
			trans->AddCondition(FindParam(paramName), condition, type, MD_TRIGGER);
		}
	}

	void graphics::ModelController::DrawModel(Shader *shader)
	{
		this->Draw(shader);
	}

	void graphics::ModelController::SetCurrentAnimation(std::string const &name)
	{
		this->ChangeAnimation(name);
	}

	// Add error checking!!!!!!!!
	std::shared_ptr<graphics::anim_t> graphics::ModelController::GetAnimation(std::string const &name)
	{
		try
		{
			return m_AnimationsLoaded[name];
		}
		catch (const std::out_of_range &e)
		{
			md_log("Animation: %s does not exist!", name.c_str());
		}

		return nullptr;
	}

	graphics::transition_t *graphics::ModelController::GetTransition(std::string const &first, std::string const &second)
	{
		size_t animSize = m_AnimationsLoaded.size();
		for (auto & i : m_AnimationsLoaded)
		{
			transition_t *trans;
			if ((trans = i.second->FindTransition(second)) && i.first == first)
			{
				return trans;
			}
		}

		md_log("Transition | %s --> %s | does not exist!", first.c_str(), second.c_str());

		return nullptr;
	}

	b8 graphics::ModelController::IsAnimated()
	{
		return m_AnimationsLoaded.size() > 0;
	}

	graphics::param_t *graphics::ModelController::FindParam(std::string const &name)
	{
		size_t size = m_Parameters.size();
		for (u32 i = 0; i < size; i++)
		{
			if (m_Parameters[i].mName == name)
			{
				return &m_Parameters[i];
			}
		}
		
		md_error("Parameter: %s does not exist!", name.c_str());
		
		return nullptr;
	}
}
}