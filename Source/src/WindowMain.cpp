
#include "WindowMain.h"
#include "Application.h"
#include "FileIni.h"
#include "Notification.h"
#include "Popup.h"


OINI_NAMESPACE_BEGIN(Oini)

WindowMain::WindowMain() : Window()
{
}

WindowMain::~WindowMain()
{
}

void WindowMain::Initialize()
{
	Create(800u, 600u, "Open Ini");

	ImGui::SFML::Initialize(*this);

	ForceRedraw(true);
}

void WindowMain::Shutdown()
{
	ImGui::SFML::Shutdown();

	super::Shutdown();
}

void WindowMain::DrawGraphics()
{
	ImGui::SFML::UpdateValues();

	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScrollbarRounding = 0.0f;
	style.ScrollbarSize = 22.0f;
	style.WindowPadding = ImVec2(0.0f, 0.0f);
	style.WindowRounding = 0.0f;
	style.WindowTitleAlign = ImGuiAlign_Center;

	ImGui::Begin("Main", nullptr, ImGuiWindowFlags_MenuBar |
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);

	style.WindowPadding = ImVec2(8.0f, 8.0f);

	const sf::Vector2f size = getView().getSize();
	ImGui::SetWindowSize(ImVec2(size.x, size.y));

	if (!mKeepOpen)
	{
		mKeepOpen = true;

		//Add a closing pop-up.
		std::shared_ptr<Popup> pPopup(OINI_NEW Popup());
		pPopup->Initialize("Closing",
			"Not saved changes will be lost!\nExit anyways?", false);
		pPopup->AddButton("OK", std::bind(&WindowMain::Close, this));
		pPopup->AddButton("Cancel");
		mpPopups.push_back(pPopup);
	}

	DrawMenus();
	DrawActions();
	DrawTabs();
	DrawFile();
	DrawPopups();

	ImGui::End();

//ImGui::ShowTestWindow();

	ImGui::Render();
}

void WindowMain::HandleEvent(sf::Event event)
{
	ImGui::SFML::ProcessEvent(event);
}

void WindowMain::HandleNotification(Notification* pNotification)
{
	//Catch the open file notification.
	if (pNotification->IsType(Notification::Type::FILE_OPEN))
	{
		const char* fileFilter[1] = { "*.ini" };
		auto pFilePath = tinyfd_openFileDialog(nullptr, nullptr, 1, fileFilter,
			nullptr, 0);

		if (pFilePath == nullptr)
		{
			OINI_LOG("Obtained file path is empty or invalid.");
		}

		try
		{
			OINI_APP()->OpenFile(pFilePath);
		}
		catch (Poco::Exception& exception)
		{
			OINI_LOG(">>> Error [" + OINI_INFO + "] >>> " +
				Str(exception.className()) + "; " + exception.message() +
				"; Code: " + Str(exception.code()));

			//Add a new warning pop-up.
			std::shared_ptr<Popup> pPopup(OINI_NEW Popup());
			pPopup->Initialize("Warning",
				"A minor error occurred while performing an operation!\n" +
				exception.message(), true);
			mpPopups.push_back(pPopup);
		}
		catch (std::exception& exception)
		{
			OINI_LOG(">>> Error [" + OINI_INFO + "] >>> " +
				Str(exception.what()));

			//Add a new warning pop-up.
			std::shared_ptr<Popup> pPopup(OINI_NEW Popup());
			pPopup->Initialize("Warning",
				"A minor error occurred while performing an operation!\n" +
				Str(exception.what()), true);
			mpPopups.push_back(pPopup);
		}
	}
	//Catch the open file notification.
	else if (pNotification->IsType(Notification::Type::LOG_OPEN))
	{
		mLogOpened = true;
	}
}

void WindowMain::DrawMenus()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open", "CTRL+O"))
			{
				mNotifications.enqueueNotification(
					OINI_NEW Notification(Notification::Type::FILE_OPEN));
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Exit", "ALT+F4"))
			{
				mNotifications.enqueueUrgentNotification(
					OINI_NEW Notification(Notification::Type::EXIT));
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
			if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}

			ImGui::Separator();

			if (ImGui::MenuItem("Cut", "CTRL+X")) {}
			if (ImGui::MenuItem("Copy", "CTRL+C")) {}
			if (ImGui::MenuItem("Paste", "CTRL+V")) {}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Settings"))
		{
			if (ImGui::MenuItem("Number Sign as Comment", nullptr,
				mNumberSignAsComment))
			{
				NumberSignAsComment(!mNumberSignAsComment);
			}

			ImGui::EndMenu();
		}

#if defined(OINI_CONFIGURATION_DEBUG)
		if (ImGui::BeginMenu("Debug"))
		{
			if (ImGui::MenuItem("Log"))
			{
				mNotifications.enqueueNotification(
					OINI_NEW Notification(Notification::Type::LOG_OPEN));
			}

			ImGui::EndMenu();
		}
#endif

		ImGui::EndMenuBar();
	}
}

void WindowMain::DrawActions()
{
}

void WindowMain::DrawTabs()
{
	for (std::shared_ptr<FileIni>& pFile : OINI_APP()->mpIniFiles)
	{
		if (ImGui::Button(pFile->FileName().c_str())) {}
		ImGui::SameLine();
	}

	if (mLogOpened)
	{
		if (ImGui::Button("OINI Log")) {}
		ImGui::SameLine();
	}

	ImGui::NewLine();
}

void WindowMain::DrawFile()
{
	for (std::shared_ptr<FileIni>& pFile : OINI_APP()->mpIniFiles)
	{
		bool sectionHeaderExpanded = false;

		for (uint32 i = 0u; i < pFile->TextLines(); ++i)
		{
			switch (pFile->TextLineType(i))
			{
				case FileIni::LineType::SECTION:
				{
					sectionHeaderExpanded =
						ImGui::CollapsingHeader(pFile->TextLine(i).c_str());

					break;
				}

				case FileIni::LineType::PROPERTY:
				{
					if (sectionHeaderExpanded)
					{
						ImGui::Columns(2, "properties");
						ImGui::Separator();

						ImGui::Text(pFile->TextLineName(i).c_str());
						ImGui::NextColumn();
						ImGui::Text(pFile->TextLineValue(i).c_str());

						ImGui::Columns(1);
						ImGui::Separator();
					}

					break;
				}
			}
		}
	}

	if (mLogOpened)
	{
		ImGui::BeginChild("LogRegion", ImVec2(0.0f, 0.0f), false,
			ImGuiWindowFlags_HorizontalScrollbar);

		ImGui::TextUnformatted(OINI_APP()->GetSessionLog().c_str());

		ImGui::EndChild();
	}
}

void WindowMain::DrawPopups()
{
	if (mpPopups.size() > 0u)
	{
		auto pPopup = mpPopups[0u];
		ImGui::OpenPopup(pPopup->Name().c_str());

		if (ImGui::BeginPopupModal(pPopup->Name().c_str(), nullptr,
			ImGuiWindowFlags_NoResize))
		{
			//Draw text of message.
			ImGui::Text(pPopup->Message().c_str());

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
			ImGui::Spacing();

			//Draw buttons.
			if (pPopup->Buttons() > 0u)
			{
				//Get sizes.
				const float buttonWidth = 120.0f;
				const float buttonHeight = 24.0f;
				const float buttonSpacing = 20.0f;

				//Calculate horizontal alignment.
				const float totalWidth = buttonWidth * pPopup->Buttons() +
					buttonSpacing * (pPopup->Buttons() - 1u);
				ImGui::SameLine(std::max(0.0f,
					ImGui::GetWindowWidth() - totalWidth) * 0.5f, 0.0f);

				for (uint i = 0u; i < pPopup->Buttons(); ++i)
				{
					if (ImGui::Button(pPopup->ButtonLabel(i).c_str(),
						ImVec2(buttonWidth, buttonHeight)))
					{
						ImGui::CloseCurrentPopup();
						mpPopups.erase(mpPopups.begin());

						pPopup->ButtonCallback(i);
					}

					ImGui::SameLine(0.0f, buttonSpacing);
				}
			}

			ImGui::EndPopup();
		}
	}
}

void WindowMain::NumberSignAsComment(bool set)
{
	if (set != mNumberSignAsComment)
	{
		mNumberSignAsComment = set;

		for (std::shared_ptr<FileIni>& pFile : OINI_APP()->mpIniFiles)
		{
			pFile->NumberSignAsComment(set);
		}
	}
}

OINI_NAMESPACE_END(Oini)
