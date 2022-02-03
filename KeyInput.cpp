#include "KeyInput.h"

#include "../imgui/imgui.h"

namespace
{
	using KeyBinds::KeyCode;
	static constexpr KeyCode allowedKeyCodesValues[] = {
		KeyCode::LeftAlt, KeyCode::LeftCtrl, KeyCode::LeftShift, KeyCode::Quote, KeyCode::Hash,
		KeyCode::CapsLock, KeyCode::Colon, KeyCode::Minus, KeyCode::Equals, KeyCode::Escape,
		KeyCode::OpenBracket, KeyCode::NumLock, KeyCode::Period, KeyCode::CloseBracket, KeyCode::Semicolon,
		KeyCode::Slash, KeyCode::Print, KeyCode::Tilde, KeyCode::Backspace, KeyCode::Delete, KeyCode::Enter,
		KeyCode::Space, KeyCode::Tab, KeyCode::End, KeyCode::Home, KeyCode::Insert, KeyCode::Next,
		KeyCode::Prior, KeyCode::ArrowDown, KeyCode::ArrowLeft, KeyCode::ArrowRight, KeyCode::ArrowUp,
		KeyCode::F1, KeyCode::F2, KeyCode::F3, KeyCode::F4, KeyCode::F5, KeyCode::F6, KeyCode::F7, KeyCode::F8,
		KeyCode::F9, KeyCode::F10, KeyCode::F11, KeyCode::F12, KeyCode::_0, KeyCode::_1, KeyCode::_2,
		KeyCode::_3, KeyCode::_4, KeyCode::_5, KeyCode::_6, KeyCode::_7, KeyCode::_8, KeyCode::_9, KeyCode::A,
		KeyCode::B, KeyCode::C, KeyCode::D, KeyCode::E, KeyCode::F, KeyCode::G, KeyCode::H, KeyCode::I,
		KeyCode::J, KeyCode::K, KeyCode::L, KeyCode::M, KeyCode::N, KeyCode::O, KeyCode::P, KeyCode::Q,
		KeyCode::R, KeyCode::S, KeyCode::T, KeyCode::U, KeyCode::V, KeyCode::W, KeyCode::X, KeyCode::Y,
		KeyCode::Z, KeyCode::PlusNum, KeyCode::DecimalNum, KeyCode::DivideNum, KeyCode::MultiplyNum,
		KeyCode::_0_NUM, KeyCode::_1_NUM, KeyCode::_2_NUM, KeyCode::_3_NUM, KeyCode::_4_NUM, KeyCode::_5_NUM,
		KeyCode::_6_NUM, KeyCode::_7_NUM, KeyCode::_8_NUM, KeyCode::_9_NUM, KeyCode::EnterNum,
		KeyCode::MinusNum, KeyCode::ImeKey1, KeyCode::ImeKey2, KeyCode::RightAlt, KeyCode::RightCtrl,
		KeyCode::Backslash, KeyCode::F13, KeyCode::F14, KeyCode::F15, KeyCode::F16, KeyCode::F17, KeyCode::F18,
		KeyCode::F19, KeyCode::F20, KeyCode::F21, KeyCode::F22, KeyCode::F23, KeyCode::F24, KeyCode::F25,
		KeyCode::F26, KeyCode::F27, KeyCode::F28, KeyCode::F29, KeyCode::F30, KeyCode::F31, KeyCode::F32,
		KeyCode::F33, KeyCode::F34, KeyCode::F35, KeyCode::RightShift, KeyCode::Eject, KeyCode::EqualNum,
		KeyCode::ClearNum, KeyCode::LeftCmd, KeyCode::Function, KeyCode::RightCmd
	};
	static_assert(std::size(allowedKeyCodesValues) == KeyBinds::KEY_CODES_SIZE_GW2);
}

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

					if (keyCodeInputFlags & KeyCodeInputFlags_OnlyGW2Keys) {
						if (std::ranges::find(allowedKeyCodesValues, keyCode) == std::end(allowedKeyCodesValues)) {
							return true;
						}
					}

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
