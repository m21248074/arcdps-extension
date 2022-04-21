#include "DemoWindow.h"

#include "DemoKeyBindComponent.h"
#include "DemoPositioningComponent.h"

#include "../../KeyBindHandler.h"

DemoWindow::DemoWindow() : MainWindow() {
	CreateComponent<DemoPositioningComponent>();
	CreateComponent<DemoKeyBindComponent>();
}

bool& DemoWindow::GetOpenVar() {
	return mOpen;
}

void DemoWindow::SetMaxHeightCursorPos(float pNewCursorPos) {
	MainWindow::SetMaxHeightCursorPos(pNewCursorPos - GImGui->Style.ItemSpacing.y);
}

void DemoWindow::DrawContextMenu() {
	ImGui::Text("testWindowContextMenu");

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
	ImGui::PopStyleVar();
}

void DemoWindow::DrawContent() {
	mCurrentRow = 0;
	ImGui::Text("testWindow");
	newRow();
	ImGui::Text("some additional text");
	newRow();
	ImGui::Text("more text :P");
	newRow();
}

void DemoWindow::newRow() {
	if (mCurrentRow < mMaxDisplayed) {
		SetMaxHeightCursorPos();
	}

	++mCurrentRow;
}

const std::string& DemoWindow::getTitleDefault() {
	return mTitleDefault;
}

std::optional<std::string>& DemoWindow::getTitle() {
	return mTitle;
}

const std::string& DemoWindow::getWindowID() {
	return mWindowID;
}

bool& DemoWindow::getShowTitleBar() {
	return mShowTitleBar;
}

bool& DemoWindow::getShowBackground() {
	return mGetShowBackground;
}

bool& DemoWindow::GetShowScrollbar() {
	return mShowScrollbar;
}

std::optional<ImVec2>& DemoWindow::getPadding() {
	return mPadding;
}

SizingPolicy& DemoWindow::getSizingPolicy() {
	return mSizingPolicy;
}

bool DemoWindow::getMaxHeightActive() {
	return mMaxDisplayed != 0;
}

std::optional<std::string>& DemoWindow::getAppearAsInOption() {
	return mAppearAsInOptionOpt;
}

const std::string& DemoWindow::getAppearAsInOptionDefault() {
	return mAppearAsInOptionDefault;
}

void DemoWindow::DrawStyleSettingsSubMenu() {
	MainWindow::DrawStyleSettingsSubMenu();

	ImGui::InputInt("max displayed", &mMaxDisplayed);
}
