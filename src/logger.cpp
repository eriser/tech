#include <tech/logger.h>


namespace Tech {


static LogMessageHandler logMessageHandler;


void setLogMessageHandler(const LogMessageHandler& handler)
{
	logMessageHandler = handler;
}


void logMessage(const char* fileName, int line, const String& message)
{
	if(!logMessageHandler.isNull())
		logMessageHandler(fileName, line, message);
}


} // namespace Tech
