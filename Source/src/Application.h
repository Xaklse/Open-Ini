
#ifndef OINI_APPLICATION_H
#define OINI_APPLICATION_H


#include "Globals.h"

#include "Poco/Util/Application.h"


OINI_NAMESPACE_BEGIN(Oini)

class Application : public Poco::Util::Application, private boost::noncopyable
{
public:
	Application();
	virtual ~Application();

public:
	static Application* Get();

protected:
	void defineOptions(Poco::Util::OptionSet& options) override
		{ super::defineOptions(options); AddOptions(options); }
	void handleOption(const std::string& name,const std::string& value) override
		{ super::handleOption(name, value); HandleOption(name, value); }

public:
	int run() override
		{ initialize(*this); Run(); uninitialize(); return mErrorCode; }

public:
	void ErrorCode(Poco::Util::Application::ExitCode errorCode)
		{ mErrorCode = errorCode; }

protected:
	void AddOptions(Poco::Util::OptionSet& options);
	void HandleOption(const string& name, const string& value);

protected:
	void Run();

public:
	float ElapsedTime() const;

public:
	string GetSessionLog() const
		{ return mSessionLog; }
	void Log(const string& message, bool omitTimeStamp = false);

protected:
	bool mEnableLogging = false;
	string mSessionLog = "";

public:
	std::shared_ptr<class Font> GetFont(const string& identifier);

public:
	void OpenFile(const string& absolutePath);
	void CloseFile(const std::shared_ptr<class FileIni> pFile);

protected:
	string mCurrentDirectory = "";
	Poco::Util::Application::ExitCode mErrorCode =
		Poco::Util::Application::EXIT_OK;

protected:
	Poco::Stopwatch mStopWatch;

protected:
	std::vector<std::shared_ptr<class Font>> mpFonts;
	std::unique_ptr<class Logger> mpLogger;
	std::unique_ptr<class WindowMain> mpMainWindow;

public:
	std::vector<std::shared_ptr<class FileIni>> mpIniFiles;

private:
	typedef Poco::Util::Application super;
};

OINI_NAMESPACE_END(Oini)


#endif //OINI_APPLICATION_H
