
#ifndef OINI_SFML_WINDOW_H
#define OINI_SFML_WINDOW_H


#include "Globals.h"
#include "SfObject.h"

#include "SFML/Graphics.hpp"


OINI_NAMESPACE_BEGIN(Oini)

class SfWindow : public sf::RenderWindow, public SfObject
{
public:
	SfWindow();
	virtual ~SfWindow();

protected:
	virtual void Create(uint width, uint height, const string& title);
	void LimitFramerate(uint framerate);
	void Deactivate();
	void Move(uint pixelX, uint pixelY);

private:
	typedef sf::RenderWindow super;
};

OINI_NAMESPACE_END(Oini)


#endif //OINI_SFML_WINDOW_H
