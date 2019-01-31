#ifndef CORE_H
#define CORE_H

#include "app_handler_interface.h"

namespace md
{
	namespace core
	{
		class ApplicationHandlerInterface;
	}
}

namespace md
{
	namespace core
	{
		void OpenRealtimeApplication(ApplicationHandlerInterface& applicationHandler);
		void RunRealtimeApplication(ApplicationHandlerInterface& applicationHandler);
		void StopRealtimeApplication(ApplicationHandlerInterface& applicationHandler);
		void CloseRealtimeApplication(ApplicationHandlerInterface& applicationHandler);
	}

	/*namespace Window
	{
		extern App::WindowProperties WindowProperties;
		extern App::MonitorProperties MonitorProperties;
		extern App::TrayIconProperties TrayIconProperties;

		void HideToTray();
		void BackFromTray();
		void MinimizeWindow();
		void ShowWindow();
		void HideWindow();
		void RestoreWindow();
		void MaximizeWindow();
		SDL_Window* GetSDLWindow();
		HWND GetHWNDWindow();
		SDL_GLContext* GetMainWindowContext();
		void SetWindowTitle(const b8& windowName);
		void SetWindowPos(s32 x, s32 y);
		void SetWindowSize(s32 w, s32 h);
		void GetWindowSize(s32* w, s32* h);
		glm::vec2 GetWindowSize();
		void GetWindowScale(f32* scaleX, f32* scaleY);
		void GetWindowPos(s32* x, s32* y);
		glm::vec2 GetWindowPos();
		glm::vec2 GetWindowPos(SDL_Window* window);
		void UpdateViewport(s32 w, s32 h);
		// Returns true if application height was changed do max window height, else false
		b8 CheckWindowSize();
	}*/
}

#endif // !CORE_H
