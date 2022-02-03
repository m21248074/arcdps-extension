#include "KeyInput.h"

#include "../imgui/imgui.h"


namespace ImGuiEx
{
	static inline KeyBinds::Key keyCodeInputKeyState;
	static inline KeyBinds::Modifier keyCodeInputCurrentModifier;
	static inline bool keyCodeInputActive = false;
	static inline int keyCodeInputLastActiveFrame = 0;
	static inline KeyCodeInputFlags keyCodeInputFlags = 0;

	bool KeyCodeInputWndHandle(
		[[maybe_unused]] HWND pWindowHandle,
		UINT pMessage, 
		[[maybe_unused]] WPARAM pAdditionalW,
		[[maybe_unused]] LPARAM pAdditionalL
	) {
		if (keyCodeInputActive == true) {
			// if not called last frame, the window is closed and we should not consume any events anymore
			if (keyCodeInputLastActiveFrame < ImGui::GetFrameCount()-1) {
				keyCodeInputActive = false;
				return false;
			}

			switch (pMessage) {
				case WM_KEYUP:
				case WM_SYSKEYUP: {
					UINT scanCode = (pAdditionalL >> 16) & 0x00ff;
					const bool extended = (pAdditionalL >> 24) & 0x1;

					if (extended == true) {
						scanCode |= 0xE000;
					}

					KeyBinds::KeyCode keyCode = MsvcScanCodeToKeyCode(scanCode);
					if (!(keyCodeInputFlags & KeyCodeInputFlags_NoModifier)) {
						KeyBinds::Modifier modifier = KeyBinds::GetModifier(keyCode);
						if (modifier != 0) {
							keyCodeInputCurrentModifier &= !modifier;
						}
					}
					return true;
				}
				case WM_KEYDOWN:
				case WM_SYSKEYDOWN: {
					UINT scanCode = (pAdditionalL >> 16) & 0x00ff;
					const bool extended = (pAdditionalL >> 24) & 0x1;
					const bool previous = (pAdditionalL >> 30) & 0x1;

					if (previous == true) return true;

					if (extended == true) {
						scanCode |= 0xE000;
					}

					KeyBinds::KeyCode keyCode = MsvcScanCodeToKeyCode(scanCode);
					keyCodeInputKeyState.DeviceType = KeyBinds::DeviceType::Keyboard;
					keyCodeInputKeyState.Code = static_cast<int32_t>(keyCode);
					if (!(keyCodeInputFlags & KeyCodeInputFlags_NoModifier)) {
						KeyBinds::Modifier modifier = KeyBinds::GetModifier(keyCode);
						if (modifier != 0) {
							keyCodeInputCurrentModifier |= modifier;
							keyCodeInputKeyState.Modifier = 0;
						} else {
							keyCodeInputKeyState.Modifier = keyCodeInputCurrentModifier;
						}
					}
					return true;
				}
				case WM_XBUTTONDOWN: {
					if (!(keyCodeInputFlags & KeyCodeInputFlags_NoMouse)) {
						WORD word = GET_XBUTTON_WPARAM(pAdditionalW);
						if (word == XBUTTON1) {
							keyCodeInputKeyState.DeviceType = KeyBinds::DeviceType::Mouse;
							keyCodeInputKeyState.Code = static_cast<int32_t>(KeyBinds::MouseCode::Mouse_4);
							keyCodeInputKeyState.Modifier = keyCodeInputCurrentModifier;
						} else if (word == XBUTTON2) {
							keyCodeInputKeyState.DeviceType = KeyBinds::DeviceType::Mouse;
							keyCodeInputKeyState.Code = static_cast<int32_t>(KeyBinds::MouseCode::Mouse_5);
							keyCodeInputKeyState.Modifier = keyCodeInputCurrentModifier;
						}
					}
					return true;
				}
				case WM_MBUTTONDOWN: {
					if (!(keyCodeInputFlags & KeyCodeInputFlags_NoMouse)) {
						keyCodeInputKeyState.DeviceType = KeyBinds::DeviceType::Mouse;
						keyCodeInputKeyState.Code = static_cast<int32_t>(KeyBinds::MouseCode::Mouse_3);
						keyCodeInputKeyState.Modifier = keyCodeInputCurrentModifier;
					}
					return true;
				}
				case WM_KILLFOCUS: {
					if (!(keyCodeInputFlags & KeyCodeInputFlags_NoMouse)){
						// reset modifier when loosing focus (Alt+Tab)
						keyCodeInputCurrentModifier = 0;
					}
					return true;
				}
			}
		}
		return false;
	}

	void OpenKeyCodePopupState(const KeyBinds::Key& pKeyContainer, KeyCodeInputFlags pFlags) {
		keyCodeInputActive = true;
		keyCodeInputKeyState = pKeyContainer;
		keyCodeInputCurrentModifier = 0;
		keyCodeInputFlags = pFlags;
	}

	void CloseKeyCodePopupState() {
		keyCodeInputActive = false;
	}

	void RestartKeyCodePopupState() {
		keyCodeInputActive = true;
	}

	void KeyCodeInputActiveFrame() {
		keyCodeInputLastActiveFrame = ImGui::GetFrameCount();
	}

	const KeyBinds::Key& GetKeyCodeInputKeyState()
	{
		return keyCodeInputKeyState;
	}

	void KeyCodeInput(const char* pLabel, KeyBinds::Key& pKeyContainer, Language pLanguage, KeyCodeInputFlags pFlags) {
		KeyCodeInputActiveFrame();

		ImGui::TextUnformatted(pLabel);
		ImGui::SameLine();
		std::string keyStr = to_string(pKeyContainer, pLanguage, GetKeyboardLayout(NULL));
		ImVec2 textSize = ImGui::CalcTextSize(keyStr.c_str());
		keyStr.append("##");
		keyStr.append(pLabel);

		std::string popupName = "KeyBind - ";
		popupName.append(pLabel);

		if (ImGui::Button(keyStr.c_str(), ImVec2(textSize.x + 50.f, 0.f))) {
			ImGui::OpenPopup(popupName.c_str());

			OpenKeyCodePopupState(pKeyContainer, pFlags);
		}

		// Always center this window when appearing
		ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		ImGui::SetNextWindowSize(ImVec2(250.f, 50.f));
		if (ImGui::BeginPopupModal(popupName.c_str(), NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) {
			std::string keyCodeText = to_string(keyCodeInputKeyState, pLanguage, GetKeyboardLayout(NULL));

			// center text
			float windowX = ImGui::GetWindowSize().x;
			const float indentation = (windowX - ImGui::CalcTextSize(keyCodeText.c_str()).x) * 0.5f;
			ImGui::SameLine(indentation);
			ImGui::PushTextWrapPos(ImGui::GetWindowSize().x - indentation);
			ImGui::TextWrapped(keyCodeText.c_str());
			ImGui::PopTextWrapPos();

			float buttonSizeX = (windowX - ImGui::GetStyle().FramePadding.x*2 - ImGui::GetStyle().ItemSpacing.x - ImGui::GetStyle().WindowPadding.x*2) / 2;
			if (ImGui::Button("Apply", ImVec2(buttonSizeX, 0))) {
				pKeyContainer = keyCodeInputKeyState;
				CloseKeyCodePopupState();
				ImGui::CloseCurrentPopup();
			}
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(buttonSizeX, 0))) {
				CloseKeyCodePopupState();
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}
}
// #endif
