#pragma once

#include <array>
#include <atomic>
#include <optional>
#include <string>
#include <Windows.h>

class UpdateCheckerBase {
public:
	using Version = std::array<WORD, 4>;
	
	enum class Status {
		Unknown,
		UpdateAvailable,
		UpdatingInProgress,
		RestartPending,
		UpdateError
	};
	
	/**
	 * \brief Check if a new update is available on the given github repo. Call this inside `mod_init()`.
	 * \param dll the module to this dll (self_dll) got from DllMain()
	 * \param repo string to the repo (e.g. `knoxfighter/arcdps-killproof.me-plugin`)
	 */
	void CheckForUpdate(HMODULE dll, std::string repo);

	/**
	 * \brief Draw command for running in `mod_imgui()`
	 */
	virtual void Draw() = 0;

	/**
	 * \brief Clear rest-files after automatic download (dll.tmp and dll.old)
	 * \param dll the module to this dll (self_dll) got from DllMain()
	 */
	static void ClearFiles(HMODULE dll);
	
	/**
	 * \brief get the current version as semver version. Version is set in VS_VERSION_INFO.
	 * \param dll the module to this dll (self_dll) got from DllMain()
	 * \return Version as semver compatible ImVec4
	 */
	static std::optional<Version> GetCurrentVersion(HMODULE dll);
	
	/**
	 * \brief get the current version as string. Version is read by `GetCurrentVersion(HMODULE)`.
	 * \param dll the module to this dll (self_dll) got from DllMain()
	 * \return Version as semver compatible string
	 */
	static std::string GetVersionAsString(HMODULE dll);

	/**
	 * \brief Update the dll automatically. current version will be renamed to `dll.old` and new one will be downloaded as `dll.tmp` and renamed to current dll name.
	 * \param dll the module to this dll (self_dll) got from DllMain()
	 */
	void UpdateAutomatically(HMODULE dll);

protected:
	std::atomic<Status> update_status = Status::Unknown;
	Version version {};
	Version newVersion {};
	bool shown = true;
	std::string downloadUrl;
};
