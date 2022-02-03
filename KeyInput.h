#pragma once

#if __has_include("KeyBindsTranslation.h") || __has_include("../unofficial_extras/KeyBindsTranslation.h")
	#if __has_include("KeyBindsTranslation.h")
		#include "KeyBindsTranslation.h"
	#elif __has_include("../unofficial_extras/KeyBindsTranslation.h")
		#include "../unofficial_extras/KeyBindsTranslation.h"
	#endif
/**
 * How to use:
 *
 * Call `KeyCodeInputWndHandle` in `mod_wnd` or your specific window handler.
 * Call `KeyCodeInput` whenever you want to have a KeyBind selection.
 * the wndHandler only has to be called once.
 */
namespace ImGuiEx {
	enum KeyCodeInputFlags_ : int32_t {
		KeyCodeInputFlags_NoModifier = 0b1,
		KeyCodeInputFlags_NoMouse    = 0b10,
	};
	typedef int32_t KeyCodeInputFlags;

	/**
	 * \brief Has to be called in `mod_wnd` or your specific window handler. This function only has to be called once per program.
	 * \param pWindowHandle WndHandle of mod_wnd
	 * \param pMessage Message of mod_wnd
	 * \param pAdditionalW wParam of mod_wnd
	 * \param pAdditionalL lParam of mod_wnd
	 * \return `true` if the event was consumed. Make sure to stop the event calls to run further.
	 */
	bool KeyCodeInputWndHandle(HWND pWindowHandle, UINT pMessage, WPARAM pAdditionalW, LPARAM pAdditionalL);

	/**
	 * \brief Show a keybind on a button. On pressing that button a popup is opened and all keyevents are used to fill the keybind. (Kind of like GW2 does it).
	 * \param pLabel Shown as text next to the input button. Also shown in the popup header. Will be used as ID for the button.
	 * \param pKeyContainer The container of the original key, when pressing `Apply` it will be saved in there.
	 * \param pLanguage The language in which this Tool should be shown.
	 * \param pFlags Additional flags for the input.
	 */
	void KeyCodeInput(const char* pLabel, KeyBinds::Key& pKeyContainer, Language pLanguage, KeyCodeInputFlags pFlags = 0);

	void OpenKeyCodePopupState(const KeyBinds::Key& pKeyContainer, KeyCodeInputFlags pFlags);
	void CloseKeyCodePopupState();
	void RestartKeyCodePopupState();
	void KeyCodeInputActiveFrame();
	const KeyBinds::Key& GetKeyCodeInputKeyState();
}
#endif
