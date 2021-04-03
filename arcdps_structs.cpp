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
#endif

bool is_player(ag* new_player)
{
	return new_player
		&& new_player->elite != 0xffffffff
		&& new_player->name
		&& std::string(new_player->name).length() > 1
		&& new_player->id;
}
