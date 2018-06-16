// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
//
// main-menu-buttons.cpp
//
#include "main-menu-buttons.hpp"

#include "game/game-data-file.hpp"
#include "game/loop-manager.hpp"
#include "misc/filesystem-helpers.hpp"
#include "sfml-util/gui/text-info.hpp"

#include <string>

namespace heroespath
{
namespace sfml_util
{
    namespace main_menu_buttons
    {

        CreateCharactersButton::CreateCharactersButton(
            const float POS_LEFT, const float POS_TOP, const float SCALE, const bool WILL_BOX)
            : FourStateButton("CreateCharacters")
        {
            auto const BUTTONS_PATH_STR{ game::GameDataFile::Instance()->GetMediaPath(
                "media-images-buttons-mainmenu-dir") };

            FourStateButton::Setup(
                POS_LEFT,
                POS_TOP,
                misc::filesystem::CompletePath(BUTTONS_PATH_STR, "create_char_button_normal.png"),
                "",
                misc::filesystem::CompletePath(BUTTONS_PATH_STR, "create_char_button_lit.png"),
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
            const float POS_LEFT, const float POS_TOP, const float SCALE, const bool WILL_BOX)
            : FourStateButton("Resume")
        {
            auto const BUTTONS_PATH_STR{ game::GameDataFile::Instance()->GetMediaPath(
                "media-images-buttons-mainmenu-dir") };

            FourStateButton::Setup(
                POS_LEFT,
                POS_TOP,
                misc::filesystem::CompletePath(BUTTONS_PATH_STR, "resume_button_normal.png"),
                "",
                misc::filesystem::CompletePath(BUTTONS_PATH_STR, "resume_button_lit.png"),
                misc::filesystem::CompletePath(BUTTONS_PATH_STR, "resume_button_faded.png"),
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
            const float POS_LEFT, const float POS_TOP, const float SCALE, const bool WILL_BOX)
            : FourStateButton("StartGame")
        {
            auto const BUTTONS_PATH_STR{ game::GameDataFile::Instance()->GetMediaPath(
                "media-images-buttons-mainmenu-dir") };

            FourStateButton::Setup(
                POS_LEFT,
                POS_TOP,
                misc::filesystem::CompletePath(BUTTONS_PATH_STR, "startgame_button_faded.png"),
                "",
                "",
                "",
                sfml_util::gui::MouseTextInfo(),
                sfml_util::gui::TextInfo(),
                WILL_BOX,
                SCALE);
        }

        void StartGameButton::OnClick(const sf::Vector2f &) {}

        SettingsButton::SettingsButton(
            const float POS_LEFT, const float POS_TOP, const float SCALE, const bool WILL_BOX)
            : FourStateButton("Settings")
        {
            auto const BUTTONS_PATH_STR{ game::GameDataFile::Instance()->GetMediaPath(
                "media-images-buttons-mainmenu-dir") };

            FourStateButton::Setup(
                POS_LEFT,
                POS_TOP,
                misc::filesystem::CompletePath(BUTTONS_PATH_STR, "settings_button_normal.png"),
                "",
                misc::filesystem::CompletePath(BUTTONS_PATH_STR, "settings_button_lit.png"),
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
            const float POS_LEFT, const float POS_TOP, const float SCALE, const bool WILL_BOX)
            : FourStateButton("Credits")
        {
            auto const BUTTONS_PATH_STR{ game::GameDataFile::Instance()->GetMediaPath(
                "media-images-buttons-mainmenu-dir") };

            FourStateButton::Setup(
                POS_LEFT,
                POS_TOP,
                misc::filesystem::CompletePath(BUTTONS_PATH_STR, "credits_button_normal.png"),
                "",
                misc::filesystem::CompletePath(BUTTONS_PATH_STR, "credits_button_lit.png"),
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
            const float POS_LEFT, const float POS_TOP, const float SCALE, const bool WILL_BOX)
            : FourStateButton("Exit")
        {
            auto const BUTTONS_PATH_STR{ game::GameDataFile::Instance()->GetMediaPath(
                "media-images-buttons-mainmenu-dir") };

            FourStateButton::Setup(
                POS_LEFT,
                POS_TOP,
                misc::filesystem::CompletePath(BUTTONS_PATH_STR, "exit_button_normal.png"),
                "",
                misc::filesystem::CompletePath(BUTTONS_PATH_STR, "exit_button_lit.png"),
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
            const float POS_LEFT, const float POS_TOP, const float SCALE, const bool WILL_BOX)
            : FourStateButton("Back")
        {
            auto const BUTTONS_PATH_STR{ game::GameDataFile::Instance()->GetMediaPath(
                "media-images-buttons-mainmenu-dir") };

            FourStateButton::Setup(
                POS_LEFT,
                POS_TOP,
                misc::filesystem::CompletePath(BUTTONS_PATH_STR, "back_button_normal.png"),
                "",
                misc::filesystem::CompletePath(BUTTONS_PATH_STR, "back_button_lit.png"),
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
            const float POS_LEFT, const float POS_TOP, const float SCALE, const bool WILL_BOX)
            : FourStateButton("Delete")
        {
            auto const BUTTONS_PATH_STR{ game::GameDataFile::Instance()->GetMediaPath(
                "media-images-buttons-mainmenu-dir") };

            FourStateButton::Setup(
                POS_LEFT,
                POS_TOP,
                misc::filesystem::CompletePath(BUTTONS_PATH_STR, "delete_button_normal.png"),
                "",
                misc::filesystem::CompletePath(BUTTONS_PATH_STR, "delete_button_lit.png"),
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

    } // namespace main_menu_buttons
} // namespace sfml_util
} // namespace heroespath
