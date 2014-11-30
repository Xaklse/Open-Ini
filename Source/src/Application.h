
#ifndef OINI_APPLICATION_H
#define OINI_APPLICATION_H


#include "Globals.h"

#include "wx/wxprec.h"
#if !defined(WX_PRECOMP)
	#include "wx/wx.h"
#endif


NAMESPACE_BEGIN(Oini)

class FileIni;
class Window;


class Application : public wxApp, private boost::noncopyable
{
public:
	Application();
	virtual ~Application();

public:
	static Application* Get();

public:
	virtual bool OnInit() override;
	virtual int OnRun() override;

public:
	float ElapsedTime() const;
	void Log(const string& message, bool omitTimeStamp = false) const;

public:
	string ApplicationName() const;

public:
	void OpenFile(const string& absolutePath);
	void CloseFile(const std::shared_ptr<FileIni> pFile);

public:
	void ShowErrorDialog(const string& message) const;

protected:
	string mCurrentDirectory;
	bool mEnableLogging;

private:
	string mPendingFilePath;

protected:
	Poco::AutoPtr<Poco::Util::IniFileConfiguration> mpConfigurationFile;
	std::vector<std::shared_ptr<FileIni>> mpFiles;
	Poco::Logger* mpLogger;
	Poco::Stopwatch mStopWatch;
	Window* mpWindow;
};

NAMESPACE_END //Oini


#endif //OINI_APPLICATION_H
