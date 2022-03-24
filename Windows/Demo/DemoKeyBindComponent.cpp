#include "DemoKeyBindComponent.h"

KeyBinds::Key& DemoKeyBindComponent::getKeyBind() {
	return mKeyBind;
}

bool DemoKeyBindComponent::getCloseWithEsc() {
	return true;
}
