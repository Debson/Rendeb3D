#include "debug.h"

#include "types.h"

namespace md
{
	namespace debug
	{
		std::stringstream mdPrevLog;
		std::stringstream mdCurrLog;
		bool mdDisplayLog(false);
	}

	void debug::conf::process()
	{
		if (mdDisplayLog)
		{
			std::cout << mdCurrLog.str() << std::endl;
			mdDisplayLog = false;
		}

	}
}