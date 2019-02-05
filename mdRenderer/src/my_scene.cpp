#include "my_scene.h"

#include <iostream>


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
	myModel = new engine::GameObject("face", "assets//dance.fbx");
	m_Animator = engine::Animator(myModel);
	m_Animator.AddAnimation("Goalkepper", "assets//Goalkeeper.fbx");
	m_Animator.AddAnimation("Header", "assets//Header.fbx");
	m_Animator.AddTransition("Start", "Header");
	m_Animator.AddTransition("Header", "Start");

	m_Animator.PlayAnimation("Header");


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
		m_Animator.PlayAnimation("Header");
	}
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
