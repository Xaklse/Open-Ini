
#include "SfWindow.h"


OINI_NAMESPACE_BEGIN(Oini)

SfWindow::SfWindow()
{
}

SfWindow::~SfWindow()
{
}

/*virtual*/ void SfWindow::Create(uint width, uint height, const string& title)
{
	RedirectSfError();

	//Create the actual window.
	create(sf::VideoMode(width, height), sf::String(title), sf::Style::Default);

	ThrowSfError();
}

void SfWindow::LimitFramerate(uint framerate)
{
	RedirectSfError();

	//Limit frame-rate to a fixed amount of FPS.
	setFramerateLimit(framerate);

	LogSfError();
}

void SfWindow::Deactivate()
{
	RedirectSfError();

	//Deactivate the window as the current target for rendering.
	setActive(false);

	ThrowSfError();
}

void SfWindow::Move(uint pixelX, uint pixelY)
{
	RedirectSfError();

	//Change the position of the window.
	setPosition(sf::Vector2i(pixelX, pixelY));

	LogSfError();
}

OINI_NAMESPACE_END(Oini)
