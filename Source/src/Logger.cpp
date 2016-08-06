
#include "Logger.h"


OINI_NAMESPACE_BEGIN(Oini)

const string LOGGING_FILE = "log.log";


Logger::Logger()
{
}

Logger::~Logger()
{
	Shutdown();
}

void Logger::Initialize()
{
	//Create a file channel assigned to a logging file.
	mpFileChannel = OINI_NEW Poco::FileChannel();
	mpFileChannel->setProperty("path", Oini::LOGGING_FILE);
	mpFileChannel->setProperty("rotation", "10 M");
	mpFileChannel->open();

	//Assign the new channel to the logger object.
	Poco::Logger::root().setChannel(mpFileChannel);
}

void Logger::Shutdown()
{
	//Free file channel resources.
	if (mpFileChannel.get() != nullptr)
	{
		mpFileChannel->close();
		mpFileChannel = nullptr;
	}
}

void Logger::Log(const string& message) const
{
	Poco::Logger::root().information(message);
}

OINI_NAMESPACE_END(Oini)
