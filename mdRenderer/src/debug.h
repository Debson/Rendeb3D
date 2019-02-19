#ifndef MD_DEBUG_H
#define MD_DEBUG_H

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdarg.h>

#define md_error(message, ...) { md::debug::error::TriggerError(__FILE__, __LINE__, message, ##__VA_ARGS__); }
#define md_log(message, ...) { md::debug::log(message, ##__VA_ARGS__); }

namespace md
{
	namespace debug
	{
		void log(const char* formattedMessage, ...);

		namespace error
		{
			void OnTriggerError(const char* const fromFile, int onLineNumber, const char* const formattedString, va_list variableArguments);
			void TriggerError(const char* const fromFile, int onLineNumber, const char* const formattedString, ...);
		}

		namespace conf
		{
			void process();
		}
	}
}

#endif // !MD_DEBUG_H
