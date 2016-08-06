
#include "Window.h"
#include "Application.h"
#include "Notification.h"


OINI_NAMESPACE_BEGIN(Oini)

const uint REDRAW_FRAMES = 10u;


Window::Window() : SfWindow()
{
}

Window::~Window()
{
	if (isOpen())
	{
		Shutdown();
	}
}

/*virtual*/ void Window::Initialize()
{
	Create(800u, 600u, "Title");
	ForceRedraw(true);
}

/*virtual*/ bool Window::Run()
{
	//Handle window events.
	sf::Event event;
	while (pollEvent(event))
	{
		//Catch the window close event.
		if (event.type == sf::Event::Closed)
		{
			mKeepOpen = false;
		}
		else
		{
			//Catch the window resize event.
			if (event.type == sf::Event::Resized)
			{
				OINI_LOG("Resizing rendering area to " + Str(event.size.width) +
					"x" + Str(event.size.height) + "...");

				//Resize the active 2D view.
				sf::FloatRect renderArea(0.0f, 0.0f,
					static_cast<float>(event.size.width),
					static_cast<float>(event.size.height));
				setView(sf::View(renderArea));
			}

			//Pass event.
			HandleEvent(event);
		}

		//Request graphics to be redrawn.
		ForceRedraw(true);
	}

	//Handle internal events.
	while (!mNotifications.empty())
	{
		//Dequeue the next pending notification.
		Poco::AutoPtr<Poco::Notification> pNotification(
			mNotifications.dequeueNotification());
		auto pNotification2 = dynamic_cast<Notification*>(pNotification.get());

		if (pNotification2 != nullptr)
		{
			//Catch the exit program notification.
			if (pNotification2->IsType(Notification::Type::EXIT))
			{
				mKeepOpen = false;
			}
			else
			{
				//Pass notification.
				HandleNotification(pNotification2);
			}
		}

		//Request graphics to be redrawn.
		ForceRedraw(true);
	}

	if (mRedrawGraphics > 0u)
	{
		--mRedrawGraphics;

		//Activate the window as the current target for rendering.
		setActive(true);

		//Clear the window with a solid color.
		clear(mBackgroundColor);

		//Draw stuff.
		DrawGraphics();

		//Update the window.
		display();
	}

	return mKeepOpen;
}

/*virtual*/ void Window::Shutdown()
{
	//Clear the notifications queue.
	mNotifications.clear();
	mNotifications.wakeUpAll();

	//Release resources.
	close();
}

void Window::Close()
{
	mKeepOpen = false;
}

/*virtual*/ void Window::Create(uint width, uint height, const string& title)
{
	//Avoid creation of a window with unreasonable size.
	width = boost::algorithm::clamp(width, 200u,
		sf::VideoMode::getDesktopMode().width - 50u);
	height = boost::algorithm::clamp(height, 200u,
		sf::VideoMode::getDesktopMode().height - 50u);

	//Create the actual window.
	super::Create(width, height, title);

	//Limit frame-rate to 60 FPS.
	LimitFramerate(60u);

	//Deactivate the window as the current target for rendering.
	Deactivate();
}

void Window::ForceRedraw(bool forceRedraw)
{
	mRedrawGraphics = forceRedraw ? REDRAW_FRAMES : 0u;
}

OINI_NAMESPACE_END(Oini)
