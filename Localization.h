#pragma once

#include "arcdps_structs_slim.h"
#include "Singleton.h"

#include <vector>
#include <string>
#include <type_traits>
#include <concepts>
#include <array>

/**
 * How to use:
 * Call `Load()` with your translations you want to add.
 * Things are added to the back of the list with all `Extension` based translations at the beginning.
 * So make sure, your IDs enum or whatever you use, starts with one higher than the last used one.
 * Also make sure you add all 4 translations and they are of the same size.
 * Call it within `mod_init` or as early as possible.
 *
 * Call `ChangeLanguage` when you want to change the language.
 * Call it once to setup the language the user has in `mod_init`
 */
class Localization final : public Singleton<Localization> {
public:
	Localization();

	[[nodiscard]] const std::string& Translate(size_t pId) const;

    template <typename E>
    requires std::is_enum_v<E>
    [[nodiscard]] const std::string& Translate(E pId) const {
        return Translate(static_cast<std::underlying_type_t<E>>(pId));
	}

    template<typename Num>
    requires std::is_integral_v<Num> && (!std::same_as<Num, size_t>)
    [[nodiscard]] const std::string& Translate(Num pId) const {
        return Translate(static_cast<size_t>(pId));
	}

    template<typename T>
    [[nodiscard]] static const std::string& STranslate(T pId) {
		return Localization::instance().Translate(pId);
	}

    void Load(gwlanguage pLang, const std::ranges::common_range auto& pRange) {
	    auto& translation = mTranslations.at(pLang);
	    for (const auto& value : pRange) {
	        translation.emplace_back(value);
	    }
    }

    void ChangeLanguage(gwlanguage pLang);
    static void SChangeLanguage(gwlanguage pLang);

    void OverrideTranslation(gwlanguage pLanguage, size_t pTranslation, const std::string& pText);

    template <typename E>
    requires std::is_enum_v<E>
    const std::string& OverrideTranslation(gwlanguage pLanguage, E pTranslation, const std::string& pText) {
        return OverrideTranslation(pLanguage, static_cast<std::underlying_type_t<E>>(pTranslation), pText);
	}

    template<typename Num>
    requires std::is_integral_v<Num> && (!std::same_as<Num, size_t>)
    void OverrideTranslation(gwlanguage pLanguage, Num pTranslation, const std::string& pText) {
        return OverrideTranslation(pLanguage, static_cast<size_t>(pTranslation), pText);
	}

    template <typename T>
    void OverrideTranslation(gwlanguage pLanguage, T pTranslation, const std::string& pText) {
	    return OverrideTranslation(pLanguage, pTranslation, pText);
    }

private:
    std::array<std::vector<std::string>, 6> mTranslations;
    gwlanguage mCurrentLanguage = GWL_ENG;
	std::vector<std::string>* mCurrentTranslation;
};
