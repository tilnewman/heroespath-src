// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
//
// main-menu-buttons.cpp
//
#include "main-menu-buttons.hpp"

#include "game/game-data-file.hpp"
#include "game/loop-manager.hpp"

#include "sfml-util/gui/text-info.hpp"

#include <string>


namespace heroespath
{
namespace sfml_util
{
namespace main_menu_buttons
{

    CreateCharactersButton::CreateCharactersButton(
        const float POS_LEFT,
        const float POS_TOP,
        const float SCALE,
        const bool WILL_BOX )
    :
        FourStateButton("CreateCharacters")
    {
        auto const BUTTONS_PATH{
            game::GameDataFile::Instance()->
                GetMediaPath("media-images-buttons-mainmenu-dir") };

        FourStateButton::Setup(
            POS_LEFT,
            POS_TOP,
            std::string(BUTTONS_PATH).append("create_char_button_normal.png"),
            "",
            std::string(BUTTONS_PATH).append("create_char_button_lit.png"),
            "",
            sfml_util::gui::MouseTextInfo(),
            sfml_util::gui::TextInfo(),
            WILL_BOX,
            SCALE);
    }

    void CreateCharactersButton::OnClick(const sf::Vector2f &)
    {
        game::LoopManager::Instance()->TransitionTo_CharacterCreation();
    }


    ResumeButton::ResumeButton(
        const float POS_LEFT,
        const float POS_TOP,
        const float SCALE,
        const bool  WILL_BOX)
    :
        FourStateButton("Resume")
    {
        auto const BUTTONS_PATH{
            game::GameDataFile::Instance()->
                GetMediaPath("media-images-buttons-mainmenu-dir") };

        FourStateButton::Setup(
            POS_LEFT,
            POS_TOP,
            std::string(BUTTONS_PATH).append("resume_button_normal.png"),
            "",
            std::string(BUTTONS_PATH).append("resume_button_lit.png"),
            std::string(BUTTONS_PATH).append("resume_button_faded.png"),
            sfml_util::gui::MouseTextInfo(),
            sfml_util::gui::TextInfo(),
            WILL_BOX,
            SCALE);
    }

    void ResumeButton::OnClick(const sf::Vector2f & MOUSE_POS_V)
    {
        FourStateButton::OnClick(MOUSE_POS_V);
    }


    StartGameButton::StartGameButton(
        const float POS_LEFT,
        const float POS_TOP,
        const float SCALE,
        const bool  WILL_BOX)
    :
        FourStateButton("StartGame")
    {
        auto const BUTTONS_PATH{
            game::GameDataFile::Instance()->
                GetMediaPath("media-images-buttons-mainmenu-dir") };

        FourStateButton::Setup(
            POS_LEFT,
            POS_TOP,
            std::string(BUTTONS_PATH).append("startgame_button_faded.png"),
            "",
            "",
            "",
            sfml_util::gui::MouseTextInfo(),
            sfml_util::gui::TextInfo(),
            WILL_BOX,
            SCALE);
    }

    void StartGameButton::OnClick(const sf::Vector2f &)
    {}


    SettingsButton::SettingsButton(
        const float POS_LEFT,
        const float POS_TOP,
        const float SCALE,
        const bool  WILL_BOX)
    :
        FourStateButton("Settings")
    {
        auto const BUTTONS_PATH{
            game::GameDataFile::Instance()->
                GetMediaPath("media-images-buttons-mainmenu-dir") };

        FourStateButton::Setup(
            POS_LEFT,
            POS_TOP,
            std::string(BUTTONS_PATH).append("settings_button_normal.png"),
            "",
            std::string(BUTTONS_PATH).append("settings_button_lit.png"),
            "",
            sfml_util::gui::MouseTextInfo(),
            sfml_util::gui::TextInfo(),
            WILL_BOX,
            SCALE);
    }

    void SettingsButton::OnClick(const sf::Vector2f &)
    {
        game::LoopManager::Instance()->TransitionTo_Settings();
    }


    CreditsButton::CreditsButton(
        const float POS_LEFT,
        const float POS_TOP,
        const float SCALE,
        const bool  WILL_BOX)
    :
        FourStateButton("Credits")
    {
        auto const BUTTONS_PATH{
            game::GameDataFile::Instance()->
                GetMediaPath("media-images-buttons-mainmenu-dir") };

        FourStateButton::Setup(
            POS_LEFT,
            POS_TOP,
            std::string(BUTTONS_PATH).append("credits_button_normal.png"),
            "",
            std::string(BUTTONS_PATH).append("credits_button_lit.png"),
            "",
            sfml_util::gui::MouseTextInfo(),
            sfml_util::gui::TextInfo(),
            WILL_BOX,
            SCALE);
    }

    void CreditsButton::OnClick(const sf::Vector2f &)
    {
        game::LoopManager::Instance()->TransitionTo_Credits();
    }


    ExitButton::ExitButton(
        const float POS_LEFT,
        const float POS_TOP,
        const float SCALE,
        const bool  WILL_BOX)
    :
        FourStateButton("Exit")
    {
        auto const BUTTONS_PATH{
            game::GameDataFile::Instance()->
                GetMediaPath("media-images-buttons-mainmenu-dir") };

        FourStateButton::Setup(
            POS_LEFT,
            POS_TOP,
            std::string(BUTTONS_PATH).append("exit_button_normal.png"),
            "",
            std::string(BUTTONS_PATH).append("exit_button_lit.png"),
            "",
            sfml_util::gui::MouseTextInfo(),
            sfml_util::gui::TextInfo(),
            WILL_BOX,
            SCALE);
    }

    void ExitButton::OnClick(const sf::Vector2f &)
    {
       game::LoopManager::Instance()->TransitionTo_Exit();
    }



    BackButton::BackButton(
        const float POS_LEFT,
        const float POS_TOP,
        const float SCALE,
        const bool  WILL_BOX)
    :
        FourStateButton("Back")
    {
        auto const BUTTONS_PATH{
            game::GameDataFile::Instance()->
                GetMediaPath("media-images-buttons-mainmenu-dir") };

        FourStateButton::Setup(
            POS_LEFT,
            POS_TOP,
            std::string(BUTTONS_PATH).append("back_button_normal.png"),
            "",
            std::string(BUTTONS_PATH).append("back_button_lit.png"),
            "",
            sfml_util::gui::MouseTextInfo(),
            sfml_util::gui::TextInfo(),
            WILL_BOX,
            SCALE);
    }

    void BackButton::OnClick(const sf::Vector2f &)
    {
        game::LoopManager::Instance()->TransitionTo_Previous();
    }


    DeleteButton::DeleteButton(
        const float POS_LEFT,
        const float POS_TOP,
        const float SCALE,
        const bool  WILL_BOX)
    :
        FourStateButton("Delete")
    {
        auto const BUTTONS_PATH{
            game::GameDataFile::Instance()->
                GetMediaPath("media-images-buttons-mainmenu-dir") };

        FourStateButton::Setup(
            POS_LEFT,
            POS_TOP,
            std::string(BUTTONS_PATH).append("delete_button_normal.png"),
            "",
            std::string(BUTTONS_PATH).append("delete_button_lit.png"),
            "",
            sfml_util::gui::MouseTextInfo(),
            sfml_util::gui::TextInfo(),
            WILL_BOX,
            SCALE);
    }

    void DeleteButton::OnClick(const sf::Vector2f &)
    {
        game::LoopManager::Instance()->TransitionTo_Previous();
    }

}
}
}
