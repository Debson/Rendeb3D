#ifndef MY_SCENE_H
#define MY_SCENE_H

//#include "md_renderer_kit.h"

#include "app_handler_interface.h"
#include "types.h"


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

		b8 m_DebugMode;
	};
}

#endif // MY_SCENE_H
