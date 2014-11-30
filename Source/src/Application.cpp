
#include "Application.h"
#include "Exception.h"
#include "FileIni.h"
#include "Window.h"

#include "Poco/DateTimeFormat.h"
#include "Poco/Environment.h"
#include "Poco/File.h"
#include "Poco/FileChannel.h"
#include "Poco/Path.h"
#if defined(OINI_DEBUG_LOG_TO_DEBUGGER)
	#include "Poco/Debugger.h"
#endif

#include "wx/cmdline.h"


NAMESPACE_BEGIN(Oini)

static Application* spApplication = nullptr;

const string APPLICATION_NAME = "Open Ini";
const string CONFIGURATION_FILE = "config.ini";
const string LOGGING_FILE = "log.log";



Application::Application() : wxApp(),
	mpConfigurationFile(nullptr),
	mpLogger(nullptr),
	mPendingFilePath(""),
	mpWindow(nullptr)
{
	spApplication = this;
}

Application::~Application()
{
	LOG("Shutting down program...");

	//Stop the measured time.
	mStopWatch.stop();

	//Free resources used by all opened files.
	for (int i = 0; i < mpFiles.size(); i++)
	{
		if (mpFiles[i].get() != nullptr)
		{
			mpFiles[i].reset();
			mpFiles[i] = nullptr;
		}
	}

	if (mpConfigurationFile.get() != nullptr)
	{
		mpConfigurationFile = nullptr;
	}

	//Log an additional empty line.
	LOG("", true);

	mpLogger = nullptr;
	spApplication = nullptr;
}

/*static*/
Application* Application::Get()
{
	return spApplication;
};

wxIMPLEMENT_APP(Application);

/*virtual*/
bool Application::OnInit()
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

		//Create and get the logger object.
		mpLogger = &Poco::Logger::get("Logger");

		//Create a file channel assigned to a logging file.
		Poco::AutoPtr<Poco::FileChannel> pFileChannel(NEW Poco::FileChannel());
		pFileChannel->setProperty("path", Oini::LOGGING_FILE);
		pFileChannel->setProperty("rotation", "10 M");
		pFileChannel->open();

		//Assign the new channel to the logger object.
		mpLogger->setChannel(pFileChannel);
	}
	catch (std::exception& exception)
	{
#if defined(OINI_DEBUG_LOG_TO_DEBUGGER)
		Poco::Debugger::message(exception.what(), __FILE__, __LINE__);
#endif

		std::cerr << exception.what() << std::endl;

		return false;
	}

	try
	{
		//Initialize the logging flag.
#ifdef OINI_CONFIGURATION_DEBUG
		mEnableLogging = true;
#else
		mEnableLogging = false;
#endif

		//Log the current date and time.
		LOG(Poco::DateTimeFormatter::format(Poco::LocalDateTime(),
			Poco::DateTimeFormat::HTTP_FORMAT), true);

		bool showWindow = false;

		LOG("Parsing command-line arguments...");

		if (argc < 2)
		{
			showWindow = true;

			LOG("No command-line arguments were found.");
		}
		else
		{
			const wxCmdLineEntryDesc commandLineDescs[] =
			{
				{ wxCMD_LINE_SWITCH, "h", "help", "Shows this help list",
					wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
				{ wxCMD_LINE_OPTION, "f", "file",
					"Name of the file to load, with its full path" },
				{ wxCMD_LINE_SWITCH, "l", "log", "Enables logging" },
				{ wxCMD_LINE_NONE }
			};

			//Parse command-line arguments.
			wxCmdLineParser commandLineParser(commandLineDescs, argc, argv);

			showWindow = commandLineParser.Parse(true) == 0;

			if (!showWindow)
			{
				LOG("Showing help message for the command-line.");
			}
			else
			{
				if (!mEnableLogging && commandLineParser.Found(wxT("l")))
				{
					mEnableLogging = true;

					//Log the current date and time.
					LOG(Poco::DateTimeFormatter::format(Poco::LocalDateTime(),
						Poco::DateTimeFormat::HTTP_FORMAT), true);

					LOG("Parsing command-line arguments...");
				}

				string arguments = "";
				for (int i = 1; i < argc; i++)
				{
					arguments += " " + argv[i];
				}

				LOG("The command-line arguments are:" + arguments);

				wxString pendingFilePath;
				if (commandLineParser.Found(wxT("f"), &pendingFilePath))
				{
					mPendingFilePath = pendingFilePath.ToStdString();
				}
			}
		}

		if (showWindow)
		{
			LOG("Initializing program...");

			//Log the program compilation mode.
#ifdef OINI_CONFIGURATION_DEBUG
			LOG("Program is compiled in debug mode.");
#else
			LOG("Program is compiled in release mode.");
#endif

			//Log the operating system details.
			LOG("OS details: " + Poco::Environment::osName() + "; " +
				Poco::Environment::osVersion() + "; " +
				Poco::Environment::osArchitecture());

			mCurrentDirectory = Poco::Path::current();

			LOG("Current working directory: " + mCurrentDirectory);

			LOG("Loading configurable options...");

			Poco::File configurationFile(mCurrentDirectory +
				Oini::CONFIGURATION_FILE);

			if (!configurationFile.exists())
			{
				LOG("The configuration file doesn't exist; creating it...");

				configurationFile.createFile();
			}

			//Load the main configuration file.
			mpConfigurationFile = NEW Poco::Util::IniFileConfiguration(
				Oini::CONFIGURATION_FILE);

			LOG("Opening main window...");

			//Show the main window of the application.
			mpWindow = NEW Window();
			mpWindow->Initialize();

			LOG("Initialization took " + Str(ElapsedTime(), 6) + " seconds.");

			return true;
		}
	}
	catch (Poco::Exception& exception)
	{
		LOG(">>> Error [" + OINI_INFO + "] >>> " + Str(exception.className()) +
			"; " + exception.message() + "; Code: " + Str(exception.code()));

		return false;
	}
	catch (std::exception& exception)
	{
		LOG(">>> Error [" + OINI_INFO + "] >>> " + Str(exception.what()));

		return false;
	}

	return false;
}

/*virtual*/
int Application::OnRun()
{
	if (mPendingFilePath != "")
	{
		LOG("Opening pending file...");

		try
		{
			OpenFile(mPendingFilePath);
		}
		catch (Poco::Exception& exception)
		{
			LOG(">>> Error [" + OINI_INFO + "] >>> " +
				Str(exception.className()) + "; " + exception.message() +
				"; Code: " + Str(exception.code()));

			ShowErrorDialog(exception.message());
		}
		catch (std::exception& exception)
		{
			LOG(">>> Error [" + OINI_INFO + "] >>> " + Str(exception.what()));

			ShowErrorDialog(exception.what());
		}
	}

	return wxApp::OnRun();
}

float Application::ElapsedTime() const
{
	return static_cast<float>(mStopWatch.elapsed()) /
		static_cast<float>(mStopWatch.resolution());
}

void Application::Log(const string& message, bool omitTimeStamp) const
{
	if (mEnableLogging)
	{
		if (!omitTimeStamp)
		{
			mpLogger->information(Poco::DateTimeFormatter::format(
				Poco::LocalDateTime(), "[%M:%S.%i] ") + message);
		}
		else
		{
			mpLogger->information(message);
		}

#if defined(OINI_DEBUG_LOG_TO_DEBUGGER)
		Poco::Debugger::message("> " + message);
#endif
	}
}

string Application::ApplicationName() const
{
	return APPLICATION_NAME;
}

void Application::OpenFile(const string& absolutePath)
{
	std::shared_ptr<FileIni> pFile(NEW FileIni());

	pFile->Initialize(absolutePath);

	mpFiles.push_back(pFile);

	mpWindow->OpenFile(pFile);
}

void Application::CloseFile(const std::shared_ptr<FileIni> pFile)
{
	for (int i = 0; i < mpFiles.size(); i++)
	{
		if (mpFiles[i].get() == pFile.get())
		{
			mpFiles[i].reset();
			mpFiles[i] = nullptr;
		}
	}
}

void Application::ShowErrorDialog(const string& message) const
{
	wxMessageDialog* pDialog = NEW wxMessageDialog(nullptr,
		wxString(message), wxT("Error"), wxICON_ERROR | wxOK);

	pDialog->ShowModal();
}

NAMESPACE_END //Oini
