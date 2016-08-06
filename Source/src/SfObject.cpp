
#include "SfObject.h"
#include "Application.h"
#include "Exception.h"

#include "SFML/System.hpp"


OINI_NAMESPACE_BEGIN(Oini)

SfObject::SfObject()
{
}

SfObject::~SfObject()
{
}

void SfObject::RedirectSfError() const
{
	//Redirect SFML errors/warnings to a special stream.
	sf::err().rdbuf(mSfErrorMessage.rdbuf());
}

void SfObject::ThrowSfError()
{
	if (IsSfErrorDetected())
	{
		throw Exception(mErrorMessage);
	}
}

void SfObject::LogSfError()
{
	if (IsSfErrorDetected())
	{
		OINI_LOG(mErrorMessage);
	}
}

bool SfObject::IsSfErrorDetected()
{
	//Read the special stream.
	mErrorMessage = mSfErrorMessage.str();

	if (mErrorMessage.length() == 0)
	{
		return false;
	}

	//Empty the special stream.
	mSfErrorMessage.str("");

	//Remove trailing whitespace.
	Poco::trimRightInPlace(mErrorMessage);

	return true;
}

OINI_NAMESPACE_END(Oini)
