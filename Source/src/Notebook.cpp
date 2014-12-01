
#include "Notebook.h"
#include "Application.h"
#include "Exception.h"
#include "FileIni.h"
#include "Page.h"
#include "Window.h"

#include "wx/artprov.h"


NAMESPACE_BEGIN(Oini)

Notebook::Notebook() : wxAuiNotebook()
{
}

Notebook::~Notebook()
{
}

void Notebook::Initialize(Window* pWindow)
{
	//Create the notebook.
	if (!Create(pWindow, wxID_ANY, wxDefaultPosition, wxDefaultSize,
		wxAUI_NB_DEFAULT_STYLE | wxAUI_NB_TAB_EXTERNAL_MOVE | wxNO_BORDER))
	{
		throw Exception("Notebook creation failed");
	}

	mpWindow = pWindow;
}

wxBEGIN_EVENT_TABLE(Notebook, wxAuiNotebook)
	EVT_MIDDLE_DOWN(Notebook::OnMouseMiddleClick)
	EVT_AUINOTEBOOK_PAGE_CHANGED(wxID_ANY, Notebook::OnPageChanged)
	EVT_AUINOTEBOOK_PAGE_CLOSE(wxID_ANY, Notebook::OnPageClose)
	EVT_AUINOTEBOOK_PAGE_CLOSED(wxID_ANY, Notebook::OnPageClosed)
wxEND_EVENT_TABLE()

void Notebook::OnMouseMiddleClick(wxMouseEvent& event)
{
	//Disable Ctrl-Alt-middle click.
}

void Notebook::OnPageChanged(wxAuiNotebookEvent& event)
{
	//Find the selected page.
	const auto pPage = static_cast<Page*>(GetPage(event.GetSelection()));

	const string title = pPage->FetchHint() + " - " +
		Application::Get()->ApplicationName();

	//Change the title of the main window.
	mpWindow->SetTitle(wxString(title.c_str(), wxConvUTF8));
}

void Notebook::OnPageClose(wxAuiNotebookEvent& event)
{
	try
	{
		//Find the page that will be closed.
		const auto pPage = static_cast<Page*>(GetPage(event.GetSelection()));

		//Ask the page if the close action should be ignored.
		if (!pPage->AllowClose())
		{
			event.Veto();
		}
	}
	catch (Poco::Exception& exception)
	{
		LOG(">>> Error [" + OINI_INFO + "] >>> " + Str(exception.className()) +
			"; " + exception.message() + "; Code: " + Str(exception.code()));

		Application::Get()->ShowErrorDialog(exception.message());
	}
	catch (std::exception& exception)
	{
		LOG(">>> Error [" + OINI_INFO + "] >>> " + Str(exception.what()));

		Application::Get()->ShowErrorDialog(exception.what());
	}
}

void Notebook::OnPageClosed(wxAuiNotebookEvent& event)
{
	if (GetPageCount() == 0)
	{
		//Change the title of the main window.
		mpWindow->SetTitle(Application::Get()->ApplicationName());
	}
}

void Notebook::OpenPage(const std::shared_ptr<FileIni>& pFile)
{
	LOG("Opening new page for file...");

	Freeze();

	//Create a new page.
	Page* pPage = NEW Page();
	pPage->Initialize(this, pFile);

	//Add the new page.
	if (!AddPage(pPage, wxEmptyString, true,
		wxArtProvider::GetBitmap(wxART_NORMAL_FILE, wxART_OTHER,
		wxSize(16, 16))))
	{
		throw Exception("Notebook page addition failed");
	}

	SetPageTitle(pPage, pPage->FetchTitle());
	SetPageHint(pPage, pPage->FetchHint());

	Thaw();

	Update();

	LOG("New page opened.");
}

void Notebook::SetPageTitle(Page* pPage, const string& title)
{
	if (!SetPageText(GetPageIndex(pPage), wxString(title.c_str(), wxConvUTF8)))
	{
		LOG("Set page title failed.");
	}
}

void Notebook::SetPageHint(Page* pPage, const string& hint)
{
	if (!SetPageToolTip(GetPageIndex(pPage),
		wxString(hint.c_str(), wxConvUTF8)))
	{
		LOG("Set page hint failed.");
	}
}

NAMESPACE_END //Oini
