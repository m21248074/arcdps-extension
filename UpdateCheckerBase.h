#pragma once

#include <optional>
#include <Windows.h>

#include "../imgui/imgui.h"

class UpdateCheckerBase {
public:
	void checkForUpdate(HMODULE dll);
	virtual void Draw() = 0;

	static std::optional<ImVec4> GetCurrentVersion(HMODULE dll);

protected:
	bool update_available = false;
	ImVec4 version;
	ImVec4 newVersion;
	bool shown = true;
};
