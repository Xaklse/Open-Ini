
#include "SfFont.h"


OINI_NAMESPACE_BEGIN(Oini)

SfFont::SfFont() : sf::Font()
{
}

SfFont::~SfFont()
{
}

/*virtual*/ void SfFont::Load(const std::string& fileName)
{
	RedirectSfError();

	//Load the font from the file.
	loadFromFile(fileName);

	ThrowSfError();
}

OINI_NAMESPACE_END(Oini)
