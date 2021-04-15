#pragma once

#include <Windows.h>

#include "../imgui/imgui.h"

class UpdateCheckerBase {
public:
	void checkForUpdate(HMODULE dll);
	virtual void Draw() = 0;

protected:
	bool update_available = false;
	ImVec4 version;
	ImVec4 newVersion;
	bool shown = true;

	bool getCurrentVersion(HMODULE dll);
};
