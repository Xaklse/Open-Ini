
#ifndef OINI_SFML_OBJECT_H
#define OINI_SFML_OBJECT_H


#include "Globals.h"


OINI_NAMESPACE_BEGIN(Oini)

class SfObject
{
public:
	SfObject();
	virtual ~SfObject();

protected:
	void RedirectSfError() const;
	void ThrowSfError();
	void LogSfError();

private:
	bool IsSfErrorDetected();
	string mErrorMessage = "";
	std::ostringstream mSfErrorMessage;
};

OINI_NAMESPACE_END(Oini)


#endif //OINI_SFML_OBJECT_H
