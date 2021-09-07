#include "UpdateCheckerBase.h"

#include <optional>
#include <thread>
#include <sstream>

#include "json.hpp"
bool compareFloat(float x, float y, float epsilon = FLT_EPSILON) {
	return std::abs(x - y) < epsilon;
}

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
#include <cpr/cpr.h>

void UpdateCheckerBase::CheckForUpdate(HMODULE dll, std::string repo) {
	auto currentVersion = GetCurrentVersion(dll);
	if (!currentVersion) return;
	version = currentVersion.value();

	std::thread cprCall([this, repo]() {
		std::string link = "https://api.github.com/repos/";
		link.append(repo);
		link.append("/releases/latest");

		cpr::Response response = cpr::Get(cpr::Url{ link });

		if (response.status_code == 200) {
			auto json = nlohmann::json::parse(response.text);

			std::string tagName = json.at("tag_name").get<std::string>();
			if (tagName[0] == 'v') {
				tagName.erase(0, 1);
			}

			std::vector<std::string> versionNums;
			std::istringstream iss(tagName);

			for (std::string token; std::getline(iss, token, '.'); )
			{
				versionNums.push_back(std::move(token));
			}

			// TODO: use semver to calculate this. So all semver releases can be parsed and not only my hardcoded ones :)
			// libary for it: https://github.com/Neargye/semver
			
			if (versionNums.size() < 3) return;

			newVersion[0] = std::stof(versionNums[0]);
			newVersion[1] = std::stof(versionNums[1]);
			newVersion[2] = std::stof(versionNums[2]);

			if (newVersion[0] > version[0] || newVersion[1] > version[1] || newVersion[2] > version[2]) {
				Status expected = Status::Unknown;
				update_status.compare_exchange_strong(expected, Status::UpdateAvailable);
			}

			// load download URL
			downloadUrl = json["assets"][0]["browser_download_url"].get<std::string>();
		}
	});

	cprCall.detach();
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

#endif
