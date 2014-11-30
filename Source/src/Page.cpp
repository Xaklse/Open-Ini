
#include "Page.h"
#include "Application.h"
#include "Exception.h"
#include "FileIni.h"
#include "Notebook.h"


NAMESPACE_BEGIN(Oini)

Page::Page() : wxTextCtrl()
{
}

Page::~Page()
{
	if (mpFile != nullptr)
	{
		Application::Get()->CloseFile(mpFile);
	}
}

void Page::Initialize(Notebook* pNotebook, const std::shared_ptr<FileIni> pFile)
{
	//Create the page.
	if (!Create(pNotebook, wxID_ANY, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, wxNO_BORDER | wxTE_MULTILINE))
	{
		throw Exception("Page creation failed");
	}

	mpFile = pFile;
	mpNotebook = pNotebook;

	//Dump the whole file.
	for (int i = 0; i < mpFile->TextLines(); i++)
	{
		*this << wxString(mpFile->TextLine(i) + mpFile->LineSeparator());
	}
}

bool Page::AllowClose()
{
	return (wxMessageBox(wxT("Are you sure you want to close this tab?"),
		wxT("Please confirm"), wxICON_WARNING | wxYES_NO, this) == wxYES);
}

string Page::FetchTitle() const
{
	return mpFile->FileName();
}

string Page::FetchHint() const
{
	return mpFile->FullPath();
}

NAMESPACE_END //Oini
