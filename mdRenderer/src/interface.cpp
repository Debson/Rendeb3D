#include "interface.h"

#include <iostream>

#include "input.h"
#include "time.h"
#include "gui.h"

namespace md
{
	namespace interface
	{
		b8 renderGui = true;
		f32 shiftSpeed = 15.f;
	};

	void interface::ProcessCameraInput(engine::Camera *cam)
	{

		if (input::IsKeyDown(input::KeyCode::W) && input::IsKeyDown(input::KeyCode::LCtrl))
		{
			if (input::IsKeyDown(input::KeyCode::LShift))
				cam->ProcessKeyboard(engine::CameraMovement::Forward, time::DeltaTime, shiftSpeed);
			else
				cam->ProcessKeyboard(engine::CameraMovement::Forward, time::DeltaTime);
		}
		if (input::IsKeyDown(input::KeyCode::S))
		{
			if (input::IsKeyDown(input::KeyCode::LShift))
				cam->ProcessKeyboard(engine::CameraMovement::Backward, time::DeltaTime, shiftSpeed);
			else
				cam->ProcessKeyboard(engine::CameraMovement::Backward, time::DeltaTime);
		}
		if (input::IsKeyDown(input::KeyCode::A))
		{
			if (input::IsKeyDown(input::KeyCode::LShift))
				cam->ProcessKeyboard(engine::CameraMovement::Left, time::DeltaTime, shiftSpeed);
			else
				cam->ProcessKeyboard(engine::CameraMovement::Left, time::DeltaTime);
		}
		if (input::IsKeyDown(input::KeyCode::D))
		{
			if (input::IsKeyDown(input::KeyCode::LShift))
				cam->ProcessKeyboard(engine::CameraMovement::Right, time::DeltaTime, shiftSpeed);
			else
				cam->ProcessKeyboard(engine::CameraMovement::Right, time::DeltaTime);
		}

		s32 x, y;
		input::GetRelativeMousePosition(&x, &y);
		cam->ProcessMouseMovement(x, y);
	}

	bool interface::ProcessUserDebugInput(b8 *debug)
	{
		b8 debugEnabled = false;
		if (input::IsKeyPressed(input::KeyCode::F11))
		{
			*debug = !*debug;
			debugEnabled = true;
		}

		if (input::IsKeyPressed(input::KeyCode::F10))
		{
			renderGui = !renderGui;
			engine::Gui::SetRenderGUI(renderGui);
		}

		return debugEnabled;
	}
}