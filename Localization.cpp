#include "Localization.h"

#include "ExtensionTranslations.h"

ArcdpsExtension::Localization::Localization() {
	Load(Lang::English, EXTENSION_TRANSLATION_ENGLISH);
	Load(Lang::German, EXTENSION_TRANSLATION_GERMAN);
	Load(Lang::Spanish, EXTENSION_TRANSLATION_SPANISH);
	Load(Lang::French, EXTENSION_TRANSLATION_FRENCH);
	Load(Lang::Chinese, EXTENSION_TRANSLATION_CHINESE);

	mCurrentTranslation = &mTranslations.at(Lang::English);
	mFallbackTranslation = &mTranslations.at(Lang::English);
}

std::string_view ArcdpsExtension::Localization::Translate(size_t pId) const {
	const auto it = mCurrentTranslation->find(pId);
	if (it == mCurrentTranslation->end()) {
		return mFallbackTranslation->at(pId);
	}
	return it->second;
}

std::string_view ArcdpsExtension::Localization::Translate(const std::string& pLang) const {
	return mTranslations.at(pLang).at(ET_LanguageName);
}

void ArcdpsExtension::Localization::ChangeLanguage(const std::string& pLang) {
	mCurrentTranslation = &mTranslations.at(pLang);
}

void ArcdpsExtension::Localization::SChangeLanguage(const std::string& pLang) {
	Localization::instance().ChangeLanguage(pLang);
}
void ArcdpsExtension::Localization::ChangeFallbackLanguage(const std::string& pLang) {
	mFallbackTranslation = &mTranslations.at(pLang);
}
void ArcdpsExtension::Localization::SChangeFallbackLanguage(const std::string& pLang) {
	Localization::instance().ChangeFallbackLanguage(pLang);
}
