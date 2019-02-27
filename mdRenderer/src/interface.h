#ifndef INTERFACE_H
#define INTERFACE_H

#include "camera.h"
#include "types.h"

namespace md
{
	namespace interface
	{

		void ProcessCameraInput(engine::Camera &cam);

		bool ProcessUserDebugInput(b8 *debug);


	};
}

#endif // !INTERFACE_H
