
#ifndef OINI_PAGE_H
#define OINI_PAGE_H


#include "Globals.h"

#include "wx/wxprec.h"
#if !defined(WX_PRECOMP)
	#include "wx/wx.h"
#endif


NAMESPACE_BEGIN(Oini)

class FileIni;
class Notebook;


class Page : public wxTextCtrl, private boost::noncopyable
{
public:
	Page();
	virtual ~Page();

public:
	void Initialize(Notebook* pNotebook, const std::shared_ptr<FileIni> pFile);

public:
	bool AllowClose();

public:
	string FetchTitle() const;
	string FetchHint() const;

private:
	std::shared_ptr<FileIni> mpFile;
	Notebook* mpNotebook;
};

NAMESPACE_END //Oini


#endif //OINI_PAGE_H
