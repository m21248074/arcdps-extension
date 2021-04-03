#pragma once

#include <functional>

#include "arcdps_structs.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

static inline ImVec2 operator+(const ImVec2& lhs, const float rhs) { return ImVec2(lhs.x + rhs, lhs.y + rhs); }

namespace ImGuiEx {
	bool Spinner(const char* label, float radius, float thickness, const ImU32& color);
	bool SpinnerAligned(const char* label, float radius, float thickness, const ImU32& color, Alignment alignment);
	void AlignedTextColumn(Alignment alignment, const char* text, ...);
	void TableHeader(const char* label, bool show_label, ImTextureID texture, Alignment alignment = Alignment::Left);
	void AlignedProgressBar(float fraction, const ImVec2& size_arg, const char* overlay, Alignment alignment);
	bool BeginMenu(const char* label, bool enabled, bool& hovered);
	void BeginMenuChild(const char* child_str_id, const char* menu_label, std::function<void()> draw_func);
	void BeginMenu(const char* menu_label, std::function<void()> draw_func);
	bool BeginPopupContextWindow(const char* str_id, ImGuiPopupFlags popup_flags, ImGuiHoveredFlags hovered_flags);
	void MenuItemTableColumnVisibility(ImGuiTable* table, int columnIdx);

	template<typename E>
	void Selectable(E& storage, E value) {
		if (ImGui::Selectable(to_string(value).c_str())) {
			storage = value;
		}
	}
}
