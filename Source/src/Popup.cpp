
#include "Popup.h"


OINI_NAMESPACE_BEGIN(Oini)

Popup::Popup()
{
}

Popup::~Popup()
{
}

void Popup::Initialize(const string& name, const string& message,
	bool addCloseButton)
{
	mName = name;
	mMessage = message;

	if (addCloseButton)
	{
		AddButton("OK");
	}
}

void Popup::AddButton(const string& label)
{
	mButtons.push_back(Button(label));
}

void Popup::AddButton(const string& label,
	const std::function<void()>& callback)
{
	mButtons.push_back(Button(label, callback));
}

string Popup::ButtonLabel(uint index) const
{
	return mButtons[index].Label;
}

void Popup::ButtonCallback(uint index) const
{
	if (mButtons[index].Callback)
	{
		mButtons[index].Callback();
	}
}

uint Popup::Buttons() const
{
	return mButtons.size();
}

OINI_NAMESPACE_END(Oini)
