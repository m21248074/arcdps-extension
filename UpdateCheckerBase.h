#pragma once

#include <optional>
#include <string>
#include <Windows.h>

#include "../imgui/imgui.h"

class UpdateCheckerBase {
public:
	void checkForUpdate(HMODULE dll, std::string repo);
	virtual void Draw() = 0;

	static std::optional<ImVec4> GetCurrentVersion(HMODULE dll);
	static char* GetVersionAsString(HMODULE dll);

protected:
	bool update_available = false;
	ImVec4 version;
	ImVec4 newVersion;
	bool shown = true;
};
