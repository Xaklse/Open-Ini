
#ifndef OINI_WINDOW_MAIN_H
#define OINI_WINDOW_MAIN_H


#include "Window.h"

#include "imgui.h"

#include "tinyfiledialogs.h"


OINI_NAMESPACE_BEGIN(Oini)

class WindowMain : public Window
{
public:
	WindowMain();
	virtual ~WindowMain();

public:
	void Initialize() override;
	void Shutdown() override;

public:
	void DrawGraphics() override;
	void HandleEvent(sf::Event event) override;
	void HandleNotification(Notification* pNotification) override;

private:
	void DrawMenus();
	void DrawActions();
	void DrawTabs();
	void DrawFile();
	void DrawPopups();

protected:
	void NumberSignAsComment(bool set);

protected:
	bool mLogOpened = false;
	bool mNumberSignAsComment = false;

private:
	typedef Window super;
};

OINI_NAMESPACE_END(Oini)


// imgui-events-SFML.h // imgui-backends (2016-06-18):
#include "SFML/Window/Event.hpp"
#include "SFML/Window/Keyboard.hpp"
#include "SFML/Window/Window.hpp"

namespace ImGui
{
	namespace ImImpl
	{
		static sf::Clock ImImpl_timeElapsed;
		static bool ImImpl_mousePressed[5] = { false, false, false, false, false };
		static sf::Window* ImImpl_window;
	}
	namespace SFML
	{
		static void SetWindow(sf::Window& window)
		{
			ImImpl::ImImpl_window = &window;
		}

		static void ProcessEvent(sf::Event& event)
		{
			switch (event.type)
			{
				case sf::Event::MouseButtonPressed:
				{
					ImImpl::ImImpl_mousePressed[event.mouseButton.button] = true;
					break;
				}
				case sf::Event::MouseButtonReleased:
				{
					ImImpl::ImImpl_mousePressed[event.mouseButton.button] = false;
					break;
				}
				case sf::Event::MouseWheelMoved:
				{
					ImGuiIO& io = ImGui::GetIO();
					io.MouseWheel += (float)event.mouseWheel.delta;
					break;
				}
				case sf::Event::KeyPressed:
				{
					ImGuiIO& io = ImGui::GetIO();
					io.KeysDown[event.key.code] = true;
					io.KeyCtrl = event.key.control;
					io.KeyShift = event.key.shift;
					break;
				}
				case sf::Event::KeyReleased:
				{
					ImGuiIO& io = ImGui::GetIO();
					io.KeysDown[event.key.code] = false;
					io.KeyCtrl = event.key.control;
					io.KeyShift = event.key.shift;
					break;
				}
				case sf::Event::TextEntered:
				{
					if (event.text.unicode > 0 && event.text.unicode < 0x10000)
						ImGui::GetIO().AddInputCharacter(event.text.unicode);
					break;
				}
				default:
					break;
			}
		}

		static void InitImGuiEvents()
		{
			ImGuiIO& io = ImGui::GetIO();
			io.KeyMap[ImGuiKey_Tab] = sf::Keyboard::Tab;
			io.KeyMap[ImGuiKey_LeftArrow] = sf::Keyboard::Left;
			io.KeyMap[ImGuiKey_RightArrow] = sf::Keyboard::Right;
			io.KeyMap[ImGuiKey_UpArrow] = sf::Keyboard::Up;
			io.KeyMap[ImGuiKey_DownArrow] = sf::Keyboard::Down;
			io.KeyMap[ImGuiKey_Home] = sf::Keyboard::Home;
			io.KeyMap[ImGuiKey_End] = sf::Keyboard::End;
			io.KeyMap[ImGuiKey_Delete] = sf::Keyboard::Delete;
			io.KeyMap[ImGuiKey_Backspace] = sf::Keyboard::BackSpace;
			io.KeyMap[ImGuiKey_Enter] = sf::Keyboard::Return;
			io.KeyMap[ImGuiKey_Escape] = sf::Keyboard::Escape;
			io.KeyMap[ImGuiKey_A] = sf::Keyboard::A;
			io.KeyMap[ImGuiKey_C] = sf::Keyboard::C;
			io.KeyMap[ImGuiKey_V] = sf::Keyboard::V;
			io.KeyMap[ImGuiKey_X] = sf::Keyboard::X;
			io.KeyMap[ImGuiKey_Y] = sf::Keyboard::Y;
			io.KeyMap[ImGuiKey_Z] = sf::Keyboard::Z;
			ImImpl::ImImpl_timeElapsed.restart();
		}

		static void UpdateImGui()
		{
			ImGuiIO& io = ImGui::GetIO();
			static double time = 0.0f;
			const double current_time = ImImpl::ImImpl_timeElapsed.getElapsedTime().asSeconds();
			io.DeltaTime = (float)(current_time - time);
			time = current_time;
			sf::Vector2i mouse = sf::Mouse::getPosition(*ImImpl::ImImpl_window);
			io.MousePos = ImVec2((float)mouse.x, (float)mouse.y);
			io.MouseDown[0] = ImImpl::ImImpl_mousePressed[0] || sf::Mouse::isButtonPressed(sf::Mouse::Left);
			io.MouseDown[1] = ImImpl::ImImpl_mousePressed[1] || sf::Mouse::isButtonPressed(sf::Mouse::Right);
			ImGui::NewFrame();
		}
	}
}


// imgui-rendering-SFML.h // imgui-backends (2016-06-18):
#include "SFML/OpenGL.hpp"
#include "SFML/Graphics/RenderTarget.hpp"
#include "SFML/Graphics/Texture.hpp"

namespace ImGui
{
	namespace ImImpl
	{
		static sf::RenderTarget* ImImpl_rtarget;
		static sf::Texture* ImImpl_fontTex;

		static void ImImpl_RenderDrawLists(ImDrawData* draw_data)
		{
			if (draw_data->CmdListsCount == 0)
				return;

			ImImpl_rtarget->pushGLStates();

			GLint last_texture;
			glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
			GLint last_viewport[4];
			glGetIntegerv(GL_VIEWPORT, last_viewport);
			glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TRANSFORM_BIT);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDisable(GL_CULL_FACE);
			glDisable(GL_DEPTH_TEST);
			glEnable(GL_SCISSOR_TEST);
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);
			glEnable(GL_TEXTURE_2D);

			ImGuiIO& io = ImGui::GetIO();
			glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
			glOrtho(0.0f, io.DisplaySize.x, io.DisplaySize.y, 0.0f, -1.0f, +1.0f);
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();

			sf::RenderStates states;
			states.blendMode = sf::BlendMode(sf::BlendMode::SrcAlpha, sf::BlendMode::OneMinusSrcAlpha);

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
			for (int n = 0; n < draw_data->CmdListsCount; n++)
			{
				const ImDrawList* cmd_list = draw_data->CmdLists[n];
				const unsigned char* vtx_buffer = (const unsigned char*)&cmd_list->VtxBuffer.front();
				const ImDrawIdx* idx_buffer = &cmd_list->IdxBuffer.front();
				glVertexPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*)(vtx_buffer + OFFSETOF(ImDrawVert, pos)));
				glTexCoordPointer(2, GL_FLOAT, sizeof(ImDrawVert), (void*)(vtx_buffer + OFFSETOF(ImDrawVert, uv)));
				glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(ImDrawVert), (void*)(vtx_buffer + OFFSETOF(ImDrawVert, col)));

				for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.size(); cmd_i++)
				{
					const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
					if (pcmd->UserCallback)
					{
						pcmd->UserCallback(cmd_list, pcmd);
					}
					else
					{
						sf::Vector2u win_size = ImImpl_rtarget->getSize();
						sf::Texture::bind((sf::Texture*)pcmd->TextureId);
						glScissor((int)pcmd->ClipRect.x, (int)(win_size.y - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
						glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, GL_UNSIGNED_SHORT, idx_buffer);
					}
					idx_buffer += pcmd->ElemCount;
				}
			}
#undef OFFSETOF

			// Restore modified state
			glDisableClientState(GL_COLOR_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glDisableClientState(GL_VERTEX_ARRAY);
			glBindTexture(GL_TEXTURE_2D, last_texture);
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glPopAttrib();

			ImImpl_rtarget->popGLStates();

			ImImpl_rtarget->resetGLStates();
		}
	}
	namespace SFML
	{
		static void SetRenderTarget(sf::RenderTarget& target)
		{
			ImImpl::ImImpl_rtarget = &target;
		}

		static void InitImGuiRendering()
		{
			ImGuiIO& io = ImGui::GetIO();
			io.DisplaySize = ImVec2((float)ImImpl::ImImpl_rtarget->getSize().x, (float)ImImpl::ImImpl_rtarget->getSize().y);
			io.IniFilename = nullptr;
			io.RenderDrawListsFn = ImImpl::ImImpl_RenderDrawLists;

			ImFontConfig config;
			config.OversampleH = 3;
			config.OversampleV = 3;
			config.GlyphExtraSpacing.x = 1.0f;
			io.Fonts->AddFontFromFileTTF("droidsans.ttf", 18.0f, &config);

			unsigned char* pixels;
			int width, height;
			io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
			ImImpl::ImImpl_fontTex = new sf::Texture;
			ImImpl::ImImpl_fontTex->create(width, height);
			ImImpl::ImImpl_fontTex->update(pixels);
			io.Fonts->TexID = (void*)ImImpl::ImImpl_fontTex;
			io.Fonts->ClearInputData();
			io.Fonts->ClearTexData();
		}

		static void UpdateImGuiRendering()
		{
			ImGuiIO& io = ImGui::GetIO();
			io.DisplaySize = ImVec2((float)ImImpl::ImImpl_rtarget->getSize().x, (float)ImImpl::ImImpl_rtarget->getSize().y);
		}

		static void Shutdown()
		{
			ImGuiIO& io = ImGui::GetIO();
			io.Fonts->TexID = nullptr;
			delete ImImpl::ImImpl_fontTex;

			ImImpl::ImImpl_rtarget = nullptr;
			ImGui::Shutdown();
		}
	}
}


OINI_NAMESPACE_BEGIN(ImGui)
OINI_NAMESPACE_BEGIN(SFML)

static void Initialize(Oini::Window& window)
{
	ImGui::SFML::SetRenderTarget(window);
	ImGui::SFML::InitImGuiRendering();
	ImGui::SFML::SetWindow(window);
	ImGui::SFML::InitImGuiEvents();
}

static void UpdateValues()
{
	ImGui::SFML::UpdateImGui();
	ImGui::SFML::UpdateImGuiRendering();
}

OINI_NAMESPACE_END(SFML)
OINI_NAMESPACE_END(ImGui)


#endif //OINI_WINDOW_MAIN_H
