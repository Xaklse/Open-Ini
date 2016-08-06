
#include "Application.h"
#include "Exception.h"
#include "FileIni.h"
#include "Font.h"
#include "Logger.h"
#include "WindowMain.h"

#include "Poco/Environment.h"
#if defined(OINI_DEBUG_LOG_TO_DEBUGGER)
	#include "Poco/Debugger.h"
#endif



POCO_APP_MAIN(Oini::Application)

#if defined(OINI_PLATFORM_WINDOWS)
/**
 * The entry point for any window application based on Windows.
 * @param instanceHandle	integer which identifies this application
 * @param hPrevInstance		obsolete
 * @param lpCommandLine		long pointer to the string that has the command line
 * @param nShowCommand		indicates how the window is to appear when created
 * @return					exit value of the program; 0 means no errors
 */
#if !defined(POCO_NO_WSTRING)
int WINAPI wWinMain(HINSTANCE instanceHandle, HINSTANCE hPrevInstance,
	PWSTR lpCommandLine, int nShowCommand)
#else
int WINAPI WinMain(HINSTANCE instanceHandle, HINSTANCE hPrevInstance,
	LPSTR lpCommandLine, int nShowCommand)
#endif
{
	//Avoid compiler warnings about unreferenced parameters.
	UNREFERENCED_PARAMETER(instanceHandle);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCommandLine);
	UNREFERENCED_PARAMETER(nShowCommand);

	//Call the entry point for any console application based on Windows.
#if !defined(POCO_NO_WSTRING)
	wmain(__argc, __wargv);
#else
	main(__argc, __argv);
#endif
}
#endif //OINI_PLATFORM_WINDOWS



OINI_NAMESPACE_BEGIN(Oini)

static Application* spApplication = nullptr;


Application::Application() : Poco::Util::Application()
{
	spApplication = this;
}

Application::~Application()
{
	OINI_LOG("Shutting down program...");

	//Free resources used by all opened files.
	for (std::shared_ptr<FileIni>& pFile : mpIniFiles)
	{
		pFile.reset();
		pFile = nullptr;
	}

	//Free font resources.
	for (std::shared_ptr<Font>& pFont : mpFonts)
	{
		pFont.reset();
		pFont = nullptr;
	}

	//Free resources used by windows.
	if (mpMainWindow.get() != nullptr)
	{
		OINI_LOG("Closing main window...");

		mpMainWindow->Shutdown();
		mpMainWindow.reset(nullptr);
	}

	//Stop the measured time.
	mStopWatch.stop();

	//Log an additional empty line.
	OINI_LOG("", true);

	//Free logger resources.
	if (mpLogger.get() != nullptr)
	{
		mpLogger->Shutdown();
		mpLogger.reset(nullptr);
	}

	spApplication = nullptr;
}

/*static*/
Application* Application::Get()
{
	return spApplication;
}

void Application::AddOptions(Poco::Util::OptionSet& options)
{
	options.addOption(
		Poco::Util::Option("log", "l", "Enables logging").
		required(false).
		repeatable(false));
}

void Application::HandleOption(const string& name, const string& value)
{
	if (name == "log")
	{
		//Enable the logging flag.
		mEnableLogging = true;
	}
}

void Application::Run()
{
#if defined(OINI_DEBUG_MEMORY_LEAKS) && defined(OINI_PLATFORM_WINDOWS)
	//Retrieve the state of the debug flag to control the allocation behavior of
	//the debug heap manager.
	int debugFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

	//Perform memory check for each allocation/deallocation request.
	debugFlag |= _CRTDBG_CHECK_ALWAYS_DF;

	//Perform automatic leak checking at program exit.
	debugFlag |= _CRTDBG_LEAK_CHECK_DF;

	_CrtSetDbgFlag(debugFlag);
#endif

	try
	{
		//Start the time to measure it.
		mStopWatch.start();

		//Create and initialize the logger.
		mpLogger.reset(OINI_NEW Logger());
		mpLogger->Initialize();
	}
	catch (std::exception& exception)
	{
#if defined(OINI_DEBUG_LOG_TO_DEBUGGER)
		Poco::Debugger::message(exception.what(), __FILE__, __LINE__);
#endif

		std::cerr << exception.what() << std::endl;
		ErrorCode(Poco::Util::Application::EXIT_IOERR);

		return;
	}

	try
	{
#if defined(OINI_CONFIGURATION_DEBUG)
		//Always enable the logging flag with debug configuration.
		mEnableLogging = true;
#endif

		//Log the current date and time.
		OINI_LOG(Poco::DateTimeFormatter::format(Poco::LocalDateTime(),
			Poco::DateTimeFormat::HTTP_FORMAT), true);

#if defined(OINI_CONFIGURATION_DEBUG)
		OINI_LOG("Warning: program is compiled with debug configuration.");
#endif

		string commandLine = "";
		Poco::Util::Application::ArgVec arguments = argv();
		auto argumentsIterator = arguments.begin();
		++argumentsIterator;

		//Obtain the command-line arguments.
		while (argumentsIterator != arguments.end())
		{
			commandLine += " " + *argumentsIterator;
			++argumentsIterator;
		}

		if (commandLine.empty())
		{
			OINI_LOG("No command-line arguments were found.");
		}
		else
		{
			OINI_LOG("The command-line arguments are:" + commandLine);
		}

		OINI_LOG("Initializing program...");

		//Log the operating system details.
		OINI_LOG("OS details: " + Poco::Environment::osName() + "; " +
			Poco::Environment::osVersion() + "; " +
			Poco::Environment::osArchitecture());

		mCurrentDirectory = Poco::Path::current();
		OINI_LOG("Current working directory: " + mCurrentDirectory);

		OINI_LOG("Opening main window...");

		//Create and initialize the main window.
		mpMainWindow.reset(OINI_NEW WindowMain());
		mpMainWindow->Initialize();

		OINI_LOG("Initialization took " + Str(ElapsedTime(), 6) + " seconds.");
	}
	catch (Poco::Exception& exception)
	{
		OINI_LOG(">>> Error [" + OINI_INFO + "] >>> " +
			Str(exception.className()) + "; " + exception.message() +
			"; Code: " + Str(exception.code()));

		ErrorCode(Poco::Util::Application::EXIT_CONFIG);

		return;
	}
	catch (std::exception& exception)
	{
		OINI_LOG(">>> Error [" + OINI_INFO + "] >>> " + Str(exception.what()));

		ErrorCode(Poco::Util::Application::EXIT_CONFIG);

		return;
	}

	//Start main loop.
	while (mpMainWindow->Run())
	{
		//Sleep for at least a millisecond.
		Poco::Thread::sleep(1);
	}
}

float Application::ElapsedTime() const
{
	return static_cast<float>(mStopWatch.elapsed()) /
		static_cast<float>(mStopWatch.resolution());
}

void Application::Log(const string& message, bool omitTimeStamp)
{
	if (mEnableLogging)
	{
#if defined(OINI_DEBUG_LOG_TO_DEBUGGER)
		Poco::Debugger::message("> " + message);
#endif

		string logMessage = message;
		if (!omitTimeStamp)
		{
			logMessage = Poco::DateTimeFormatter::format(Poco::LocalDateTime(),
				"[%M:%S.%i] ") + logMessage;
		}

		mpLogger->Log(logMessage);

#if defined(OINI_CONFIGURATION_DEBUG)
		mSessionLog += logMessage + '\n';
#endif
	}
}

std::shared_ptr<Font> Application::GetFont(const string& identifier)
{
	//Search for the font, it might already exist.
	for (std::shared_ptr<Font>& pFont : mpFonts)
	{
		if (pFont->Identifier() == identifier)
		{
			return pFont;
		}
	}

	//Load the new font.
	std::shared_ptr<Font> pFont(OINI_NEW Font());
	pFont->Load(identifier);
	mpFonts.push_back(pFont);

	return pFont;
}

void Application::OpenFile(const string& absolutePath)
{
	//Load the new file.
	std::shared_ptr<FileIni> pIniFile(OINI_NEW FileIni());
	pIniFile->Initialize(absolutePath);
	mpIniFiles.push_back(pIniFile);
}

void Application::CloseFile(const std::shared_ptr<class FileIni> pIniFile)
{
	//Search for the file.
	auto filesIterator = mpIniFiles.begin();
	while (filesIterator != mpIniFiles.end())
	{
		if ((*filesIterator).get() == pIniFile.get())
		{
			//Free resources.
			(*filesIterator).reset();
			(*filesIterator) = nullptr;

			//Delete from files array.
			mpIniFiles.erase(filesIterator);
			return;
		}

		++filesIterator;
	}
}

OINI_NAMESPACE_END(Oini)
