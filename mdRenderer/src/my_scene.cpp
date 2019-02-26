#include "my_scene.h"

#include <iostream>

#include "md_imgui_parser.h"

namespace md
{
	engine::GameObject *myModel;
	engine::Animator m_Animator;
}

md::Scene::~Scene()
{
	//delete myModel;
}

void md::Scene::OnWindowOpen()
{
	m_DebugMode = false;
	SetRelativeMouseMode(SDL_TRUE);
	m_Camera = mdGraphics::Camera(glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT), glm::vec3(0.f, 0.f, 3.f));
	mdGraphics::Renderer::SetCamera(&m_Camera);
	this->SetClearColor(math::Color4(0.95f, 0.95f, 0.7f, 1.f));

	myModel = engine::GameObject::LoadModel("Model", "assets//Idle.fbx");

	myModel->AddComponent<engine::Animator>(myModel);

	m_Animator = myModel->GetComponent<engine::Animator>();


	//myModel = new engine::GameObject("face", "assets//Idle.fbx");
	
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

void md::Scene::OnWindowClose()
{
	
}

void md::Scene::OnRealtimeUpdate()
{
	if (interface::ProcessUserDebugInput(&m_DebugMode))
	{
		SetRelativeMouseMode((SDL_bool)!m_DebugMode);
	}

	if (m_DebugMode == false)
	{
		interface::ProcessCameraInput(m_Camera);
	}

	UpdateScene();
}

void md::Scene::OnRealtimeRender()
{
	m_Gui.RenderGUI();
	m_Camera.RenderGUI();

	RenderScene();
}

void md::Scene::OnNewFrame()
{

}

void md::Scene::OnFinishFrame()
{

}

void md::Scene::ProcessInput(SDL_Event *e)
{
	
}

void md::Scene::RenderScene()
{
	/*for (auto & i : myGameObj)
	{
		i.Render(&m_DefaultShader);
	}*/
	


	//myModel.Render(mdShaders::Model());
	//auto & graphics = myModel->GetComponent<engine::Graphics>();
	//graphics.Render(mdShaders::Model());
}

void md::Scene::UpdateScene()
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


int main(int argc, char *args[])
{
	md::Scene scene;
	md::core::RealtimeApplication app(scene);

	app.Open();
	app.Run();
	app.Close();

	return 0;
}
