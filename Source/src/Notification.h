
#ifndef OINI_NOTIFICATION_H
#define OINI_NOTIFICATION_H


#include "Globals.h"

#include "Poco/Notification.h"


OINI_NAMESPACE_BEGIN(Oini)

class Notification : public Poco::Notification, private boost::noncopyable
{
public:
	enum class Type : uint8
	{
		EXIT,
		FILE_OPEN,
		LOG_OPEN
	};

public:
	Notification(Type type);
	virtual ~Notification();

public:
	bool IsType(Type type) const;

protected:
	Type mType;

private:
	typedef Poco::Notification super;
};

OINI_NAMESPACE_END(Oini)


#endif //OINI_NOTIFICATION_H
