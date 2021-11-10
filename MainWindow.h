#pragma once

#include <string>

#include "arcdps_structs.h"

#include "../imgui/imgui.h"

struct Param {
	std::string title;
	bool* p_open;
	ImGuiWindowFlags flags = 0;
	bool showHeader = true;
	Position position = Position::Manual;
	ImVec2 cornerVector;
	CornerPosition cornerPosition;
	ImGuiID fromWindowID;
	CornerPosition anchorPanelCornerPosition;
	CornerPosition selfPanelCornerPosition;
};

class MainWindow {
public:
	void Draw(const Param& param);

protected:
	virtual void DrawContextMenu() = 0;
	virtual void DrawContent() = 0;
};
