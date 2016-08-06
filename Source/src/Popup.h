
#ifndef OINI_POPUP_H
#define OINI_POPUP_H


#include "Globals.h"


OINI_NAMESPACE_BEGIN(Oini)

class Popup : private boost::noncopyable
{
public:
	Popup();
	virtual ~Popup();

protected:
	struct Button
	{
		Button(const string& label) : Label(label) {}
		Button(const string& label, const std::function<void()>& callback) :
			Label(label), Callback(callback) {}

		string Label;
		std::function<void()> Callback;
	};

public:
	void Initialize(const string& name, const string& message,
		bool addCloseButton);

public:
	string Name() const
		{ return mName; }
	string Message() const
		{ return mMessage; }

public:
	void AddButton(const string& label);
	void AddButton(const string& label, const std::function<void()>& callback);
	string ButtonLabel(uint index) const;
	void ButtonCallback(uint index) const;
	uint Buttons() const;

protected:
	string mName = "";
	string mMessage = "";
	std::vector<Button> mButtons;
};

OINI_NAMESPACE_END(Oini)


#endif //OINI_POPUP_H
