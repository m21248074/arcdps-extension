#pragma once

#include "UpdateCheckerBase.h"
#include "Singleton.h"

/**
 * Use this only if you can live with the Dependencies:
 * - ImGui directly
 * - Localization.h
 * - Singleton of this library (it has to be setup correctly as well)
 */
class UpdateChecker final : public UpdateCheckerBase, public Singleton<UpdateChecker> {
public:
	void Draw(const std::unique_ptr<UpdateState>& pUpdateState, const std::string& pPluginName, const std::string& pRepoReleaseLink);
};
