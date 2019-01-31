#ifndef REALTIME_APPLICATION_H
#define REALTIME_APPLICATION_H

#include "app_handler_interface.h"

namespace md
{
	namespace core
	{
		class RealtimeApplication
		{
		public:
			RealtimeApplication(ApplicationHandlerInterface& applicationHandler);

			~RealtimeApplication();

			void Open();

			void Run();

			void Stop();

			void Close();

		private:
			ApplicationHandlerInterface& mApplicationHandler;
		};
	}
}

#endif // !REALTIME_APPLICATION_H