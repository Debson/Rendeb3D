#ifndef APP_HANDLER_INTERFACE_H
#define APP_HANDLER_INTERFACE_H

#include <SDL.h>


namespace md
{
	namespace core
	{
		class ApplicationHandlerInterface
		{
		public:

			virtual ~ApplicationHandlerInterface();

			virtual void OnWindowOpen();

			virtual void OnWindowClose();

			virtual void OnNewFrame();

			virtual void OnFinishFrame();

			virtual void ProcessInput(SDL_Event *e);

			virtual void OnRealtimeUpdate();

			virtual void OnRealtimeRender();

			virtual void SetupSDLWindow(SDL_Window* win);

			//Trap mouse into window
			void SetRelativeMouseMode(SDL_bool grab);

			void FreeCursor();
		protected:
			SDL_Window *m_Window;
			
		private:
			SDL_Cursor *m_Cursor;

			

		};
	}
}
#endif // !APP_HANDLER_INTERFACE_H