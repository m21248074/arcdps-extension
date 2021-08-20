#pragma once

#include <functional>

#include "arcdps_structs.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

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
	void WindowReposition(Position position, const ImVec2& cornerVector, CornerPosition cornerPosition, ImGuiID fromWindowID,
	                      CornerPosition anchorPanelCornerPosition, CornerPosition selfPanelCornerPosition);

	template <typename E, typename = typename std::enable_if<std::is_enum<E>::value, void>::type>
	void Selectable(E& storage, E value) {
		if (ImGui::Selectable(to_string(value).c_str())) {
			storage = value;
		}
	}

	template <typename E, typename = typename std::enable_if<std::is_enum<E>::value, void>::type>
	void EnumCombo(const char* label, E& storage, std::initializer_list<E> values) {
		if (ImGui::BeginCombo(label, to_string(storage).c_str())) {
			for (const E& val : values) {
				ImGuiEx::Selectable(storage, val);
			}

			ImGui::EndCombo();
		}
	}

	template <typename E, typename = typename std::enable_if<std::is_enum<E>::value, void>::type>
	void EnumCombo(const char* label, E& storage, E lastElement) {
		if (ImGui::BeginCombo(label, to_string(storage).c_str())) {
			for (uint64_t i = 0; i < static_cast<uint64_t>(lastElement); ++i) {
				ImGuiEx::Selectable(storage, static_cast<E>(i));
			}

			ImGui::EndCombo();
		}
	}

	template <typename E, typename = typename std::enable_if<std::is_enum<E>::value, void>::type>
	bool EnumRadioButton(int& buttonStorage, E value) {
		return ImGui::RadioButton(to_string(value).c_str(), &buttonStorage, static_cast<int>(value));
	}

	template <typename E, typename = typename std::enable_if<std::is_enum<E>::value, void>::type>
	bool EnumRadioButton(int& buttonStorage, E value, E& storage) {
		bool res = EnumRadioButton(buttonStorage, value);
		if (res) {
			storage = value;
		}

		return res;
	}

#ifdef _WIN32
	/// <summary>
	/// KeyInput, has `label` on the left, a textInput in the middle and the used shortcut at the right.
	/// Only defined for Windows, cause `VkKeyScanA`, `MapVirtualKeyA` and `GetKeyNameTextA` are windows-api functions.
	/// </summary>
	/// <param name="label">text to the left of the textInput</param>
	/// <param name="id">id for the inputtext (has to be unique and start with '##')</param>
	/// <param name="buffer">textbuffer for the textInput</param>
	/// <param name="bufSize">the size of the textbuffer</param>
	/// <param name="keyContainer">int container for the real keyCode</param>
	void KeyInput(const char* label, const char* id, char* buffer, size_t bufSize, WPARAM& keyContainer);
#endif
}
