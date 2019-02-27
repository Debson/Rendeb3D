#include "my_scene.h"

#include <iostream>

#include "script_list.h"
#include "scriptable.h"
#include "conf.h"
#include "interface.h"
#include "realtime_app.h"
#include "environment.h"

namespace md
{
	std::vector<std::unique_ptr<Script>> mdScriptsContainer;

	engine::Gui gui;
	engine::GameObject *cameraObj;
	engine::Camera cam;
}

md::Scene::~Scene()
{
	
}

void md::Scene::OnWindowOpen()
{
	// Setup Scene
	m_DebugMode = false;
	SetRelativeMouseMode(SDL_TRUE);
	this->SetClearColor(math::Color4(0.95f, 0.95f, 0.7f, 1.f));
	
	cameraObj = engine::GameObject::Create("Camera");
	cameraObj->AddComponent<engine::Camera>(glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT), glm::vec3(0.f, 0.f, 3.f));
	cam = cameraObj->GetComponent<engine::Camera>();
	mdGraphics::Renderer::SetCamera(&cam);
	
	std::vector<std::string> faces =
	{
		"assets//skybox//right.jpg",
		"assets//skybox//left.jpg",
		"assets//skybox//top.jpg",
		"assets//skybox//bottom.jpg",
		"assets//skybox//front.jpg",
		"assets//skybox//back.jpg",
	};
	cameraObj->AddComponent<environment::Skybox>(faces);


	// Setup scripts
	mdScriptsContainer.emplace_back(std::make_unique<SkeletalTests>());

	// Execute scripts
	for (auto && script : mdScriptsContainer)
	{
		script->Start();
	}
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
		interface::ProcessCameraInput(cam);
	}

	UpdateScene();
}

void md::Scene::OnRealtimeRender()
{
	gui.RenderGUI();
	cam.RenderGUI();

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
	
}

void md::Scene::UpdateScene()
{
	for (auto && script : mdScriptsContainer)
	{
		script->Update();
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
