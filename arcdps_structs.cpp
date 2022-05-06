#include "arcdps_structs.h"

#ifndef ARCDPS_EXTENSION_NO_LANG_H
#include "../Lang.h"
std::string to_string(Alignment alignment) {
	switch (alignment) {
	case Alignment::Left: return lang.translate(LangKey::Left);
	case Alignment::Center: return lang.translate(LangKey::Center);
	case Alignment::Right: return lang.translate(LangKey::Right);
	case Alignment::Unaligned: return lang.translate(LangKey::Unaligned);
	default: return "Unknown";
	}
}

std::string to_string(Position position) {
	switch (position) {
	case Position::Manual: return lang.translate(LangKey::PositionManual);
	case Position::ScreenRelative: return lang.translate(LangKey::PositionScreenRelative);
	case Position::WindowRelative: return lang.translate(LangKey::PositionWindowRelative);
	default: return "Unknown";
	}
}

std::string to_string(CornerPosition position) {
	switch (position) {
	case CornerPosition::TopLeft: return lang.translate(LangKey::CornerPositionTopLeft);
	case CornerPosition::TopRight: return lang.translate(LangKey::CornerPositionTopRight);
	case CornerPosition::BottomLeft: return lang.translate(LangKey::CornerPositionBottomLeft);
	case CornerPosition::BottomRight: return lang.translate(LangKey::CornerPositionBottomRight);
	}
}

std::string to_string(SizingPolicy sizingPolicy) {
	switch (sizingPolicy) {
	case SizingPolicy::SizeToContent: return lang.translate(LangKey::SizingPolicySizeToContent);
	case SizingPolicy::SizeContentToWindow: return lang.translate(LangKey::SizingPolicySizeContentToWindow);
	case SizingPolicy::ManualWindowSize: return lang.translate(LangKey::SizingPolicyManualWindowSize);
	default: return "Unknown";
	}
}
#else
std::string to_string(Alignment alignment) {
	switch (alignment) {
	case Alignment::Left: return "Left";
	case Alignment::Center: return "Center";
	case Alignment::Right: return "Right";
	case Alignment::Unaligned: return "Unaligned";
	default: return "Unknown";
	}
}

std::string to_string(Position position) {
	switch (position) {
	case Position::Manual: return "Manual";
	case Position::ScreenRelative: return "Screen Relative";
	case Position::WindowRelative: return "Window Relative";
	default: return "Unknown";
	}
}

std::string to_string(CornerPosition position) {
	switch (position) {
	case CornerPosition::TopLeft: return "Top-Left";
	case CornerPosition::TopRight: return "Top-Right";
	case CornerPosition::BottomLeft: return "Bottom-Left";
	case CornerPosition::BottomRight: return "Bottom-Right";
	}
}

std::string to_string(SizingPolicy sizingPolicy) {
	switch (sizingPolicy) {
	case SizingPolicy::SizeToContent: return "Size to Content";
	case SizingPolicy::SizeContentToWindow: return "Size Content to Window";
	case SizingPolicy::ManualWindowSize: return "Manuel Window Size";
	default: return "Unknown";
	}
}
#endif

bool is_player(ag* new_player)
{
	return new_player
		&& new_player->elite != 0xffffffff
		&& new_player->name
		&& std::string(new_player->name).length() > 1
		&& new_player->id;
}
