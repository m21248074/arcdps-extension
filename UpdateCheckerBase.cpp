#include "UpdateCheckerBase.h"

#include <thread>
#include <cpr/cpr.h>

#include "json.hpp"

void UpdateCheckerBase::checkForUpdate(HMODULE dll) {
    if (!getCurrentVersion(dll)) return;

    std::thread cprCall([this]() {
        std::string link = "https://api.github.com/repos/knoxfighter/GW2-ArcDPS-Boon-Table/releases/latest";

        cpr::Response response = cpr::Get(cpr::Url{ link }, cpr::Header{ {"User-Agent", "arcdps-killproof.me-plugin"} });

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

            newVersion.x = std::stof(versionNums[0]);
            newVersion.y = std::stof(versionNums[1]);
            newVersion.z = std::stof(versionNums[2]);

            if (newVersion.x > version.x || newVersion.y > version.y || newVersion.z > version.z) {
                update_available = true;
            }
        }
    });

    cprCall.detach();
}

bool UpdateCheckerBase::getCurrentVersion(HMODULE dll) {
    // GetModuleFileName
    TCHAR moduleFileName[MAX_PATH + 1]{ };
    if (!GetModuleFileName(dll, moduleFileName, MAX_PATH)) {
        return false;
    }

    // GetFileVersionInfoSize
    DWORD dummy; // this will get set to 0 (wtf windows api)
    DWORD versionInfoSize = GetFileVersionInfoSize(moduleFileName, &dummy);
    if (versionInfoSize == 0) {
        return false;
    }
    std::vector<BYTE> data(versionInfoSize);

    // GetFileVersionInfo
    if (!GetFileVersionInfo(moduleFileName, NULL, versionInfoSize, &data[0])) {
        return false;
    }

    UINT randomPointer = 0;
    VS_FIXEDFILEINFO* fixedFileInfo = nullptr;
    if (!VerQueryValue(&data[0], TEXT("\\"), (void**)&fixedFileInfo, &randomPointer)) {
        return false;
    }

    version = ImVec4(
        HIWORD(fixedFileInfo->dwProductVersionMS),
        LOWORD(fixedFileInfo->dwProductVersionMS),
        HIWORD(fixedFileInfo->dwProductVersionLS),
        LOWORD(fixedFileInfo->dwProductVersionLS)
    );

    return true;
}
