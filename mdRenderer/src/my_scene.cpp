#include "my_scene.h"

#include <iostream>

#include "md_imgui_parser.h"


md::Scene::~Scene()
{
	delete myModel;
}

void md::Scene::OnWindowOpen()
{
	m_DebugMode = false;
	SetRelativeMouseMode(SDL_TRUE);
	m_Camera = mdGraphics::Camera(glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT), glm::vec3(0.f, 0.f, 3.f));
	mdGraphics::Renderer::SetCamera(&m_Camera);
	/*s32 index = 0;
	for (auto & i : myGameObj)
	{
		i = engine::GameObject("debson" + std::to_string(index), mdGraphics::Type::tCube);
		i.graphics.ApplyTexture("assets//pic.jpg");
		i.transform.position.x = 2.1f * index;
		index++;
	}


	myGameObj[0].transform.Rotate(glm::vec3(1.f, 0.f, 1.f), 45.f);
	myGameObj[3].transform.Rotate(glm::vec3(0.f, 1.f, 0.f), 45.f);*/

	//myModel = new engine::GameObject("face", "assets//xbot@Running.fbx");
	myModel = new engine::GameObject("face", "assets//Idle.fbx");
	m_Animator = engine::Animator(myModel);
	m_Animator.AddAnimation("Goalkepper", "assets//Goalkeeper.fbx");
	m_Animator.AddAnimation("Header", "assets//Header.fbx");
	m_Animator.AddAnimation("Running", "assets//Running.fbx");
	
	m_Animator.AddTransition("Start", "Header", 10.f, engine::graphics::TransitionType::BILATERAL);
	//m_Animator.AddTransition("Goalkepper", "Start", 10.f, engine::graphics::TransitionType::ONESIDED);
	m_Animator.AddTransition("Start", "Goalkepper", 10.f, engine::graphics::TransitionType::ONESIDED);
	m_Animator.AddTransition("Start", "Running", 10.f, engine::graphics::TransitionType::BILATERAL);

	//m_Animator.PlayAnimation("Header");
	m_Animator.AddParameter("Speed");
	m_Animator.AddParameter("Trigger");
	m_Animator.AddParameter("Bool");

	m_Animator.AddTransitionCondition("Start", "Running", "Speed", MD_GREATER, 0.2f);
	m_Animator.AddTransitionCondition("Running", "Start", "Speed", MD_LESS, 0.2f);

	m_Animator.SetTransitionExitTimeState("Start", "Running", false);
	
	m_Animator.SetTransitionExitTimeState("Goalkepper", "Start", false);

	m_Animator.AddTransitionCondition("Start", "Running", "Speed", MD_GREATER, 0.2f);;
		
	/*m_Animator.AddTransitionCondition("Start", "Running", "Bool", MD_TRUE, true);
	m_Animator.AddTransitionCondition("Running", "Start", "Bool", MD_FALSE, false);*/


	//myModel->AddAnimation("test2", "assets//Header.fbx");
	//myModel = new engine::GameObject("face", "assets//Header.dae");
	//myModel = engine::GameObject("nanosuit", "assets//skeletal//skeletal.md5mesh");
	//myModel = engine::GameObject("cube", engine::graphics::Type::tCube);
	myModel->transform.position.x = 0.f;
	myModel->transform.localScale = glm::vec3(0.01f);
	//myModel->transform.Rotate(-90.f, 0.f, 0.f);

	//myModel = engine::GameObject("rabbit", "assets//skeletal//skeletal.md5mesh");

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
	glClearColor(1.f, 1.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
	
	myModel->Render(mdShaders::Model());

}

void md::Scene::UpdateScene()
{
	/*myGameObj[3].transform.Rotate(glm::vec3(1.f, 0.f, 0.f), 0);
	myGameObj[1].transform.Rotate(0, 0.f, time::DeltaTime * 60.f);*/
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
	//std::cout << speed << std::endl;

	// Parser tests
	/*ImGui::Config::BeginConfig("_DEBUG_");
	ImGui::Config::SaveFloat("test", 15.f);
	ImGui::Config::SaveFloat("lol", 15.f);
	ImGui::Config::SaveVec2("xd", ImVec2(24.f, 1.f));
	float test = 0.f;
	ImVec2 vec;
	ImGui::Config::ReadFloat("test", &test);
	ImGui::Config::ReadVec2("xd", &vec);
	
	ImGui::Config::EndConfig();

	ImGui::Config::UpdateConfig();*/

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
