
#ifndef OINI_SFML_FONT_H
#define OINI_SFML_FONT_H


#include "Globals.h"
#include "SfObject.h"

#include "SFML/Graphics.hpp"


OINI_NAMESPACE_BEGIN(Oini)

class SfFont : public sf::Font, public SfObject
{
public:
	SfFont();
	virtual ~SfFont();

public:
	virtual void Load(const std::string& fileName);

private:
	typedef sf::Font super;
};

OINI_NAMESPACE_END(Oini)


#endif //OINI_SFML_FONT_H
