#include "UpdateCheckerBase.h"

#include <charconv>
#include <optional>
#include <thread>
#include <sstream>

#include "json.hpp"

void UpdateCheckerBase::ClearFiles(HMODULE dll) {
	CHAR dllPath[MAX_PATH] = { 0 };
	if (GetModuleFileNameA(dll, dllPath, _countof(dllPath)) == 0) {
		return;
	}

	std::string dllPathTemp(dllPath);
	dllPathTemp.append(".tmp");

	std::remove(dllPathTemp.c_str());

	std::string dllPathOld(dllPath);
	dllPathOld.append(".old");

	std::remove(dllPathOld.c_str());
}

std::optional<UpdateCheckerBase::Version> UpdateCheckerBase::GetCurrentVersion(HMODULE dll) {
	// GetModuleFileName
	TCHAR moduleFileName[MAX_PATH + 1]{ };
	if (!GetModuleFileName(dll, moduleFileName, MAX_PATH)) {
		return std::nullopt;
	}

	// GetFileVersionInfoSize
	DWORD dummy; // this will get set to 0 (wtf windows api)
	DWORD versionInfoSize = GetFileVersionInfoSize(moduleFileName, &dummy);
	if (versionInfoSize == 0) {
		return std::nullopt;
	}
	std::vector<BYTE> data(versionInfoSize);

	// GetFileVersionInfo
	if (!GetFileVersionInfo(moduleFileName, NULL, versionInfoSize, &data[0])) {
		return std::nullopt;
	}

	UINT randomPointer = 0;
	VS_FIXEDFILEINFO* fixedFileInfo = nullptr;
	if (!VerQueryValue(&data[0], TEXT("\\"), (void**)&fixedFileInfo, &randomPointer)) {
		return std::nullopt;
	}

	return Version ({
		HIWORD(fixedFileInfo->dwProductVersionMS),
		LOWORD(fixedFileInfo->dwProductVersionMS),
		HIWORD(fixedFileInfo->dwProductVersionLS),
		LOWORD(fixedFileInfo->dwProductVersionLS)
	});
}

std::string UpdateCheckerBase::GetVersionAsString(HMODULE dll) {
	auto currentVersion = UpdateCheckerBase::GetCurrentVersion(dll);
	std::stringstream version;
	if (currentVersion) {
		version << currentVersion->at(0) << "." << currentVersion->at(1) << "." << currentVersion->at(2) << "." << currentVersion->at(3);
	}
	else {
		version << __DATE__;
	}
	return version.str();
}

#ifndef ARCDPS_EXTENSION_NO_CPR
#pragma warning(push)
#pragma warning(disable: 4996) //  error C4996: '_Header_cstdbool': warning STL4004: <ccomplex>, <cstdalign>, <cstdbool>, and <ctgmath> are deprecated in C++17.
#include <cpr/cpr.h>
#pragma warning(pop)

void UpdateCheckerBase::CheckForUpdate(Version currentVersion, std::string repo, bool allowPrerelease) {
	version = currentVersion;

	std::thread cprCall([this, repo, allowPrerelease]() {
		nlohmann::basic_json<> json;
		nlohmann::basic_json<>* release;
		if (!allowPrerelease) {
			std::string link = "https://api.github.com/repos/";
			link.append(repo);
			link.append("/releases/latest");

			auto response = HttpGet(link);
			if (!response.has_value()) {
				return;
			}

			json = nlohmann::json::parse(response.value());
			release = &json;
		} else {
			std::string link = "https://api.github.com/repos/";
			link.append(repo);
			link.append("/releases");

			auto response = HttpGet(link);
			if (!response.has_value()) {
				return;
			}

			json = nlohmann::json::parse(response.value());
			release = &json[0];
		}

		const auto tagName = release->at("tag_name").get<std::string>();
		newVersion = ParseVersion(tagName);

		// load download URL (use the first asset that has a .dll ending)
		for (const auto& item : (*release)["assets"]) {
			const auto assetName = item["name"].get<std::string>();
			if (assetName.size() < 4) {
				continue;
			}

			if (std::string_view(assetName).substr(assetName.size() - 4) == ".dll")	{
				downloadUrl = item["browser_download_url"].get<std::string>();
				//LogD("Found download url in {} - {}", assetName, downloadUrl);
			}
		}

		if (IsNewer(newVersion, version)) {
			Status expected = Status::Unknown;
			update_status.compare_exchange_strong(expected, Status::UpdateAvailable);
		}
	});

	cprCall.detach();
}

std::optional<std::string> UpdateCheckerBase::HttpGet(const std::string& url) {
	cpr::Response response = cpr::Get(cpr::Url{ url });

	if (response.status_code != 200) {
		//LogD("Getting {} failed {} {}", url, response.status_code, response.status_line);
		return std::nullopt;
	}

	return response.text;
}

void UpdateCheckerBase::UpdateAutomatically(HMODULE dll) {
	if (downloadUrl.empty()) return;

	Status expected = Status::UpdateAvailable;
	if (!update_status.compare_exchange_strong(expected, Status::UpdatingInProgress))
		return;

	std::thread t([this, dll]() {
		cpr::Session session;
		session.SetUrl(cpr::Url{ downloadUrl });

		CHAR dllPath[MAX_PATH] = { 0 };
		if (GetModuleFileNameA(dll, dllPath, _countof(dllPath)) == 0) {
			update_status = Status::UpdateError;
			return;
		}

		std::string dllPathTemp(dllPath);
		dllPathTemp.append(".tmp");

		// new context to have ofstream and session only temporary (they have to be closed later on)
		{
			std::ofstream outFile(dllPathTemp, std::ios::binary);

			cpr::Response response = session.Download(outFile);
			if (response.status_code != 200) {
				// Error downloading
				update_status = Status::UpdateError;
				return;
			}
		}

		std::string dllPathOld(dllPath);
		dllPathOld.append(".old");
		
		if (std::rename(dllPath, dllPathOld.c_str())) {
			// Error renaming
			update_status = Status::UpdateError;
			return;
		}

		if (std::rename(dllPathTemp.c_str(), dllPath)) {
			// Error renaming
			update_status = Status::UpdateError;
			return;
		}

		update_status = Status::RestartPending;
	});

	t.detach();
}

bool UpdateCheckerBase::IsNewer(const Version& repoVersion, const Version& currentVersion) {
	return std::tie(currentVersion[0], currentVersion[1], currentVersion[2])
	     < std::tie(repoVersion[0], repoVersion[1], repoVersion[2]);
}

UpdateCheckerBase::Version UpdateCheckerBase::ParseVersion(std::string_view versionString) {
	// TODO: use semver to calculate this. So all semver releases can be parsed and not only my hardcoded ones :)
	// libary for it: https://github.com/Neargye/semver

	Version result{};
	size_t tokenIndex = 0;
	size_t start = 0;
	do {
		size_t dotPos = versionString.find('.', start);
		if (dotPos == std::string_view::npos) {
			dotPos = versionString.size();
		}
			
		std::string_view token_str = versionString.substr(start, dotPos - start);

		// Remove all non-digit characters from the beginning of the token
		while (true) {
			if (token_str.empty()) {
				break;
			}
			if (isdigit(*token_str.begin())) {
				break;
			}
			token_str = token_str.substr(1);
		}

		// Parse the str token to an integer
		const auto from_chars_result = std::from_chars(
			token_str.data(),
			token_str.data() + token_str.size(),
			result[tokenIndex]);
		if (from_chars_result.ec != std::errc{}) {
			//LogD("Parsing version token '{}' from '{}' failed", versionString, token_str);
		} else {
			tokenIndex++;
		}

		start = dotPos + 1; // + 1 to skip over the '.'
	} while(start < versionString.size() && tokenIndex < 3);
	
	if (tokenIndex < 3) {
		//LogD("Failed to parse version from {} - only found {} tokens", versionString, tokenIndex);
		return Version{};
	}

	return result;
}


#endif // ARCDPS_EXTENSION_NO_CPR
