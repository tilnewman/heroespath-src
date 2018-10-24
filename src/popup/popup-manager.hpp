// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_POPUP_POPUPMANAGER_HPP_INCLUDED
#define HEROESPATH_POPUP_POPUPMANAGER_HPP_INCLUDED
//
// popup-manager.hpp
//
#include "combat/combat-over-enum.hpp"
#include "creature/title.hpp"
#include "misc/boost-optional-that-throws.hpp"
#include "misc/not-null.hpp"
#include "popup/popup-enums.hpp"
#include "popup/popup-info.hpp"
#include "sfml-util/cached-texture.hpp"
#include "sfml-util/callback.hpp"
#include "sfml-util/font-manager.hpp"
#include "sfml-util/justified-enum.hpp"
#include "sfml-util/sound-effects-enum.hpp"

#include <memory>
#include <string>
#include <vector>

namespace heroespath
{

namespace creature
{
    class Creature;
    using CreaturePtr_t = misc::NotNull<Creature *>;
} // namespace creature

namespace popup
{

    // A class that loads, stores, and distributes fonts by style.
    class PopupManager
    {
    public:
        PopupManager(const PopupManager &) = delete;
        PopupManager(PopupManager &&) = delete;
        PopupManager & operator=(const PopupManager &) = delete;
        PopupManager & operator=(PopupManager &&) = delete;

        PopupManager();
        ~PopupManager();

        static misc::NotNull<PopupManager *> Instance();
        static void Acquire();
        static void Release();

        static void SetTexturesDirectoryPaths(
            const std::string & WINDOWS_PATH, const std::string & ACCENTS_PATH);

        bool Test();

        // throws range error on invalid PopupImage::Enum
        const std::string BackgroundImagePath(const PopupImage::Enum IMAGE) const;

        // The Fade color is not black so that the stage can still be seen in the background
        static sf::Color Color_Fade() { return { 0, 0, 0, 12 }; }

        static sf::Color Color_Font() { return fontColor_; }

        // speed multipliers
        static float SpeedMult_Fade() { return 500.0f; }

        // inner rects that content can appear within
        static sf::IntRect Rect_Banner() { return { 78, 32, 338, 180 }; }

        static sf::IntRect Rect_Regular() { return { 32, 32, 431, 260 }; }

        static sf::IntRect Rect_RegularSidebar() { return { 115, 32, 347, 260 }; }

        static sf::IntRect Rect_Large() { return { 47, 47, 407, 400 }; }

        static sf::IntRect Rect_LargeSidebar() { return { 85, 47, 360, 400 }; }

        static sf::IntRect Rect_Spellbook_PageLeft() { return { 165, 155, 652, 990 }; }

        static sf::IntRect Rect_Spellbook_PageRight() { return { 932, 155, 652, 990 }; }

        static sf::IntRect Rect_MusicSheet_LeftSide() { return { 116, 116, 700, 840 }; }

        static sf::IntRect Rect_MusicSheet_RightSide() { return { 816, 116, 700, 840 }; }

        // throws range_error on an unknown enum value
        const sf::IntRect Rect(const PopupImage::Enum, const float SCALE = 1.0f) const;

        // TextInfo creation helper functions
        const sfml_util::TextInfo TextInfoDefault(
            const std::string & TEXT,
            const sfml_util::Justified::Enum JUSTIFIED,
            const unsigned int FONT_SIZE) const;

        // create popup window info objects
        // use this function to make popup windows with the typical paper image backgrounds
        const PopupInfo CreatePopupInfo(
            const std::string & POPUP_NAME,
            const std::string & PROMPT_TEXT,
            const PopupButtons::Enum BUTTONS = PopupButtons::Okay,
            const PopupImage::Enum IMAGE = PopupImage::Banner,
            const sfml_util::Justified::Enum JUSTIFIED = sfml_util::Justified::Center,
            const sfml_util::sound_effect::Enum SOUND_EFFECT
            = sfml_util::sound_effect::PromptGeneric,
            const unsigned int FONT_SIZE = sfml_util::FontManager::Instance()->Size_Normal()) const;

        // use this function to create image select popup windows
        const PopupInfo CreateImageSelectionPopupInfo(
            const std::string & POPUP_NAME,
            const std::string & PROMPT_TEXT,
            const sfml_util::CachedTextureVec_t & TEXTURE_VEC,
            const std::size_t INITIAL_SELECTION = 0,
            const sfml_util::sound_effect::Enum SOUND_EFFECT
            = sfml_util::sound_effect::PromptGeneric,
            const unsigned int FONT_SIZE = sfml_util::FontManager::Instance()->Size_Normal()) const;

        // use this function to create the number selection popup window
        const PopupInfo CreateNumberSelectionPopupInfo(
            const std::string & POPUP_NAME,
            const std::string & PROMPT_TEXT,
            const std::size_t THE_MIN,
            const std::size_t THE_MAX,
            const unsigned int FONT_SIZE = sfml_util::FontManager::Instance()->Size_Normal()) const;

        // use this function to make character selection popup windows
        const PopupInfo CreateCharacterSelectPopupInfo(
            const std::string & POPUP_NAME,
            const std::string & PROMPT_TEXT,
            const std::vector<std::string> & INVALID_MSG_VEC,
            const std::size_t INITIAL_SELECTION = 0) const;

        // use this function to make image fade/transition popup windows
        const PopupInfo CreateTitleFadePopupInfo(
            const std::string & POPUP_NAME,
            const creature::CreaturePtr_t CREATURE_PTR,
            const creature::TitlePtrOpt_t & FROM_TITLE_PTR_OPT,
            const creature::TitlePtr_t TO_TITLE_PTR) const;

        // use this function to make the spellbook popup window
        const PopupInfo CreateSpellbookPopupInfo(
            const std::string & POPUP_NAME,
            const creature::CreaturePtr_t CREATURE_PTR,
            const std::size_t INITIAL_SELECTION) const;

        // use this function to make the music sheet popup window
        const PopupInfo CreateMusicPopupInfo(
            const std::string & POPUP_NAME,
            const creature::CreaturePtr_t CREATURE_PTR,
            const std::size_t INITIAL_SELECTION) const;

        // use this function to make the CombatOver popup window
        const PopupInfo CreateCombatOverPopupInfo(
            const std::string & POPUP_NAME, const combat::CombatEnd::Enum HOW_COMBAT_ENDED) const;

        // use this function t make the system error popup window
        const PopupInfo CreateSystemErrorPopupInfo(
            const std::string & POPUP_NAME,
            const std::string & GENERAL_ERROR_MSG,
            const std::string & TECH_ERROR_MSG,
            const std::string & TITLE_MSG = "") const;

        const PopupInfo CreateInventoryPromptPopupInfo(
            const std::string & POPUP_NAME,
            const std::string & PROMPT_TEXT,
            const PopupButtons::Enum BUTTONS = PopupButtons::Okay,
            const PopupImage::Enum IMAGE = PopupImage::Banner,
            const sfml_util::Justified::Enum JUSTIFIED = sfml_util::Justified::Center,
            const sfml_util::sound_effect::Enum SOUND_EFFECT
            = sfml_util::sound_effect::PromptGeneric,
            const bool WILL_INCLUDE_ITEMS = false,
            const unsigned int FONT_SIZE = sfml_util::FontManager::Instance()->Size_Normal()) const;

        const PopupInfo CreateItemProfilePleaseWaitPopupInfo(const std::string & POPUP_NAME) const;

        //'KeepAlive' popups only stay on the screen for as long as is specified.
        const PopupInfo CreateKeepAlivePopupInfo(
            const std::string & POPUP_NAME,
            const std::string & PROMPT_TEXT,
            const float KEEP_ALIVE_SECONDS,
            const unsigned int FONT_SIZE,
            const PopupButtons::Enum BUTTONS = PopupButtons::None,
            const PopupImage::Enum IMAGE = PopupImage::Banner,
            const sfml_util::sound_effect::Enum SOUND_EFFECT
            = sfml_util::sound_effect::PromptGeneric) const;

        const PopupInfo CreateTrapPopupInfo(
            const std::string & POPUP_NAME,
            const std::string & TRAP_DESCRIPTION,
            const sfml_util::sound_effect::Enum SOUND_EFFECT) const;

        sfml_util::CachedTexture LoadRandomAccentImage() const;

        void LoadAccentImagePaths();

    private:
        static std::string windowTextureDirectoryPath_;
        static std::string accentTextureDirectoryPath_;
        static sf::Color fontColor_;
        static std::unique_ptr<PopupManager> instanceUPtr_;
        //
        std::vector<std::string> accentPaths_;
    };

} // namespace popup
} // namespace heroespath

#endif // HEROESPATH_POPUP_POPUPMANAGER_HPP_INCLUDED
