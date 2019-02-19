#include "debug.h"

//#if defined(tb_windows)
#include <io.h>
#include <fcntl.h>
#include <cstdio>
//#endif /* tb_windows */
/*
#if defined(tb_linux)
#include <cstdarg>
#endif */ /* tb_linux */



#include "types.h"


namespace md
{
	namespace debug
	{
		std::string mdPrevLog("");
		std::string mdCurrLog("");
		bool mdDisplayLog(false);
		int mdErrorFiredOnLineNumber(-1);
		std::string mdErrorFiredInFileNamed("");


		void DoLogging(const char* formattedString, va_list variableArguments);
		
		void OnErrorFired(const std::string& fromFile, int onLineNumber, const std::string& errorMessage);
	}

	void debug::DoLogging(const char* formattedString, va_list variableArguments)
	{
		const size_t kLogBufferSize(8192);
		char buffer[kLogBufferSize] = { '\0', };

		vsprintf_s(buffer, kLogBufferSize, formattedString, variableArguments);

		// Implement Unity style log(don't print lines that repeats)
		std::string logMessage(buffer);
		
		printf("%s\n", logMessage.c_str());
	}

	void debug::OnErrorFired(const std::string& fromFile, int onLineNumber, const std::string& errorMessage)
	{
		mdErrorFiredOnLineNumber = onLineNumber;
		mdErrorFiredInFileNamed = fromFile;

		log("\n\n----- Error Condition Triggered -----\n");
		log("FILE: %s[%d]\n", fromFile.c_str(), onLineNumber);
		log("MESSAGE: %s\n", errorMessage.c_str());
		log("----- Error Condition Triggered -----\n");
		
		assert(-1);
	}

	void debug::log(const char* formattedMessage, ...)
	{
		va_list argList;
		va_start(argList, formattedMessage);
		DoLogging(formattedMessage, argList);
		va_end(argList);
	}

	void debug::error::OnTriggerError(const char* const fromFile, int onLineNumber, const char* const formattedString, va_list variableArguments)
	{
		const size_t kErrorBufferSize(4096);
		char buffer[kErrorBufferSize] = { '\0', };

		vsprintf_s(buffer, kErrorBufferSize, formattedString, variableArguments);
		va_end(variableArguments);

		const std::string errorMessage(buffer);
		debug::OnErrorFired(fromFile, onLineNumber, errorMessage);
	}

	void debug::error::TriggerError(const char* const fromFile, int onLineNumber, const char* const formattedString, ...)
	{
		va_list argumentsList;
		va_start(argumentsList, formattedString);
		OnTriggerError(fromFile, onLineNumber, formattedString, argumentsList);
		va_end(argumentsList);
	}

	void debug::conf::process()
	{
		if (mdDisplayLog)
		{
			//std::cout << mdCurrLog.str() << std::endl;
			//mdDisplayLog = false;
		}

	}
}