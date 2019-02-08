#include "input.h"

#include "time.h"

namespace md
{
	static bool mdNeedsInitializaiotn(true);
	static bool mdCurrentKeyStatus[input::KeyCode::count];
	static bool mdPreviousKeyStatus[input::KeyCode::count];
	static std::vector<input::axis_t> mdAxisContainer;

	static int mdCurrentMouseX, mdCurrentMouseY;
	static int mdRelativeMouseX, mdRelativeMouseY;
	static int mdPreviousMouseX, mdPreviousMouseY;

	static bool mdIsMouseScrollActive(false);
	static int mdCurrentScrollX, mdCurrentScrollY;
	static int mdPreviousScrollX, mdPreviousScrollY;
	time::Timer mdAxisTimer;
	static float mdAxisValue;
	static bool mdAxisActive;
	static float mdAxisRecenterStep;
	

	input::axis_t::axis_t() : mDead(0.0001f), mGravity(3.f), mSensitivity(3.f) { }

	bool input::IsKeyPressed(const KeyCode& key)
	{
		return (mdCurrentKeyStatus[key] == true && mdPreviousKeyStatus[key] == false) ? true : false;
	}

	bool input::IsKeyReleased(const KeyCode& key)
	{
		return (mdCurrentKeyStatus[key] == false && mdPreviousKeyStatus[key] == true) ? true : false;
	}

	bool input::IsKeyDown(const KeyCode& key)
	{
		return mdCurrentKeyStatus[key];
	}

	bool input::IsMouseActive()
	{
		return ((mdCurrentMouseX - mdPreviousMouseX) != 0 || (mdCurrentMouseY - mdPreviousMouseY) != 0) ? true : false;
	}

	bool input::IsScrollForwardActive()
	{
		return (mdIsMouseScrollActive == true && mdCurrentScrollY > 0) ? true : false;
	}

	bool input::IsScrollBackwardActive()
	{
		return (mdIsMouseScrollActive == true && mdCurrentScrollY < 0) ? true : false;
	}

	bool input::IsScrollActive()
	{
		return mdIsMouseScrollActive == true ? true : false;
	}

	float input::GetAxis(std::string const &axis)
	{
		axis_t *ax = FindAxis(axis);
		if (ax == nullptr)
			return 0.f;

		mdAxisRecenterStep = ax->mGravity * 0.01f;
		if (IsKeyDown(ax->mPositiveButton))
		{
			mdAxisActive = true;
			// Positive button pressed, reset axis value to 0;
			if (mdAxisValue < 0.f)
				mdAxisValue = 0.f;
			mdAxisValue += ax->mSensitivity * 0.01f;
			if (mdAxisValue > 1.f)
				mdAxisValue = 1.f;
		}
		else if (IsKeyDown(ax->mNegativeButton))
		{
			mdAxisActive = true;
			// Negative button pressed, reset axis value to 0;
			if (mdAxisValue > 0.f)
				mdAxisValue = 0.f;
			mdAxisValue -= ax->mSensitivity * 0.01f;
			if (mdAxisValue < -1.f)
				mdAxisValue = -1.f;
		}
		else
		{
			mdAxisActive = false;
		}

		return mdAxisValue;
	}

	void input::AddAxis(const axis_t &axis)
	{
		mdAxisContainer.push_back(axis);
	}

	input::axis_t *input::FindAxis(std::string const &axis)
	{
		size_t size = mdAxisContainer.size();
		for (size_t i = 0; i < size; i++)
		{
			if (mdAxisContainer[i].mName == axis)
				return &mdAxisContainer[i];
		}

		return nullptr;
	}

	glm::vec2 input::GetMousePosition()
	{
		return glm::vec2(mdCurrentMouseX, mdCurrentMouseY);
	}

	void input::GetMousePosition(int *x, int *y)
	{
		*x = mdCurrentMouseX;
		*y = mdCurrentMouseY;
	}

	glm::vec2 input::GetGlobalMousePosition()
	{
		int x, y;
		SDL_GetGlobalMouseState(&x, &y);
		return glm::vec2(x, y);
	}

	void input::GetGlobalMousePosition(int *x, int *y)
	{
		SDL_GetGlobalMouseState(x, y);
	}

	glm::vec2 input::GetRelativeMousePosition()
	{
		return glm::vec2(mdRelativeMouseX, mdRelativeMouseY);
	}
	
	void input::GetRelativeMousePosition(int *x, int *y)
	{
		*x = mdRelativeMouseX;
		*y = mdRelativeMouseY;
	}

	/* ******* INPUT CONFIGURATION ******** */

	void inputconf::Init()
	{
		input::axis_t vertical;
		vertical.mName = "vertical";
		vertical.mPositiveButton = input::KeyCode::W;
		vertical.mNegativeButton = input::KeyCode::S;
		input::AddAxis(vertical);

		input::axis_t horizontal;
		horizontal.mName = "horizontal";
		horizontal.mPositiveButton = input::KeyCode::D;
		horizontal.mNegativeButton = input::KeyCode::A;
		input::AddAxis(horizontal);
	}

	void inputconf::OnPressKey(const input::KeyCode& key)
	{
		mdCurrentKeyStatus[key] = true;
	}

	void inputconf::OnReleaseKey(const input::KeyCode& key)
	{
		mdCurrentKeyStatus[key] = false;
	}

	void inputconf::StartNewFrame(void)
	{
		if (mdNeedsInitializaiotn == true)
		{
			mdNeedsInitializaiotn = false;
			for (uint8_t keyIndex = 0; keyIndex < input::KeyCode::count; ++keyIndex)
			{
				mdCurrentKeyStatus[keyIndex] = false;
			}
		}

		for (uint8_t keyIndex = 0; keyIndex < input::KeyCode::count; ++keyIndex)
		{
			mdPreviousKeyStatus[keyIndex] = mdCurrentKeyStatus[keyIndex];
		}

		mdPreviousMouseX = mdCurrentMouseX;
		mdPreviousMouseY = mdCurrentMouseY;

		mdPreviousScrollX = mdCurrentScrollX;
		mdPreviousScrollY = mdCurrentScrollY;
		mdIsMouseScrollActive = false;

		// Reset axis value to 0.f
		if (mdAxisActive == false && mdAxisValue != 0.f) 
		{
			if (mdAxisValue > 0.f && mdAxisValue - mdAxisRecenterStep > 0.f)
			{
				mdAxisValue -= mdAxisRecenterStep;
			}
			else if (mdAxisValue < 0.f && mdAxisValue + mdAxisRecenterStep < 0.f)
			{
				mdAxisValue += mdAxisRecenterStep;
			}
			else
			{
				mdAxisValue = 0.f;
			}
		}
	}

	void inputconf::UpdateRelativeMousePosition()
	{
		SDL_GetRelativeMouseState(&mdRelativeMouseX, &mdRelativeMouseY);
	}

	void inputconf::UpdateMousePosition(int mouseX, int mouseY)
	{
		mdCurrentMouseX = mouseX;
		mdCurrentMouseY = mouseY;
	}

	void inputconf::UpdateScrollPosition(int scrollX, int scrollY)
	{
		mdCurrentScrollX = scrollX;
		mdCurrentScrollY = scrollY;
		mdIsMouseScrollActive = true;
	}

	void inputconf::UpdateKeyState(const uint8_t* state)
	{
		for (uint8_t keyIndex = 0; keyIndex < input::KeyCode::count; ++keyIndex)
		{
			mdCurrentKeyStatus[keyIndex] = state[keyIndex];
		}
	}

	void inputconf::UpdateMouseState(const uint8_t state)
	{
		mdCurrentKeyStatus[input::KeyCode::MouseLeft] = state & SDL_BUTTON(SDL_BUTTON_LEFT);
		mdCurrentKeyStatus[input::KeyCode::MouseRight] = state & SDL_BUTTON(SDL_BUTTON_RIGHT);
		mdCurrentKeyStatus[input::KeyCode::MouseMiddle] = state & SDL_BUTTON(SDL_BUTTON_MIDDLE);
	}
}