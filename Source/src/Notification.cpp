
#include "Notification.h"


OINI_NAMESPACE_BEGIN(Oini)

Notification::Notification(Type type) : Poco::Notification(), mType(type)
{
}

Notification::~Notification()
{
}

bool Notification::IsType(Type type) const
{
	return type == mType;
}

OINI_NAMESPACE_END(Oini)
