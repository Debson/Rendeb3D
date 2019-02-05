#ifndef TIMER_H
#define TIMER_H

#include "types.h"

namespace md
{
	namespace time
	{
		extern f64 DeltaTime;

		// Returns time from start of the program in sec
		f64 Time();

		f64 TimeTicks();

		class Timer
		{
		public:
			Timer();
			Timer(u32 tt);


			void Start();
			void Restart();
			void ChangeTargetTime(uint32_t newtt);
			uint32_t GetTicks();
				
			bool isFinished();
		private:
			uint32_t m_StartTicks;
			uint32_t m_TargetTime;
			bool m_Started;
			bool m_Finished;
			bool m_Stopped;
			bool m_Initialized;
		};


	}
}
#endif // !TIMER_H
