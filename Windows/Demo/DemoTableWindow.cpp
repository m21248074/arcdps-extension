#include "DemoTableWindow.h"

#include "DemoKeyBindComponent.h"
#include "DemoPositioningComponent.h"

#include "../../KeyBindHandler.h"

DemoTableWindow::DemoTableWindow() : MainWindow() {
	CreateComponent<DemoPositioningComponent>();
	CreateComponent<DemoKeyBindComponent>();
	mTable = std::make_unique<DemoTable>(this);
}

bool& DemoTableWindow::GetOpenVar() {
	return mOpen;
}

void DemoTableWindow::SetMaxHeightCursorPos(float pNewCursorPos) {
	MainWindow::SetMaxHeightCursorPos(pNewCursorPos - GImGui->Style.ItemSpacing.y);
}

void DemoTableWindow::DrawContextMenu() {
	ImGui::Text("testWindowContextMenu");

	mTable->DrawColumnSetupMenu();

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
	ImGui::PopStyleVar();
}

void DemoTableWindow::DrawContent() {
	mTable->Draw();
}

const std::string& DemoTableWindow::getTitleDefault() {
	return mTitleDefault;
}

std::optional<std::string>& DemoTableWindow::getTitle() {
	return mTitle;
}

const std::string& DemoTableWindow::getWindowID() {
	return mWindowID;
}

bool& DemoTableWindow::getShowTitleBar() {
	return mShowTitleBar;
}

bool& DemoTableWindow::getShowBackground() {
	return mGetShowBackground;
}

bool& DemoTableWindow::GetShowScrollbar() {
	return mShowScrollbar;
}

std::optional<ImVec2>& DemoTableWindow::getPadding() {
	return mPadding;
}

SizingPolicy& DemoTableWindow::getSizingPolicy() {
	return mSizingPolicy;
}

std::optional<std::string>& DemoTableWindow::getAppearAsInOption() {
	return mAppearAsInOptionOpt;
}

const std::string& DemoTableWindow::getAppearAsInOptionDefault() {
	return mAppearAsInOptionDefault;
}
