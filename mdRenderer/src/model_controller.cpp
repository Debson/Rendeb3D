#include "model_controller.h"


namespace md
{
namespace engine
{
	
	graphics::ModelController::ModelController(std::string const &path) : Model(path) { }

	void graphics::ModelController::AnimateBones(f64 timeInSeconds, std::vector<glm::mat4> &transforms)
	{
		this->BoneTransform(timeInSeconds, transforms);
	}

	void graphics::ModelController::LoadAnimation(std::string const &name, std::string const &path)
	{
		this->loadAnim(name, path);
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

	void graphics::ModelController::DrawModel(Shader *shader)
	{
		this->Draw(shader);
	}

	void graphics::ModelController::SetCurrentAnimation(std::string const &name)
	{
		this->ChangeAnimation(name);
	}

	graphics::anim_t *graphics::ModelController::GetAnimation(std::string const &name)
	{
		return m_AnimationsLoaded[name];
	}

	b8 graphics::ModelController::IsAnimated()
	{
		return m_AnimationsLoaded.size() > 0;
	}
}
}