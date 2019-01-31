#ifndef MY_SCENE_H
#define MY_SCENE_H

#include "camera.h"
#include "app_handler_interface.h"
#include "objects.h"
#include "animator.h"

namespace md
{
	class Scene : public core::ApplicationHandlerInterface
	{
	public:
		~Scene();
		void OnWindowOpen();
		void OnWindowClose();
		void OnRealtimeUpdate();
		void OnRealtimeRender();

	private:
		void OnNewFrame();
		void OnFinishFrame();
		void ProcessInput(SDL_Event *e);
		void RenderScene();
		void UpdateScene();

		mdGraphics::Shader m_DefaultShader;
		gui::Gui m_Gui;
		mdGraphics::Camera m_Camera;
		engine::Animator m_Animator;

		engine::GameObject *myModel;

		b8 m_DebugMode;
	};
}

#endif // MY_SCENE_H
