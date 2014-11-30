
#ifndef OINI_WINDOW_H
#define OINI_WINDOW_H


#include "Globals.h"

#include "wx/wxprec.h"
#if !defined(WX_PRECOMP)
	#include "wx/wx.h"
#endif
#include "wx/aui/aui.h"


NAMESPACE_BEGIN(Oini)

class FileIni;
class Notebook;


class Window : public wxFrame, private boost::noncopyable
{
public:
	Window();
	virtual ~Window();

public:
	void Initialize();

private:
	wxDECLARE_EVENT_TABLE();

private:
	void OnAbout(wxCommandEvent& event);
	void OnExit(wxCommandEvent& event);
	void OnMouseMiddleClick(wxMouseEvent& event);
	void OnOpen(wxCommandEvent& event);

public:
	void OpenFile(const std::shared_ptr<FileIni> pFile);

private:
	Notebook* mpNotebook;
	wxAuiManager mUiManager;
};

NAMESPACE_END //Oini


#endif //OINI_WINDOW_H
