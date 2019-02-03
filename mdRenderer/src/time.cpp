#include "time.h"

#include <SDL.h>

namespace md
{
	namespace time
	{
		f64 DeltaTime;
	}

	f64 time::CurrentTime()
	{
		return SDL_GetTicks() / 1000.0;
	}


	time::Timer::Timer() : m_StartTicks(0), m_TargetTime(0), m_Started(false), m_Finished(false), m_Initialized(false) { };

	time::Timer::Timer(uint32_t tt) : m_StartTicks(0), m_TargetTime(tt), m_Started(false), m_Finished(false), m_Initialized(false) { }

	void time::Timer::Start()
	{
		m_Started = true;
		m_StartTicks = SDL_GetTicks();
	}

	void time::Timer::Restart()
	{
		m_StartTicks = SDL_GetTicks();
	}

	void time::Timer::ChangeTargetTime(uint32_t newtt)
	{
		m_TargetTime = newtt;
	}

	uint32_t time::Timer::GetTicks()
	{
		return SDL_GetTicks() - m_StartTicks;
	}

	bool time::Timer::isFinished()
	{
		return (SDL_GetTicks() - m_StartTicks > m_TargetTime);
	}
}