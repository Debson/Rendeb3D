#include "realtime_app.h"

#include "core.h"


namespace md
{
		core::RealtimeApplication::RealtimeApplication(ApplicationHandlerInterface& applicationHandler) : mApplicationHandler(applicationHandler)
		{

			// add implementation??
		}

		core::RealtimeApplication::~RealtimeApplication()
		{

		}

		void core::RealtimeApplication::Open()
		{
			OpenRealtimeApplication(mApplicationHandler);
		}

		void core::RealtimeApplication::Run()
		{
			RunRealtimeApplication(mApplicationHandler);
		}

		void core::RealtimeApplication::Stop()
		{
			StopRealtimeApplication(mApplicationHandler);
		}

		void core::RealtimeApplication::Close()
		{
			CloseRealtimeApplication(mApplicationHandler);
		}		

	
}