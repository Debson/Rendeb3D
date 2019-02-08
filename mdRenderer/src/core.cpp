#include "core.h"

#include <iostream>

#include <glad/glad.h>
#include <gtc/matrix_transform.hpp>

#include "input.h"
#include "time.h"
#include "conf.h"
#include "gui.h"
#include "shader_manager.h"

namespace md
{
	namespace core
	{
		SDL_Window* mWindow;
		SDL_DisplayMode mWurrent;
		SDL_GLContext mContext;
		ImGuiIO mIo;

		int mdWindowID;

		const char* glsl_version = "#version 130";


		bool isRunning(false);
		bool appClosing(false);
		bool gasApplication(false);
		bool isActiveWindow(false);
		bool firstFrame(true);


		float clean_color = 1.0f;

		void SetupSDL();

		void SetupOpenGL();

		void SetupGlew();

		void OnNewFrame();

		void ProcessInput(SDL_Event *e);

		void RenderImgui();
	}

	void core::OpenRealtimeApplication(ApplicationHandlerInterface& applicationHandler)
	{
		SetupSDL();
		SetupOpenGL();
		SetupGlew();

		applicationHandler.SetupSDLWindow(mWindow);
		applicationHandler.OnWindowOpen();

		engine::shaders::Init();

		inputconf::Init();

		isRunning = true;
	}

	void core::RunRealtimeApplication(ApplicationHandlerInterface& applicationHandler)
	{
		SDL_Event event;
		f64 previousFrame = 0.0, currentFrame = 0.0;

		while (isRunning == true)
		{
			applicationHandler.OnNewFrame();
			OnNewFrame();

			if (firstFrame == false)
			{
				currentFrame = time::TimeTicks();
				time::DeltaTime = currentFrame - previousFrame;
				time::DeltaTime /= 1000.0;
				previousFrame = currentFrame;
			}
			firstFrame = false;

			applicationHandler.ProcessInput(&event);
			ProcessInput(&event);

			applicationHandler.OnRealtimeUpdate();

			applicationHandler.OnRealtimeRender();

			RenderImgui();
			SDL_GL_SwapWindow(mWindow);

			applicationHandler.OnFinishFrame();
		}
	}

	void core::StopRealtimeApplication(ApplicationHandlerInterface& applicationHandler)
	{
		isRunning = false;
	}

	void core::CloseRealtimeApplication(ApplicationHandlerInterface& applicationHandler)
	{
		applicationHandler.OnWindowClose();
		applicationHandler.FreeCursor();

		SDL_Quit();
	}

	void core::SetupSDL()
	{
		if (SDL_Init(
			SDL_INIT_VIDEO
			| SDL_INIT_EVENTS
			| SDL_INIT_TIMER
		) < 0)
		{
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SDL Error", SDL_GetError(), nullptr);
		}


		mWindow = SDL_CreateWindow("DEBSON", 
			SDL_WINDOWPOS_CENTERED, 
			SDL_WINDOWPOS_CENTERED, 
			SCREEN_WIDTH, 
			SCREEN_HEIGHT,
			SDL_WINDOW_SHOWN 
			| SDL_WINDOW_OPENGL 
			| SDL_WINDOW_RESIZABLE 
			| SDL_WINDOW_INPUT_FOCUS
			| SDL_WINDOW_MOUSE_FOCUS
			| SDL_WINDOW_MOUSE_CAPTURE
			| SDL_WINDOW_ALLOW_HIGHDPI);
		
		if (!mWindow)
		{
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Failed to open SDL window", SDL_GetError(), nullptr);
		}
	}

	void core::SetupOpenGL()
	{
		mContext = SDL_GL_CreateContext(mWindow);
		//SDL_SetRelativeMouseMode(SDL_TRUE);
		SDL_GL_MakeCurrent(mWindow, mContext);

		//SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
		//SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	}

	void core::SetupGlew()
	{
		printf("OpenGL loaded\n");
		gladLoadGLLoader(SDL_GL_GetProcAddress);
		printf("Vendor:   %s\n", glGetString(GL_VENDOR));
		printf("Renderer: %s\n", glGetString(GL_RENDERER));
		printf("Version:  %s\n", glGetString(GL_VERSION));

		glEnable(GL_DEPTH_TEST);
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_LINE_SMOOTH);
		glLineWidth(1.f);


		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

		mIo = ImGui::GetIO(); (void)mIo;
		ImGui_ImplSDL2_InitForOpenGL(mWindow, mContext);
		ImGui_ImplOpenGL3_Init(glsl_version);

		// Setup style
		ImGui::StyleColorsDark();

		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

		//SDL_GL_SetSwapInterval(0);
	}

	void core::OnNewFrame()
	{
		inputconf::StartNewFrame();

		/* imgui */
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(mWindow);
		ImGui::NewFrame();
	}

	void core::ProcessInput(SDL_Event *e)
	{
		while (SDL_PollEvent(e) != 0)
		{
			ImGui_ImplSDL2_ProcessEvent(e);

			switch (e->type)
			{
			case(SDL_QUIT): {
				isRunning = false;
				break;
			}
			case(SDL_MOUSEWHEEL): {
				inputconf::UpdateScrollPosition(e->wheel.x, e->wheel.y);
				break;
			}
			case(SDL_MOUSEMOTION): {
				inputconf::UpdateMousePosition(e->motion.x, e->motion.y);
				break;
			}
			case(SDL_KEYDOWN): {
				if (e->key.keysym.sym == SDLK_ESCAPE)
					isRunning = false;
				break;
			}
			}

			/*if (e->type == SDL_WINDOWEVENT)
			{
				switch (e->window.event)
				{
				case(SDL_WINDOWEVENT_RESIZED): {
					break;
				}
				}
			}*/
		}

		inputconf::UpdateRelativeMousePosition();

		const uint8_t *current_keystate = SDL_GetKeyboardState(NULL);
		inputconf::UpdateKeyState(current_keystate);

		const unsigned int current_mousestate = SDL_GetMouseState(NULL, NULL);
		inputconf::UpdateMouseState(current_mousestate);
	}

	void core::RenderImgui()
	{
		gui::Gui::RenderFPS();

		glViewport(0, 0, (int)mIo.DisplaySize.x, (int)mIo.DisplaySize.y);
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}