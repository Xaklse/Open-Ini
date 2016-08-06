
#ifndef OINI_WINDOW_H
#define OINI_WINDOW_H


#include "Globals.h"
#include "SfWindow.h"

#include "Poco/NotificationQueue.h"


OINI_NAMESPACE_BEGIN(Oini)

class Window : public SfWindow, private boost::noncopyable
{
public:
	Window();
	virtual ~Window();

public:
	virtual void Initialize();
	virtual bool Run();
	virtual void Shutdown();

public:
	virtual void DrawGraphics() {}
	virtual void HandleEvent(sf::Event event) {}
	virtual void HandleNotification(class Notification* pNotification) {}

public:
	void Close();

protected:
	virtual void Create(uint width, uint height, const string& title) override;

protected:
	void ForceRedraw(bool forceRedraw);

protected:
	bool mKeepOpen = true;
	uint mRedrawGraphics = 0u;
	sf::Color mBackgroundColor = sf::Color::Black;
	Poco::NotificationQueue mNotifications;
	std::vector<std::shared_ptr<class Popup>> mpPopups;

private:
	typedef SfWindow super;
};

OINI_NAMESPACE_END(Oini)


#endif //OINI_WINDOW_H
