
#include "Font.h"
#include "Exception.h"


OINI_NAMESPACE_BEGIN(Oini)

Font::Font() : SfFont()
{
}

Font::~Font()
{
}

/*virtual*/ void Font::Load(const std::string& fileName)
{
	super::Load(fileName);

	Identifier(fileName);
}

OINI_NAMESPACE_END(Oini)
