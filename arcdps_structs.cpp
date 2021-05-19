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
#endif

bool is_player(ag* new_player)
{
	return new_player
		&& new_player->elite != 0xffffffff
		&& new_player->name
		&& std::string(new_player->name).length() > 1
		&& new_player->id;
}
