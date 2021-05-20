#define IMGUI_DEFINE_MATH_OPERATORS
#include "Widgets.h"

#include <cstdlib>

#include "ImGui_Math.h"

namespace ImGuiEx {
	bool Spinner(const char* label, float radius, float thickness, const ImU32& color) {
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size((radius) * 2, (radius + style.FramePadding.y) * 2);

		const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
		ImGui::ItemSize(bb, style.FramePadding.y);
		if (!ImGui::ItemAdd(bb, id))
			return false;

		// Render
		window->DrawList->PathClear();

		int num_segments = 15;
		int start = abs(ImSin(g.Time * 1.8f) * (num_segments - 5));

		const float a_min = IM_PI * 2.0f * ((float)start) / (float)num_segments;
		const float a_max = IM_PI * 2.0f * ((float)num_segments - 3) / (float)num_segments;

		const ImVec2 centre = ImVec2(pos.x + radius, pos.y + radius + style.FramePadding.y);

		for (int i = 0; i < num_segments; i++) {
			const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
			window->DrawList->PathLineTo(ImVec2(centre.x + ImCos(a + g.Time * 8) * radius,
			                                    centre.y + ImSin(a + g.Time * 8) * radius));
		}

		window->DrawList->PathStroke(color, false, thickness);

		return true;
	}

	bool SpinnerAligned(const char* label, float radius, float thickness, const ImU32& color, Alignment alignment) {
		const float posX = ImGui::GetCursorPosX();
		float newX = posX;
		float elementWidth = radius * 2 + thickness * 2;
		float columnWidth = ImGui::GetColumnWidth();

		switch (alignment) {
			case Alignment::Left:
				break;
			case Alignment::Center:
				newX = posX + columnWidth / 2 - elementWidth / 2;
				break;
			case Alignment::Right:
				newX = posX + columnWidth - elementWidth;
				break;
		}

		// Clip to left, if text is bigger than current column
		if (newX < posX) {
			newX = posX;
		}

		ImGui::SetCursorPosX(newX);

		return Spinner(label, radius, thickness, color);
	}

	void AlignedTextColumn(Alignment alignment, const char* text, ...) {
		va_list args;
		va_start(args, text);
		char buf[4096];
		ImFormatStringV(buf, 4096, text, args);
		va_end(args);

		const float posX = ImGui::GetCursorPosX();
		float newX = posX;
		float textWidth = ImGui::CalcTextSize(buf).x;
		float columnWidth = ImGui::GetColumnWidth();

		switch (alignment) {
			case Alignment::Left:
				break;
			case Alignment::Center:
				newX = posX + columnWidth / 2 - textWidth / 2;
				break;
			case Alignment::Right:
				newX = posX + columnWidth - textWidth;
				break;
		}

		// Clip to left, if text is bigger than current column
		if (newX < posX) {
			newX = posX;
		}

		ImGui::SetCursorPosX(newX);

		ImGui::TextUnformatted(buf);
	}

	// This is a copy of `ImGui::TableHeader(const char* label)`
	// I removed the line, where the header is printed, so i can use it with image only headers.
	// When "show_label" is true, the label will be printed, as in the default one.
	//
	// Emit a column header (text + optional sort order)
	// We cpu-clip text here so that all columns headers can be merged into a same draw call.
	// Note that because of how we cpu-clip and display sorting indicators, you _cannot_ use SameLine() after a TableHeader()
	void TableHeader(const char* label, bool show_label, ImTextureID texture, Alignment alignment) {
		// TODO change eventually
		const float image_size = 16.f;

		// Show label if texture is null
		if (!texture) {
			show_label = true;
		}

		ImGuiContext& g = *GImGui;
		ImGuiWindow* window = g.CurrentWindow;
		if (window->SkipItems)
			return;

		ImGuiTable* table = g.CurrentTable;
		IM_ASSERT(table != NULL && "Need to call TableHeader() after BeginTable()!");
		IM_ASSERT(table->CurrentColumn != -1);
		const int column_n = table->CurrentColumn;
		ImGuiTableColumn* column = &table->Columns[column_n];

		// Label
		if (label == NULL)
			label = "";
		const char* label_end = ImGui::FindRenderedTextEnd(label);
		ImVec2 label_size = ImGui::CalcTextSize(label, label_end, true);
		ImVec2 label_pos = window->DC.CursorPos;

		// If we already got a row height, there's use that.
		// FIXME-TABLE: Padding problem if the correct outer-padding CellBgRect strays off our ClipRect?
		ImRect cell_r = ImGui::TableGetCellBgRect(table, column_n);
		float label_height = table->RowMinHeight - table->CellPaddingY * 2.0f;
		if (show_label) {
			label_height = ImMax(label_size.y, label_height);
		} else {
			label_height = ImMax(image_size, label_height);
		}

		// Calculate ideal size for sort order arrow
		float w_arrow = 0.0f;
		float w_sort_text = 0.0f;
		char sort_order_suf[4] = "";
		const float ARROW_SCALE = 0.65f;
		if ((table->Flags & ImGuiTableFlags_Sortable) && !(column->Flags & ImGuiTableColumnFlags_NoSort)) {
			w_arrow = ImFloor(g.FontSize * ARROW_SCALE + g.Style.FramePadding.x);
			if (column->SortOrder > 0) {
				ImFormatString(sort_order_suf, IM_ARRAYSIZE(sort_order_suf), "%d", column->SortOrder + 1);
				w_sort_text = g.Style.ItemInnerSpacing.x + ImGui::CalcTextSize(sort_order_suf).x;
			}
		}

		// We feed our unclipped width to the column without writing on CursorMaxPos, so that column is still considering for merging.
		float max_pos_x = label_pos.x + w_sort_text + w_arrow;
		if (show_label) {
			max_pos_x += label_size.x;
		} else {
			max_pos_x += image_size;
		}
		column->ContentMaxXHeadersUsed = ImMax(column->ContentMaxXHeadersUsed, column->WorkMaxX);
		column->ContentMaxXHeadersIdeal = ImMax(column->ContentMaxXHeadersIdeal, max_pos_x);

		// Keep header highlighted when context menu is open.
		const bool selected = (table->IsContextPopupOpen && table->ContextPopupColumn == column_n && table->InstanceInteracted == table->InstanceCurrent);
		ImGuiID id = window->GetID(label);
		ImRect bb(cell_r.Min.x, cell_r.Min.y, cell_r.Max.x, ImMax(cell_r.Max.y, cell_r.Min.y + label_height + g.Style.CellPadding.y * 2.0f));
		ImGui::ItemSize(ImVec2(0.0f, label_height)); // Don't declare unclipped width, it'll be fed ContentMaxPosHeadersIdeal
		if (!ImGui::ItemAdd(bb, id))
			return;

		//GetForegroundDrawList()->AddRect(cell_r.Min, cell_r.Max, IM_COL32(255, 0, 0, 255)); // [DEBUG]
		//GetForegroundDrawList()->AddRect(bb.Min, bb.Max, IM_COL32(255, 0, 0, 255)); // [DEBUG]

		// Using AllowItemOverlap mode because we cover the whole cell, and we want user to be able to submit subsequent items.
		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, ImGuiButtonFlags_AllowItemOverlap);
		if (g.ActiveId != id)
			ImGui::SetItemAllowOverlap();
		if (held || hovered || selected) {
			const ImU32 col = ImGui::GetColorU32(held ? ImGuiCol_HeaderActive : hovered ? ImGuiCol_HeaderHovered : ImGuiCol_Header);
			//RenderFrame(bb.Min, bb.Max, col, false, 0.0f);
			ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, col, table->CurrentColumn);
			ImGui::RenderNavHighlight(bb, id, ImGuiNavHighlightFlags_TypeThin | ImGuiNavHighlightFlags_NoRounding);
		} else {
			// Submit single cell bg color in the case we didn't submit a full header row
			if ((table->RowFlags & ImGuiTableRowFlags_Headers) == 0)
				ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(ImGuiCol_TableHeaderBg), table->CurrentColumn);
		}
		if (held)
			table->HeldHeaderColumn = (ImGuiTableColumnIdx)column_n;
		window->DC.CursorPos.y -= g.Style.ItemSpacing.y * 0.5f;

		// Drag and drop to re-order columns.
		// FIXME-TABLE: Scroll request while reordering a column and it lands out of the scrolling zone.
		if (held && (table->Flags & ImGuiTableFlags_Reorderable) && ImGui::IsMouseDragging(0) && !g.DragDropActive) {
			// While moving a column it will jump on the other side of the mouse, so we also test for MouseDelta.x
			table->ReorderColumn = (ImGuiTableColumnIdx)column_n;
			table->InstanceInteracted = table->InstanceCurrent;

			// We don't reorder: through the frozen<>unfrozen line, or through a column that is marked with ImGuiTableColumnFlags_NoReorder.
			if (g.IO.MouseDelta.x < 0.0f && g.IO.MousePos.x < cell_r.Min.x)
				if (ImGuiTableColumn* prev_column = (column->PrevEnabledColumn != -1) ? &table->Columns[column->PrevEnabledColumn] : NULL)
					if (!((column->Flags | prev_column->Flags) & ImGuiTableColumnFlags_NoReorder))
						if ((column->IndexWithinEnabledSet < table->FreezeColumnsRequest) == (prev_column->IndexWithinEnabledSet < table->FreezeColumnsRequest))
							table->ReorderColumnDir = -1;
			if (g.IO.MouseDelta.x > 0.0f && g.IO.MousePos.x > cell_r.Max.x)
				if (ImGuiTableColumn* next_column = (column->NextEnabledColumn != -1) ? &table->Columns[column->NextEnabledColumn] : NULL)
					if (!((column->Flags | next_column->Flags) & ImGuiTableColumnFlags_NoReorder))
						if ((column->IndexWithinEnabledSet < table->FreezeColumnsRequest) == (next_column->IndexWithinEnabledSet < table->FreezeColumnsRequest))
							table->ReorderColumnDir = +1;
		}

		// Sort order arrow
		const float ellipsis_max = cell_r.Max.x - w_arrow - w_sort_text;
		if ((table->Flags & ImGuiTableFlags_Sortable) && !(column->Flags & ImGuiTableColumnFlags_NoSort)) {
			if (column->SortOrder != -1) {
				float x = ImMax(cell_r.Min.x, cell_r.Max.x - w_arrow - w_sort_text);
				float y = label_pos.y;
				if (column->SortOrder > 0) {
					ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImGuiCol_Text, 0.70f));
					ImGui::RenderText(ImVec2(x + g.Style.ItemInnerSpacing.x, y), sort_order_suf);
					ImGui::PopStyleColor();
					x += w_sort_text;
				}
				ImGui::RenderArrow(window->DrawList, ImVec2(x, y), ImGui::GetColorU32(ImGuiCol_Text),
				                   column->SortDirection == ImGuiSortDirection_Ascending ? ImGuiDir_Up : ImGuiDir_Down, ARROW_SCALE);
			}

			// Handle clicking on column header to adjust Sort Order
			if (pressed && table->ReorderColumn != column_n) {
				ImGuiSortDirection sort_direction = ImGui::TableGetColumnNextSortDirection(column);
				ImGui::TableSetColumnSortDirection(column_n, sort_direction, g.IO.KeyShift);
			}
		}

		// Render clipped label. Clipping here ensure that in the majority of situations, all our header cells will
		// be merged into a single draw call.
		//window->DrawList->AddCircleFilled(ImVec2(ellipsis_max, label_pos.y), 40, IM_COL32_WHITE);
		if (show_label) {
			// ImGui::RenderTextEllipsis(window->DrawList, label_pos, ImVec2(ellipsis_max, label_pos.y + label_height + g.Style.FramePadding.y), ellipsis_max,
			// 	ellipsis_max, label, label_end, &label_size);

			float newX = label_pos.x;

			switch (alignment) {
				case Alignment::Center:
					// newX = label_pos.x + ((ellipsis_max - label_pos.x) / 2) - (label_size.x / 2);
					newX = label_pos.x + ((cell_r.Max.x - label_pos.x) / 2) - (label_size.x / 2);
					// ImGui::SetCursorPosX(cursorPosX + (textSpace / 2 - contentSize.x / 2));
					break;
				case Alignment::Right:
					newX = ellipsis_max - label_size.x;
					// ImGui::SetCursorPosX(cursorPosX + textSpace - contentSize.x);
					break;
				default: [[fallthrough]];
			}

			ImGui::RenderTextEllipsis(window->DrawList, ImVec2(newX, label_pos.y), ImVec2(ellipsis_max, label_pos.y + label_height + g.Style.FramePadding.y),
			                          ellipsis_max,
			                          ellipsis_max, label, label_end, &label_size);
		} else {
			float newX = label_pos.x;

			switch (alignment) {
				case Alignment::Center:
					// newX = label_pos.x + ((ellipsis_max - label_pos.x) / 2) - (image_size / 2);
					newX = label_pos.x + ((cell_r.Max.x - label_pos.x) / 2) - (image_size / 2);
					// ImGui::SetCursorPosX(cursorPosX + (textSpace / 2 - contentSize.x / 2));
					break;
				case Alignment::Right:
					newX = ellipsis_max - image_size;
					// ImGui::SetCursorPosX(cursorPosX + textSpace - contentSize.x);
					break;
				default: [[fallthrough]];
			}

			ImRect ibb(ImVec2(newX, label_pos.y), ImVec2(newX, label_pos.y) + image_size);

			window->DrawList->AddImage(texture, ibb.Min, ibb.Max);
		}

		// const bool text_clipped = label_size.x > (ellipsis_max - label_pos.x);
		// if (text_clipped && hovered && g.HoveredIdNotActiveTimer > g.TooltipSlowDelay)
		// ImGui::SetTooltip("%.*s", (int)(label_end - label), label);
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("%s", label);
		}

		// We don't use BeginPopupContextItem() because we want the popup to stay up even after the column is hidden
		// if (ImGui::IsMouseReleased(1) && ImGui::IsItemHovered())
		// ImGui::TableOpenContextMenu(column_n);
	}

	// This code can be used to make the text over the progressBar aligned.
	// This also uses imgui internals, which are likely to change between versions.
	void AlignedProgressBar(float fraction, const ImVec2& size_arg, const char* overlay, Alignment alignment) {
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size = ImGui::CalcItemSize(size_arg, ImGui::CalcItemWidth(), g.FontSize + style.FramePadding.y * 2.0f);
		ImRect bb(pos, pos + size);
		ImGui::ItemSize(size, style.FramePadding.y);
		if (!ImGui::ItemAdd(bb, 0))
			return;

		// Render
		fraction = ImSaturate(fraction);
		ImGui::RenderFrame(bb.Min, bb.Max, ImGui::GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);
		bb.Expand(ImVec2(-style.FrameBorderSize, -style.FrameBorderSize));
		const ImVec2 fill_br = ImVec2(ImLerp(bb.Min.x, bb.Max.x, fraction), bb.Max.y);
		ImGui::RenderRectFilledRangeH(window->DrawList, bb, ImGui::GetColorU32(ImGuiCol_PlotHistogram), 0.0f, fraction, style.FrameRounding);

		// Default displaying the fraction as percentage string, but user can override it
		char overlay_buf[32];
		if (!overlay) {
			ImFormatString(overlay_buf, IM_ARRAYSIZE(overlay_buf), "%.0f%%", fraction * 100 + 0.01f);
			overlay = overlay_buf;
		}

		ImVec2 overlay_size = ImGui::CalcTextSize(overlay, NULL);
		if (overlay_size.x > 0.0f) {
			switch (alignment) {
				case Alignment::Left:
					ImGui::RenderTextClipped(bb.Min, bb.Max, overlay, NULL, &overlay_size, ImVec2(0.f, 0.f), &bb);
					break;
				case Alignment::Center:
					ImGui::RenderTextClipped(bb.Min, bb.Max, overlay, NULL, &overlay_size, ImVec2(0.5f, 0.5f), &bb);
					break;
				case Alignment::Right:
					ImGui::RenderTextClipped(bb.Min, bb.Max, overlay, NULL, &overlay_size, ImVec2(1.f, 0.f), &bb);
					break;
				default:
					ImGui::RenderTextClipped(
						ImVec2(ImClamp(fill_br.x + style.ItemSpacing.x, bb.Min.x, bb.Max.x - overlay_size.x - style.ItemInnerSpacing.x), bb.Min.y), bb.Max,
						overlay, NULL, &overlay_size, ImVec2(0.0f, 0.5f), &bb);
			}
		}
	}

	/**
	 * Example usage:
	 * ```
	    // for child windows
		ImGui::BeginChild("boonTableSettings", ImVec2(0, ImGui::GetTextLineHeight()));
		bool hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_ChildWindows);
		if (ImGui::BeginMenu("##boon-table-settings")) {
		    if (!hovered) {
		        ImGui::CloseCurrentPopup();
		    }
		    settingsUi.Draw();
		    ImGui::EndMenu();
		}
		ImGui::EndChild();

		// for menus without child
		bool hovered;
		if (ImGuiEx::BeginMenu("boon-table-settings", true, hovered)) {
		    settingsUi.Draw();

		    if (!(ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_ChildWindows) || hovered) && !ImGui::IsAnyMouseDown()) {
		        ImGui::CloseCurrentPopup();
		    }

		    ImGui::EndMenu();
		}
		```
	 */
	bool BeginMenu(const char* label, bool enabled, bool& hoveredPar) {
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		bool menu_is_open = ImGui::IsPopupOpen(id, ImGuiPopupFlags_None);

		// Sub-menus are ChildWindow so that mouse can be hovering across them (otherwise top-most popup menu would steal focus and not allow hovering on parent menu)
		ImGuiWindowFlags flags = ImGuiWindowFlags_ChildMenu | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoNavFocus;
		if (window->Flags & (ImGuiWindowFlags_Popup | ImGuiWindowFlags_ChildMenu))
			flags |= ImGuiWindowFlags_ChildWindow;

		// If a menu with same the ID was already submitted, we will append to it, matching the behavior of Begin().
		// We are relying on a O(N) search - so O(N log N) over the frame - which seems like the most efficient for the expected small amount of BeginMenu() calls per frame.
		// If somehow this is ever becoming a problem we can switch to use e.g. ImGuiStorage mapping key to last frame used.
		if (g.MenusIdSubmittedThisFrame.contains(id)) {
			if (menu_is_open)
				menu_is_open = ImGui::BeginPopupEx(id, flags); // menu_is_open can be 'false' when the popup is completely clipped (e.g. zero size display)
			else
				g.NextWindowData.ClearFlags(); // we behave like Begin() and need to consume those values
			return menu_is_open;
		}

		// Tag menu as used. Next time BeginMenu() with same ID is called it will append to existing menu
		g.MenusIdSubmittedThisFrame.push_back(id);

		ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
		bool pressed;
		bool menuset_is_open = !(window->Flags & ImGuiWindowFlags_Popup) && (g.OpenPopupStack.Size > g.BeginPopupStack.Size && g.OpenPopupStack[g.
			BeginPopupStack.Size].OpenParentId == window->IDStack.back());
		ImGuiWindow* backed_nav_window = g.NavWindow;
		if (menuset_is_open)
			g.NavWindow = window; // Odd hack to allow hovering across menus of a same menu-set (otherwise we wouldn't be able to hover parent)

		// The reference position stored in popup_pos will be used by Begin() to find a suitable position for the child menu,
		// However the final position is going to be different! It is chosen by FindBestWindowPosForPopup().
		// e.g. Menus tend to overlap each other horizontally to amplify relative Z-ordering.
		ImVec2 popup_pos, pos = window->DC.CursorPos;
		if (window->DC.LayoutType == ImGuiLayoutType_Horizontal) {
			// Menu inside an horizontal menu bar
			// Selectable extend their highlight by half ItemSpacing in each direction.
			// For ChildMenu, the popup position will be overwritten by the call to FindBestWindowPosForPopup() in Begin()
			popup_pos = ImVec2(pos.x - 1.0f - IM_FLOOR(style.ItemSpacing.x * 0.5f), pos.y - style.FramePadding.y + window->MenuBarHeight());
			window->DC.CursorPos.x += IM_FLOOR(style.ItemSpacing.x * 0.5f);
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(style.ItemSpacing.x * 2.0f, style.ItemSpacing.y));
			float w = label_size.x;
			pressed = ImGui::Selectable(label, menu_is_open,
			                            ImGuiSelectableFlags_NoHoldingActiveID | ImGuiSelectableFlags_SelectOnClick | ImGuiSelectableFlags_DontClosePopups | (
				                            !enabled ? ImGuiSelectableFlags_Disabled : 0), ImVec2(w, 0.0f));
			ImGui::PopStyleVar();
			window->DC.CursorPos.x += IM_FLOOR(style.ItemSpacing.x * (-1.0f + 0.5f));
			// -1 spacing to compensate the spacing added when Selectable() did a SameLine(). It would also work to call SameLine() ourselves after the PopStyleVar().
		} else {
			// Menu inside a menu
			// (In a typical menu window where all items are BeginMenu() or MenuItem() calls, extra_w will always be 0.0f.
			//  Only when they are other items sticking out we're going to add spacing, yet only register minimum width into the layout system.
			popup_pos = ImVec2(pos.x, pos.y - style.WindowPadding.y);
			float min_w = window->DC.MenuColumns.DeclColumns(label_size.x, 0.0f, IM_FLOOR(g.FontSize * 1.20f)); // Feedback to next frame
			float extra_w = ImMax(0.0f, ImGui::GetContentRegionAvail().x - min_w);
			pressed = ImGui::Selectable(label, menu_is_open,
			                            ImGuiSelectableFlags_NoHoldingActiveID | ImGuiSelectableFlags_SelectOnClick | ImGuiSelectableFlags_DontClosePopups |
			                            ImGuiSelectableFlags_SpanAvailWidth | (!enabled ? ImGuiSelectableFlags_Disabled : 0), ImVec2(min_w, 0.0f));
			ImU32 text_col = ImGui::GetColorU32(enabled ? ImGuiCol_Text : ImGuiCol_TextDisabled);
			ImGui::RenderArrow(window->DrawList, pos + ImVec2(window->DC.MenuColumns.Pos[2] + extra_w + g.FontSize * 0.30f, 0.0f), text_col, ImGuiDir_Right);
		}

		const bool hovered = enabled && ImGui::ItemHoverable(window->DC.LastItemRect, id);
		hoveredPar = hovered;
		if (menuset_is_open)
			g.NavWindow = backed_nav_window;

		bool want_open = false;
		bool want_close = false;
		if (window->DC.LayoutType == ImGuiLayoutType_Vertical) // (window->Flags & (ImGuiWindowFlags_Popup|ImGuiWindowFlags_ChildMenu))
		{
			// Close menu when not hovering it anymore unless we are moving roughly in the direction of the menu
			// Implement http://bjk5.com/post/44698559168/breaking-down-amazons-mega-dropdown to avoid using timers, so menus feels more reactive.
			bool moving_toward_other_child_menu = false;

			ImGuiWindow* child_menu_window = (g.BeginPopupStack.Size < g.OpenPopupStack.Size && g.OpenPopupStack[g.BeginPopupStack.Size].SourceWindow == window)
				                                 ? g.OpenPopupStack[g.BeginPopupStack.Size].Window
				                                 : NULL;
			if (g.HoveredWindow == window && child_menu_window != NULL && !(window->Flags & ImGuiWindowFlags_MenuBar)) {
				// FIXME-DPI: Values should be derived from a master "scale" factor.
				ImRect next_window_rect = child_menu_window->Rect();
				ImVec2 ta = g.IO.MousePos - g.IO.MouseDelta;
				ImVec2 tb = (window->Pos.x < child_menu_window->Pos.x) ? next_window_rect.GetTL() : next_window_rect.GetTR();
				ImVec2 tc = (window->Pos.x < child_menu_window->Pos.x) ? next_window_rect.GetBL() : next_window_rect.GetBR();
				float extra = ImClamp(ImFabs(ta.x - tb.x) * 0.30f, 5.0f, 30.0f); // add a bit of extra slack.
				ta.x += (window->Pos.x < child_menu_window->Pos.x) ? -0.5f : +0.5f; // to avoid numerical issues
				tb.y = ta.y + ImMax((tb.y - extra) - ta.y, -100.0f);
				// triangle is maximum 200 high to limit the slope and the bias toward large sub-menus // FIXME: Multiply by fb_scale?
				tc.y = ta.y + ImMin((tc.y + extra) - ta.y, +100.0f);
				moving_toward_other_child_menu = ImTriangleContainsPoint(ta, tb, tc, g.IO.MousePos);
				//GetForegroundDrawList()->AddTriangleFilled(ta, tb, tc, moving_within_opened_triangle ? IM_COL32(0,128,0,128) : IM_COL32(128,0,0,128)); // [DEBUG]
			}
			if (menu_is_open && !hovered && g.HoveredWindow == window && g.HoveredIdPreviousFrame != 0 && g.HoveredIdPreviousFrame != id && !
				moving_toward_other_child_menu)
				want_close = true;

			if (!menu_is_open && hovered && pressed) // Click to open
				want_open = true;
			else if (!menu_is_open && hovered && !moving_toward_other_child_menu) // Hover to open
				want_open = true;

			if (g.NavActivateId == id) {
				want_close = menu_is_open;
				want_open = !menu_is_open;
			}
			if (g.NavId == id && g.NavMoveRequest && g.NavMoveDir == ImGuiDir_Right) // Nav-Right to open
			{
				want_open = true;
				ImGui::NavMoveRequestCancel();
			}
		} else {
			// Menu bar
			if (menu_is_open && pressed && menuset_is_open) // Click an open menu again to close it
			{
				want_close = true;
				want_open = menu_is_open = false;
			} else if (pressed || (hovered && menuset_is_open && !menu_is_open)) // First click to open, then hover to open others
			{
				want_open = true;
			} else if (g.NavId == id && g.NavMoveRequest && g.NavMoveDir == ImGuiDir_Down) // Nav-Down to open
			{
				want_open = true;
				ImGui::NavMoveRequestCancel();
			}
		}

		if (!enabled)
			// explicitly close if an open menu becomes disabled, facilitate users code a lot in pattern such as 'if (BeginMenu("options", has_object)) { ..use object.. }'
			want_close = true;
		if (want_close && ImGui::IsPopupOpen(id, ImGuiPopupFlags_None))
			ImGui::ClosePopupToLevel(g.BeginPopupStack.Size, true);

		IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.ItemFlags | ImGuiItemStatusFlags_Openable | (menu_is_open ? ImGuiItemStatusFlags_Opened : 0));

		if (!menu_is_open && want_open && g.OpenPopupStack.Size > g.BeginPopupStack.Size) {
			// Don't recycle same menu level in the same frame, first close the other menu and yield for a frame.
			ImGui::OpenPopup(label);
			return false;
		}

		menu_is_open |= want_open;
		if (want_open)
			ImGui::OpenPopup(label);

		if (menu_is_open) {
			ImGui::SetNextWindowPos(popup_pos, ImGuiCond_Always);
			menu_is_open = ImGui::BeginPopupEx(id, flags); // menu_is_open can be 'false' when the popup is completely clipped (e.g. zero size display)
		} else {
			g.NextWindowData.ClearFlags(); // We behave like Begin() and need to consume those values
		}

		return menu_is_open;
	}

	void BeginMenuChild(const char* child_str_id, const char* menu_label, std::function<void()> draw_func) {
		ImGui::BeginChild(child_str_id, ImVec2(0, ImGui::GetTextLineHeight()));
		bool hoveredChild = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_ChildWindows);

		if (ImGui::BeginMenu(menu_label)) {
			draw_func();

			bool hovoredMenu = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_ChildWindows);

			if (!hoveredChild && !hovoredMenu && !ImGui::IsAnyMouseDown()) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndMenu();
		}
		ImGui::EndChild();
	}

	void BeginMenu(const char* menu_label, std::function<void()> draw_func) {
		bool hovered;
		if (ImGuiEx::BeginMenu(menu_label, true, hovered)) {
			draw_func();

			if (!(ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_ChildWindows) || hovered) && !ImGui::IsAnyMouseDown()) {
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndMenu();
		}
	}

	bool BeginPopupContextWindow(const char* str_id, ImGuiPopupFlags popup_flags, ImGuiHoveredFlags hovered_flags) {
		ImGuiWindow* window = GImGui->CurrentWindow;
		if (!str_id)
			str_id = "window_context";
		ImGuiID id = window->GetID(str_id);
		int mouse_button = (popup_flags & ImGuiPopupFlags_MouseButtonMask_);
		hovered_flags |= ImGuiHoveredFlags_AllowWhenBlockedByPopup;
		if (ImGui::IsMouseReleased(mouse_button) && ImGui::IsWindowHovered(hovered_flags))
			if (!(popup_flags & ImGuiPopupFlags_NoOpenOverItems) || !ImGui::IsAnyItemHovered())
				ImGui::OpenPopupEx(id, popup_flags);
		return ImGui::BeginPopupEx(id, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings);
	}

	void MenuItemTableColumnVisibility(ImGuiTable* table, int columnIdx) {
		ImGuiTableColumn& column = table->Columns[columnIdx];
		const char* columnName = ImGui::TableGetColumnName(table, columnIdx);
		// Make sure we can't hide the last active column
		bool menu_item_active = (column.Flags & ImGuiTableColumnFlags_NoHide) ? false : true;
		if (column.IsEnabled && table->ColumnsEnabledCount <= 1)
			menu_item_active = false;
		if (ImGui::MenuItem(columnName, NULL, column.IsEnabled, menu_item_active))
			column.IsEnabledNextFrame = !column.IsEnabled;
	}

	void WindowReposition(Position position, const ImVec2& cornerVector, CornerPosition cornerPosition, ImGuiID fromWindowID,
	                      CornerPosition anchorPanelCornerPosition, CornerPosition selfPanelCornerPosition) {
		const ImVec2& windowSize = ImGui::GetWindowSize();
		const ImVec2& displaySize = ImGui::GetIO().DisplaySize;

		switch (position) {
			case Position::ScreenRelative: {
				ImVec2 setPosition;
				const ImVec2& userPosition = cornerVector;
				switch (cornerPosition) {
					case CornerPosition::TopLeft: {
						setPosition = userPosition;
						break;
					}
					case CornerPosition::TopRight: {
						setPosition.x = displaySize.x - windowSize.x - userPosition.x;
						setPosition.y = userPosition.y;
						break;
					}
					case CornerPosition::BottomLeft: {
						setPosition.x = userPosition.x;
						setPosition.y = displaySize.y - windowSize.y - userPosition.y;
						break;
					}
					case CornerPosition::BottomRight: {
						setPosition = displaySize - windowSize - userPosition;
						break;
					}
				}

				ImGui::SetWindowPos(setPosition);
				break;
			}
			case Position::WindowRelative: {
				ImVec2 setPosition;
				const ImVec2& userPosition = cornerVector;
				ImGuiWindow* sourceWindow = ImGui::FindWindowByID(fromWindowID);
				if (!sourceWindow) {
					break;
				}
				const ImVec2& sourceWindowPosition = sourceWindow->Pos;
				const ImVec2& sourceWindowSize = sourceWindow->Size;
				switch (anchorPanelCornerPosition) {
					case CornerPosition::TopLeft: {
						setPosition = sourceWindowPosition;
						break;
					}
					case CornerPosition::TopRight: {
						setPosition.x = sourceWindowPosition.x + sourceWindowSize.x;
						setPosition.y = sourceWindowPosition.y;
						break;
					}
					case CornerPosition::BottomLeft: {
						setPosition.x = sourceWindowPosition.x;
						setPosition.y = sourceWindowPosition.y + sourceWindowSize.y;
						break;
					}
					case CornerPosition::BottomRight: {
						setPosition.x = sourceWindowPosition.x + sourceWindowSize.x;
						setPosition.y = sourceWindowPosition.y + sourceWindowSize.y;
						break;
					}
				}
				switch (selfPanelCornerPosition) {
					case CornerPosition::TopLeft: {
						setPosition = setPosition + userPosition;
						break;
					}
					case CornerPosition::TopRight: {
						setPosition.x = setPosition.x + userPosition.x - windowSize.x;
						setPosition.y = setPosition.y + userPosition.y;
						break;
					}
					case CornerPosition::BottomLeft: {
						setPosition.x = setPosition.x + userPosition.x;
						setPosition.y = setPosition.y + userPosition.y - windowSize.y;
						break;
					}
					case CornerPosition::BottomRight: {
						setPosition = setPosition + userPosition - windowSize;
						break;
					}
				}

				// clip to screen border
				setPosition = ImMax(setPosition, ImVec2(0.f, 0.f));

				ImGui::SetWindowPos(setPosition);
				break;
			}
		}
	}
}
