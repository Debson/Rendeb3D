#include "../scriptable.h"
//#include "../md_renderer_kit.h"
#include "../objects.h"
#include "../animator.h"
#include "../input.h"


namespace md
{
	class SkeletalTests : public Script
	{
		engine::GameObject *myModel;
		engine::Animator m_Animator;

	public:

		void Start()
		{
			myModel = engine::GameObject::LoadModel("Model", "assets//Idle.fbx");

			myModel->AddComponent<engine::Animator>(myModel);
			

			m_Animator = myModel->GetComponent<engine::Animator>();

			m_Animator.AddAnimation("Goalkepper", "assets//Goalkeeper.fbx");
			m_Animator.AddAnimation("Header", "assets//Header.fbx");
			m_Animator.AddAnimation("Running", "assets//Running.fbx");

			m_Animator.AddTransition("Start", "Header", 0.2f, engine::graphics::TransitionType::BILATERAL);
			//m_Animator.AddTransition("Goalkepper", "Start", 10.f, engine::graphics::TransitionType::ONESIDED);
			m_Animator.AddTransition("Start", "Goalkepper", 0.2f, engine::graphics::TransitionType::ONESIDED);
			m_Animator.AddTransition("Start", "Running", 0.2f, engine::graphics::TransitionType::BILATERAL);

			//m_Animator.PlayAnimation("Header");
			m_Animator.AddParameter("Speed");
			m_Animator.AddParameter("Trigger");
			m_Animator.AddParameter("Bool");

			m_Animator.AddTransitionCondition("Start", "Running", "Speed", MD_GREATER, 0.2f);
			m_Animator.AddTransitionCondition("Running", "Start", "Speed", MD_LESS, 0.2f);

			m_Animator.SetTransitionExitTimeState("Start", "Running", false);

			m_Animator.SetTransitionExitTimeState("Goalkepper", "Start", false);

			m_Animator.AddTransitionCondition("Start", "Running", "Speed", MD_GREATER, 0.2f);


			myModel->transform.position.x = 0.f;
			myModel->transform.localScale = glm::vec3(0.01f);
		}

		void Update()
		{
			if (input::IsKeyPressed(input::KeyCode::R))
			{
				m_Animator.PlayAnimation("Goalkepper");
			}

			if (input::IsKeyPressed(input::KeyCode::E))
			{
				m_Animator.PlayAnimation("Running");
			}

			if (input::IsKeyPressed(input::KeyCode::T))
			{
				m_Animator.SetBool("Bool", true);
			}
			if (input::IsKeyPressed(input::KeyCode::Y))
			{
				m_Animator.SetBool("Bool", false);;
			}

			float speed = input::GetAxis("vertical");
			m_Animator.SetFloat("Speed", speed);
		}
	};
}