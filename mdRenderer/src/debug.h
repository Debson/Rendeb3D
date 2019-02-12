#ifndef MD_DEBUG_H
#define MD_DEBUG_H

#include <iostream>
#include <sstream>

namespace md
{
	namespace debug
	{
		extern std::stringstream mdPrevLog;
		extern std::stringstream mdCurrLog;
		extern bool mdDisplayLog;

		template <typename T>
		void log(T out, bool showRepeatable = false)
		{
			mdCurrLog.str(std::string());
			mdCurrLog << out << std::flush;
			if (showRepeatable)
				mdDisplayLog = true;
			else if (mdPrevLog.str() != mdCurrLog.str())
			{
				mdDisplayLog = true;
				mdPrevLog.str(std::string());
				mdPrevLog << mdCurrLog.rdbuf() << std::flush;
			}
		}

		namespace conf
		{
			void process();
		}
	}
}

#endif // !MD_DEBUG_H
