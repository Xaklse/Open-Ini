
#ifndef OINI_RESOURCE_H
#define OINI_RESOURCE_H


#include "Globals.h"


OINI_NAMESPACE_BEGIN(Oini)

class Resource
{
public:
	Resource();
	virtual ~Resource();

public:
	string Identifier() const
		{ return mIdentifier; }
	void Identifier(const string& identifier)
		{ mIdentifier = identifier; }

protected:
	string mIdentifier = "";
};

OINI_NAMESPACE_END(Oini)


#endif //OINI_RESOURCE_H
