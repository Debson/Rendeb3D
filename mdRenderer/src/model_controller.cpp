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

	void graphics::ModelController::SetParameter(std::string const &name, b8 val)
	{
		FindParam(name)->mVal.b = val;
	}

	void graphics::ModelController::CreateTransition(std::string const &firstAnim, std::string const &secondAnim, f32 time)
	{
		GetAnimation(firstAnim)->AddTransition(transition_t(secondAnim, time));
	}

	void graphics::ModelController::SetExitTransition(std::string const &name, b8 val)
	{
		m_AnimationsLoaded[name]->mHasExitTime = val;
	}

	void graphics::ModelController::SetTransitionDuration(std::string const &name, f32 dur)
	{
		//animationsLoaded[name]->mTransDuration = dur;
	}

	void graphics::ModelController::SetTransitionCondition(graphics::transition_t *trans, std::string const &paramName, int condition, f32 val)
	{
		type_t type;
		type.f = val;
		trans->SetCondition(FindParam(paramName), condition, type, MD_FLOAT);
	}

	void graphics::ModelController::SetTransitionCondition(graphics::transition_t *trans, std::string const &paramName, int condition, s32 val)
	{
		type_t type;
		type.i = val;
		trans->SetCondition(FindParam(paramName), condition, type, MD_INT);
	}

	void graphics::ModelController::SetTransitionCondition(graphics::transition_t *trans, std::string const &paramName, int condition, b8 val)
	{
		type_t type;
		type.b = val;
		trans->SetCondition(FindParam(paramName), condition, type, MD_BOOLEAN);
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
	graphics::anim_t *graphics::ModelController::GetAnimation(std::string const &name)
	{
		return m_AnimationsLoaded[name];
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
				return &m_Parameters[i];
		}

		return nullptr;
	}
}
}