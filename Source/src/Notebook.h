
#ifndef OINI_NOTEBOOK_H
#define OINI_NOTEBOOK_H


#include "Globals.h"

#include "wx/wxprec.h"
#if !defined(WX_PRECOMP)
	#include "wx/wx.h"
#endif
#include "wx/aui/aui.h"


NAMESPACE_BEGIN(Oini)

class FileIni;
class Page;
class Window;


class Notebook : public wxAuiNotebook, private boost::noncopyable
{
public:
	Notebook();
	virtual ~Notebook();

public:
	void Initialize(Window* pWindow);

private:
	wxDECLARE_EVENT_TABLE();

private:
	void OnMouseMiddleClick(wxMouseEvent& event);
	void OnPageChanged(wxAuiNotebookEvent& event);
	void OnPageClose(wxAuiNotebookEvent& event);
	void OnPageClosed(wxAuiNotebookEvent& event);

public:
	void OpenPage(const std::shared_ptr<FileIni>& pFile);

public:
	void SetPageTitle(Page* pPage, const string& title);
	void SetPageHint(Page* pPage, const string& hint);

private:
	Window* mpWindow;
};

NAMESPACE_END //Oini


#endif //OINI_NOTEBOOK_H
