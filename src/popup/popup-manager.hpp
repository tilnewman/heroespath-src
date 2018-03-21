///////////////////////////////////////////////////////////////////////////////
//
// Heroes' Path - Open-source, non-commercial, simple, game in the RPG style.
// Copyright (C) 2017 Ziesche Til Newman (tilnewman@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from
// the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//     claim that you wrote the original software.  If you use this software
//     in a product, an acknowledgment in the product documentation would be
//     appreciated but is not required.
//
//  2. Altered source versions must be plainly marked as such, and must not
//     be misrepresented as being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef HEROESPATH_POPUP_POPUPMANAGER_HPP_INCLUDED
#define HEROESPATH_POPUP_POPUPMANAGER_HPP_INCLUDED
//
// popup-manager.hpp
//  Code to load and store popup window textures.
//
#include "sfml-util/justified-enum.hpp"
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sound-effects-enum.hpp"

#include "popup/i-popup-callback.hpp"
#include "popup/popup-enums.hpp"
#include "popup/popup-info.hpp"

#include "combat/combat-over-enum.hpp"

#include <boost/filesystem.hpp>

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace heroespath
{
namespace sfml_util
{
    namespace gui
    {
        namespace box
        {
            class Info;
        }
    } // namespace gui
} // namespace sfml_util

namespace creature
{
    class Creature;
    using CreaturePtr = Creature *;

    class Title;
    using TitlePtr_t = Title *;
} // namespace creature

namespace popup
{

    using PathVec_t = std::vector<boost::filesystem::path>;

    // A class that loads, stores, and distributes fonts by style.
    class PopupManager
    {
    public:
        PopupManager(const PopupManager &) = delete;
        PopupManager(const PopupManager &&) = delete;
        PopupManager & operator=(const PopupManager &) = delete;
        PopupManager & operator=(const PopupManager &&) = delete;

        PopupManager();
        ~PopupManager();

        static PopupManager * Instance();
        static void Acquire();
        static void Release();

        static void SetTexturesDirectoryPaths(
            const std::string & WINDOWS_PATH, const std::string & ACCENTS_PATH);

        bool Test();

        // throws range error on invalid PopupImage::Enum
        const std::string BackgroundImagePath(const PopupImage::Enum IMAGE) const;

        // colors
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
        const sfml_util::gui::TextInfo TextInfoDefault(
            const std::string & TEXT,
            const sfml_util::Justified::Enum JUSTIFIED = sfml_util::Justified::Center,
            const unsigned int FONT_SIZE = sfml_util::FontManager::Instance()->Size_Large()) const;

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

        // use this function to create popup windows with a simple box and custom background color
        const PopupInfo CreateBoxedPopupInfo(
            const std::string & POPUP_NAME,
            const std::string & PROMPT_TEXT,
            const sf::Color & TEXT_COLOR,
            const sfml_util::gui::box::Info & BOX_INFO,
            const PopupButtons::Enum BUTTONS = PopupButtons::Okay,
            const sfml_util::Justified::Enum JUSTIFIED = sfml_util::Justified::Center,
            const sfml_util::sound_effect::Enum SOUND_EFFECT
            = sfml_util::sound_effect::PromptGeneric,
            const unsigned int FONT_SIZE
            = sfml_util::FontManager::Instance()->Size_Smallish()) const;

        // use this function to create image select popup windows
        const PopupInfo CreateImageSelectionPopupInfo(
            const std::string & POPUP_NAME,
            const std::string & PROMPT_TEXT,
            const sfml_util::TextureVec_t & TEXTURE_VEC,
            const bool ARE_IMAGES_CREATURES,
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
        const PopupInfo CreateImageFadePopupInfo(
            const std::string & POPUP_NAME,
            const creature::CreaturePtr_t CREATURE_PTR,
            const creature::TitlePtr_t FROM_TITLE_PTR,
            const creature::TitlePtr_t TO_TITLE_PTR,
            const sf::Texture * const FROM_IMAGE_PTR,
            const sf::Texture * const TO_IMAGE_PTR) const;

        // use this function to make the spellbook popup window
        const PopupInfo CreateSpellbookPopupInfo(
            const std::string & POPUP_NAME,
            const creature::CreaturePtr_t CREATURE_CPTR,
            const std::size_t INITIAL_SELECTION) const;

        // use this function to make the music sheet popup window
        const PopupInfo CreateMusicPopupInfo(
            const std::string & POPUP_NAME,
            const creature::CreaturePtr_t CREATURE_CPTR,
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

        void LoadRandomAccentImage(sf::Texture &) const;

        void LoadAccentImagePaths();

    private:
        static std::string windowTextureDirectoryPath_;
        static std::string accentTextureDirectoryPath_;
        static sf::Color fontColor_;
        static std::unique_ptr<PopupManager> instanceUPtr_;
        //
        PathVec_t accentPathsVec_;
    };
} // namespace popup
} // namespace heroespath

#endif // HEROESPATH_POPUP_POPUPMANAGER_HPP_INCLUDED
