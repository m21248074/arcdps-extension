#include "ExtensionTranslations.h"
#include "Localization.h"
#include "arcdps_structs_slim.h"

#include <gtest/gtest.h>

using namespace ArcdpsExtension;

TEST(LocalizationTests, BaseTranslations) {
	Localization localization;

	localization.ChangeLanguage(Lang::English);
	ASSERT_EQ(localization.Translate(ET_Left), "Left");
	localization.ChangeLanguage(Lang::German);
	ASSERT_EQ(localization.Translate(ET_Left), "Links");
	localization.ChangeLanguage(Lang::French);
	ASSERT_EQ(localization.Translate(ET_Left), "Gauche");
	localization.ChangeLanguage(Lang::Spanish);
	ASSERT_EQ(localization.Translate(ET_Left), "Izquierda");
	localization.ChangeLanguage(Lang::Chinese);
	ASSERT_EQ(localization.Translate(ET_Left), "居左");
}

TEST(LocalizationTests, BaseTranslationsLang) {
	Localization localization;

	ASSERT_EQ(localization.Translate(Lang::English, ET_Left), "Left");
	ASSERT_EQ(localization.Translate(Lang::German, ET_Left), "Links");
	ASSERT_EQ(localization.Translate(Lang::French, ET_Left), "Gauche");
	ASSERT_EQ(localization.Translate(Lang::Spanish, ET_Left), "Izquierda");
	ASSERT_EQ(localization.Translate(Lang::Chinese, ET_Left), "居左");
}

TEST(LocalizationTests, BaseTranslationsSpecialChars) {
	Localization localization;

	localization.ChangeLanguage(Lang::German);
	ASSERT_EQ(localization.Translate(ET_SizingPolicyManualWindowSize), "Manuelle Fenstergröße");
	localization.ChangeLanguage(Lang::French);
	ASSERT_EQ(localization.Translate(ET_UpdateRestartPending), "La mise à jour automatique est terminée, redémarrez votre jeu pour l'activer.");
	localization.ChangeLanguage(Lang::Spanish);
	ASSERT_EQ(localization.Translate(ET_UpdateInProgress), "Actualización automática en curso");
	localization.ChangeLanguage(Lang::Chinese);
	ASSERT_EQ(localization.Translate(ET_SizingPolicySizeContentToWindow), "根据窗口大小调整内容大小");
}

TEST(LocalizationTests, OverrideTranslation) {
	Localization localization;

	localization.ChangeLanguage(Lang::English);
	ASSERT_EQ(localization.Translate(ET_Left), "Left");
	localization.AddTranslation(Lang::English, ET_Left, "Rêchts");
	ASSERT_EQ(localization.Translate(ET_Left), "Rêchts");
}
