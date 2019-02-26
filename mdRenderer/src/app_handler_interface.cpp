#include "app_handler_interface.h"


namespace md
{

	core::ApplicationHandlerInterface::~ApplicationHandlerInterface() { }

	void core::ApplicationHandlerInterface::OnWindowOpen() { }

	void core::ApplicationHandlerInterface::OnWindowClose() { }

	void core::ApplicationHandlerInterface::OnNewFrame() { }

	void core::ApplicationHandlerInterface::OnFinishFrame() { }

	void core::ApplicationHandlerInterface::ProcessInput(SDL_Event *e) { }

	void core::ApplicationHandlerInterface::OnRealtimeUpdate() { }

	void core::ApplicationHandlerInterface::OnRealtimeRender() { }

	void core::ApplicationHandlerInterface::SetupSDLWindow(SDL_Window *window) { m_Window = window; }

	void core::ApplicationHandlerInterface::SetRelativeMouseMode(SDL_bool grab)
	{
		SDL_SetRelativeMouseMode(grab);
	}

	void core::ApplicationHandlerInterface::SetClearColor(math::Color4 color)
	{
		m_ClearColor = color;
	}

	math::Color4 &core::ApplicationHandlerInterface::GetClearColor()
	{
		return m_ClearColor;
	}

	void core::ApplicationHandlerInterface::FreeCursor()
	{
		SDL_FreeCursor(m_Cursor);
	}
}