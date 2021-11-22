#include "MainWindow.h"

#include "Widgets.h"

void MainWindow::Draw(const Param& param) {
	ImGuiWindowFlags flags = param.flags;

	// general window options
	flags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

	if (!param.showHeader) {
		flags |= ImGuiWindowFlags_NoTitleBar;
	}

	if (param.position != Position::Manual) {
		flags |= ImGuiWindowFlags_NoMove;
	}

	ImGui::Begin(param.title.c_str(), param.p_open, flags);

	if (ImGuiEx::BeginPopupContextWindow(nullptr, 1, ImGuiHoveredFlags_ChildWindows)) {
		DrawContextMenu();

		ImGui::EndPopup();
	}

	DrawContent();

	/**
	 * Reposition Window
	 */
	ImGuiEx::WindowReposition(nullptr, param.position, param.cornerVector, param.cornerPosition, param.fromWindowID,
							  param.anchorPanelCornerPosition, param.selfPanelCornerPosition);

	ImGui::End();
}
