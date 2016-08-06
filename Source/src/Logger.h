
#ifndef OINI_LOGGER_H
#define OINI_LOGGER_H


#include "Globals.h"

#include "Poco/AutoPtr.h"
#include "Poco/FileChannel.h"
#include "Poco/Logger.h"


OINI_NAMESPACE_BEGIN(Oini)

class Logger
{
public:
	Logger();
	virtual ~Logger();

public:
	void Initialize();
	void Shutdown();

public:
	void Log(const string& message) const;

protected:
	Poco::AutoPtr<Poco::FileChannel> mpFileChannel;
};

OINI_NAMESPACE_END(Oini)


#endif //OINI_LOGGER_H
