
#ifndef OINI_FONT_H
#define OINI_FONT_H


#include "Globals.h"
#include "SfFont.h"
#include "Resource.h"


OINI_NAMESPACE_BEGIN(Oini)

class Font : public SfFont, public Resource, private boost::noncopyable
{
public:
	Font();
	virtual ~Font();

public:
	virtual void Load(const std::string& fileName) override;

private:
	typedef SfFont super;
};

OINI_NAMESPACE_END(Oini)


#endif //OINI_FONT_H
