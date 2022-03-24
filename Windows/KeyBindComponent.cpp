#include "KeyBindComponent.h"

#include "MainWindow.h"
#include "../KeyBindHandler.h"


KeyBindComponent::KeyBindComponent(MainWindow* pMainWindow) : ComponentBase(pMainWindow) {
	pMainWindow->RegisterInitHook([this] {
		mKeyBindHandlerId = KeyBindHandler::instance().Subscribe(
		{
			getKeyBind(),
			[this](const KeyBinds::Key&) {
				return getKeyBindSwitch() && KeyBindPressed();
			}, KeyBindHandler::SubscriberFlags_ArcdpsModifier}
		);

		if (getCloseWithEscActive()) {
			mKeyBindEscHandlerId = KeyBindHandler::instance().Subscribe({
				KeyBinds::Key{KeyBinds::DeviceType::Keyboard, static_cast<int32_t>(KeyBinds::KeyCode::Escape), 0},
				[this](const KeyBinds::Key&) {
					return mMainWindow->GetOpenVar() && getCloseWithEsc() && EscPressed();
				},
				0
			});
		}
	});
	
	pMainWindow->RegisterDrawStyleSubMenuHook([this] {
		if (ImGuiEx::KeyCodeInput("Shortcut", getKeyBind(), static_cast<Language>(getCurrentLanguage()), getCurrentHKL(), ImGuiEx::KeyCodeInputFlags_NoModifier)) {
			KeyBindHandler::instance().UpdateKey(mKeyBindHandlerId, getKeyBind());
		}
	});
}

bool KeyBindComponent::KeyBindPressed() {
	mMainWindow->GetOpenVar() = !mMainWindow->GetOpenVar();
	return true;
}

bool KeyBindComponent::EscPressed() {
	mMainWindow->GetOpenVar() = false;
	return true;
}
