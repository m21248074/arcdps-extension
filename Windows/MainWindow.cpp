#include "MainWindow.h"

#include "../Widgets.h"
#include "../imgui_stdlib.h"

MainWindow::~MainWindow() {
	mComponents.clear();
}

void MainWindow::Draw(ImGuiWindowFlags imGuiWindowFlags, MainWindowFlags mainWindowFlags) {
	if (!GetOpenVar()) {
		return;
	}

	// general window options
	imGuiWindowFlags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoNavFocus;

	if (!getShowTitleBar()) {
		imGuiWindowFlags |= ImGuiWindowFlags_NoTitleBar;
	}

	if (!getShowBackground()) {
		imGuiWindowFlags |= ImGuiWindowFlags_NoBackground;
	}

	if (!GetShowScrollbar()) {
		imGuiWindowFlags |= ImGuiWindowFlags_NoScrollbar;
	}

	const auto& padding = getPadding();
	bool paddingApplied = false;
	if (padding) {
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, padding.value());
		paddingApplied = true;
	}

	if (getSizingPolicy() == SizingPolicy::SizeToContent) {
		imGuiWindowFlags |= ImGuiWindowFlags_AlwaysAutoResize;
	}

	if (getMaxHeightActive() && mThisWindow != nullptr && mMaxHeightCursorPos > 0) {
		ImVec2 windowPos = mThisWindow->Pos;

		if (!mThisWindow->ScrollbarY && static_cast<int32_t>(mMaxHeightCursorPos + windowPos.y /*- GImGui->Style.ItemSpacing.y*/) == static_cast<int32_t>(mThisWindow->ContentSize.y + mThisWindow->ContentRegionRect.Min.y)) {
			// [ DEBUG ]
			// ImVec2 max = mThisWindow->ContentSize;
			// ImVec2 min = mThisWindow->ContentRegionRect.Min;
			// max += min;
			// ImGui::GetForegroundDrawList(mThisWindow)->AddRect(min, max, 0xff00ff00);
		} else {
			// [ DEBUG ]
			// ImVec2 max = mThisWindow->ContentSize;
			// ImVec2 min = mThisWindow->ContentRegionRect.Min;
			// max += min;
			// ImVec2 cursorPos(max.x, mMaxHeightCursorPos + windowPos.y /*- GImGui->Style.ItemSpacing.y*/);
			// ImGui::GetForegroundDrawList(mThisWindow)->AddRect(min, max, 0xff0000ff);
			// ImGui::GetForegroundDrawList(mThisWindow)->AddRect(min, cursorPos, 0xffff0000);

			float minHeight = mThisWindow->InnerRect.Min.y + GImGui->Style.WindowPadding.y - windowPos.y + 5.f;
			float maxHeight = mMaxHeightCursorPos /*- GImGui->Style.ItemSpacing.y*/ + GImGui->Style.WindowPadding.y;
			ImGui::SetNextWindowSizeConstraints(ImVec2(50.f, minHeight), ImVec2(FLT_MAX, maxHeight));
		}
	}

	for (const auto& preDrawHook : mPreDrawHooks) {
		preDrawHook(imGuiWindowFlags);
	}

	const auto& titleOpt = getTitle();
	std::string title = titleOpt ? titleOpt.value() : getTitleDefault();
	title.append("###");
	title.append(getWindowID());
	ImGui::Begin(title.c_str(), &GetOpenVar(), imGuiWindowFlags);

	mThisWindow = ImGui::GetCurrentWindow();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 4));
	if (ImGuiEx::BeginPopupContextWindow(nullptr, 1, ImGuiHoveredFlags_ChildWindows)) {
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		ImGui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, true);

		DrawContextMenu();

		for (const auto& contextMenuHook : mContextMenuHooks | std::ranges::views::reverse) {
			contextMenuHook();
		}

		if (!(mainWindowFlags & MainWindowFlags_NoStyleMenu)) {
			if (ImGui::BeginMenu("Style")) {
				DrawStyleSettingsSubMenu();

				for (const auto& drawStyleSubMenuHook : mDrawStyleSubMenuHooks | std::ranges::views::reverse) {
					drawStyleSubMenuHook();
				} 

				ImGui::EndMenu();
			}
		}

		ImGui::PopItemFlag();
		ImGui::PopStyleVar();

		ImGui::EndPopup();
	}
	ImGui::PopStyleVar();

	DrawContent();

	for (const auto& drawContentHook : mDrawContentHooks) {
		drawContentHook();
	}

	if (paddingApplied) {
		ImGui::PopStyleVar();
	}

	ImGui::End();
}

void MainWindow::Init() {
	for (const auto& initHook : mInitHooks) {
		initHook();
	}
}

void MainWindow::DrawOptionCheckbox() {
	const auto& appearAsInOptionOpt = getAppearAsInOption();
	const std::string& appearAsInOption = appearAsInOptionOpt ? appearAsInOptionOpt.value() : getAppearAsInOptionDefault();
	ImGui::Checkbox(appearAsInOption.c_str(), &GetOpenVar());
}

void MainWindow::SetMaxHeightCursorPos(float pNewCursorPos) {
	mMaxHeightCursorPos = pNewCursorPos;
}

ImGuiWindow* MainWindow::GetInnerWindow() {
	return mThisWindow;
}

void MainWindow::RegisterPreDrawHook(PreDrawHookFunction pFun) {
	mPreDrawHooks.emplace_back(pFun);
}

void MainWindow::RegisterContextMenuHook(ContextMenuHookFunction pFun) {
	mContextMenuHooks.emplace_back(pFun);
}

void MainWindow::RegisterContentHook(DrawContentHookFunction pFun) {
	mDrawContentHooks.emplace_back(pFun);
}

void MainWindow::RegisterInitHook(InitHookFunction pFun) {
	mInitHooks.emplace_back(pFun);
}

void MainWindow::RegisterDrawStyleSubMenuHook(DrawStyleSubMenuHookFunction pFun) {
	mDrawStyleSubMenuHooks.emplace_back(pFun);
}

float MainWindow::GetMaxCursorPos() {
	return mMaxHeightCursorPos;
}

void MainWindow::DrawStyleSettingsSubMenu() {
	ImGui::Checkbox("Title bar", &getShowTitleBar());
	ImGui::Checkbox("Background", &getShowBackground());
	ImGui::Checkbox("Scrollbar", &GetShowScrollbar());

	// padding
	ImGui::Text("padding");
	ImGui::SameLine();
	auto& padding = getPadding();
	mPaddingActive = padding.has_value();
	if (ImGui::Checkbox("##paddingCheckbox", &mPaddingActive)) {
		if (mPaddingActive == true) {
			padding = ImVec2();
		} else {
			padding.reset();
		}
	}
	ImGui::SameLine();
	if (!padding) {
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
	}
	mPaddingBuffer[0] = padding ? padding.value().x : 0.f;
	mPaddingBuffer[1] = padding ? padding.value().y : 0.f;
	if (ImGui::DragFloat2("##paddingInputFloat", mPaddingBuffer, 0.1f, 0, 0, "%1.f")) {
		auto& value = padding.value();
		value.x = mPaddingBuffer[0];
		value.y = mPaddingBuffer[1];
	}
	if (!padding) {
		ImGui::PopItemFlag();
	}

	ImGui::Separator();

	// sizing policy
	ImGui::Text("Sizing Policy");
	ImGui::SameLine();
	auto& sizingPolicy = getSizingPolicy();
	ImGuiEx::EnumCombo("##sizingPolicyEnumCombo", sizingPolicy, SizingPolicy::FINAL_ENTRY);

	ImGui::Indent();
	DrawSizingPolicySubSettings(sizingPolicy);
	ImGui::Unindent();

	ImGui::Separator();

	auto& appearAsInOptionOpt = getAppearAsInOption();
	if (appearAsInOptionOpt) {
		mAppearAsInOptionTextBuffer = appearAsInOptionOpt.value();
	} else {
		mAppearAsInOptionTextBuffer = "";
	}
	if (ImGui::InputText("appear as in option###appearAsInOption", &mAppearAsInOptionTextBuffer)) {
		if (mAppearAsInOptionTextBuffer.empty()) {
			appearAsInOptionOpt.reset();
		} else {
			appearAsInOptionOpt = mAppearAsInOptionTextBuffer;
		}
	}

	auto& titleOpt = getTitle();
	if (titleOpt) {
		mTitleBuffer = titleOpt.value();
	} else {
		mTitleBuffer = "";
	}
	if (ImGui::InputText("title bar###titleBar", &mTitleBuffer)) {
		if (mTitleBuffer.empty()) {
			titleOpt.reset();
		} else {
			titleOpt = mTitleBuffer;
		}
	}
}

bool MainWindow::KeyBindPressed() {
	// TODO: implement proper checks
	return true;
}
