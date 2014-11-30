
#include "Window.h"
#include "Application.h"
#include "Exception.h"
#include "FileIni.h"
#include "Notebook.h"

#include "wx/artprov.h"


NAMESPACE_BEGIN(Oini)

Window::Window() : wxFrame(),
	mpNotebook(nullptr)
{
}

Window::~Window()
{
	mUiManager.UnInit();
}

void Window::Initialize()
{
	//Create the window.
	if (!Create(nullptr, wxID_ANY, wxEmptyString, wxDefaultPosition,
		wxSize(600, 450)))
	{
		throw Exception("Window creation failed");
	}

	//Tell the UI manager to manage this window.
	mUiManager.SetManagedWindow(this);

	//Change the title of the window.
	SetTitle(wxString(Application::Get()->ApplicationName()));

	//Set the minimum dimensions of the window.
	SetMinSize(wxSize(600, 450));

	//Add "File" menu.
	wxMenu* pMenuFile = NEW wxMenu();
	pMenuFile->Append(wxID_OPEN);
	pMenuFile->Append(wxID_EXIT);

	//Add "Help" menu.
	wxMenu* pMenuHelp = NEW wxMenu();
	pMenuHelp->Append(wxID_ABOUT);

	//Add previous menus to the main menu bar.
	wxMenuBar* pMenuBar = NEW wxMenuBar();
	pMenuBar->Append(pMenuFile, wxT("&File"));
	pMenuBar->Append(pMenuHelp, wxT("&Help"));
	SetMenuBar(pMenuBar);

	//Add a notebook that holds pages.
	mpNotebook = NEW Notebook();
	mpNotebook->Initialize(this);

	if (!mUiManager.AddPane(mpNotebook,
		wxAuiPaneInfo().Name(wxT("NotebookPane")).CenterPane().PaneBorder(
		false)))
	{
		throw Exception("Notebook pane addition failed");
	}

	//Show and center the window.
	Show();
	Centre();

	//Commit all changes made to the UI manager.
	mUiManager.Update();
}

wxBEGIN_EVENT_TABLE(Window, wxFrame)
	EVT_MENU(wxID_ABOUT, Window::OnAbout)
	EVT_MENU(wxID_OPEN, Window::OnOpen)
	EVT_MENU(wxID_EXIT, Window::OnExit)
	EVT_MIDDLE_DOWN(Window::OnMouseMiddleClick)
wxEND_EVENT_TABLE()

void Window::OnAbout(wxCommandEvent& event)
{
	wxMessageBox(wxT("This is about About"), wxT("About"),
		wxICON_INFORMATION | wxOK);
}

void Window::OnExit(wxCommandEvent& event)
{
	Close(true);
}

void Window::OnMouseMiddleClick(wxMouseEvent& event)
{
	//Disable Ctrl-Alt-middle click.
}

void Window::OnOpen(wxCommandEvent& event)
{
	wxFileDialog fileDialog(this, wxT("Open INI file"), wxEmptyString,
		wxEmptyString, "INI files (*.ini)|*.ini",
		wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);

	if (fileDialog.ShowModal() == wxID_OK)
	{
		LOG("Opening file on demand...");

		try
		{
			//Get file paths.
			wxArrayString paths;
			fileDialog.GetPaths(paths);

			for (size_t i = 0; i < paths.GetCount(); i++)
			{
				//Get an absolute path encoded with UTF-8.
				const string absolutePath = paths[i].ToUTF8();

				Application::Get()->OpenFile(absolutePath);
			}
		}
		catch (Poco::Exception& exception)
		{
			LOG(">>> Error [" + OINI_INFO + "] >>> " +
				Str(exception.className()) + "; " + exception.message() +
				"; Code: " + Str(exception.code()));

			Application::Get()->ShowErrorDialog(exception.message());
		}
		catch (std::exception& exception)
		{
			LOG(">>> Error [" + OINI_INFO + "] >>> " + Str(exception.what()));

			Application::Get()->ShowErrorDialog(exception.what());
		}
	}
}

void Window::OpenFile(const std::shared_ptr<FileIni> pFile)
{
	mpNotebook->OpenPage(pFile);
}

NAMESPACE_END //Oini
