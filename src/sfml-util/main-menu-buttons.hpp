// ----------------------------------------------------------------------------
// "THE BEER-WARE LICENSE" (Revision 42):
// <ztn@zurreal.com> wrote this file.  As long as you retain this notice you
// can do whatever you want with this stuff. If we meet some day, and you think
// this stuff is worth it, you can buy me a beer in return.  Ziesche Til Newman
// ----------------------------------------------------------------------------
#ifndef HEROESPATH_SFMLUTIL_MAINMENUBUTTONS_HPP_INCLUDED
#define HEROESPATH_SFMLUTIL_MAINMENUBUTTONS_HPP_INCLUDED
//
// main-menu-buttons.hpp
//  A set of FourStateButton classes for the heroespath main menu.
//
#include "sfml-util/gui/four-state-button.hpp"

namespace heroespath
{
namespace sfml_util
{
    namespace main_menu_buttons
    {

        class ResumeButton : public sfml_util::gui::FourStateButton
        {
        public:
            ResumeButton(
                const float POS_LEFT,
                const float POS_TOP,
                const float SCALE = 1.0f,
                const bool WILL_BOX = false);

            virtual ~ResumeButton() = default;

        protected:
            virtual void OnClick(const sf::Vector2f &);
        };

        using ResumeButtonUPtr_t = std::unique_ptr<ResumeButton>;

        class StartGameButton : public sfml_util::gui::FourStateButton
        {
        public:
            StartGameButton(
                const float POS_LEFT,
                const float POS_TOP,
                const float SCALE = 1.0f,
                const bool WILL_BOX = false);

            virtual ~StartGameButton() = default;

        protected:
            virtual void OnClick(const sf::Vector2f &);
        };

        using StartGameButtonUPtr_t = std::unique_ptr<StartGameButton>;

        class CreateCharactersButton : public sfml_util::gui::FourStateButton
        {
        public:
            CreateCharactersButton(
                const float POS_LEFT,
                const float POS_TOP,
                const float SCALE = 1.0f,
                const bool WILL_BOX = false);

            virtual ~CreateCharactersButton() = default;

        protected:
            virtual void OnClick(const sf::Vector2f &);
        };

        using CreateCharactersButtonUPtr_t = std::unique_ptr<CreateCharactersButton>;

        class SettingsButton : public sfml_util::gui::FourStateButton
        {
        public:
            SettingsButton(
                const float POS_LEFT,
                const float POS_TOP,
                const float SCALE = 1.0f,
                const bool WILL_BOX = false);

            virtual ~SettingsButton() = default;

        protected:
            virtual void OnClick(const sf::Vector2f &);
        };

        using SettingsButtonUPtr_t = std::unique_ptr<SettingsButton>;

        class CreditsButton : public sfml_util::gui::FourStateButton
        {
        public:
            CreditsButton(
                const float POS_LEFT,
                const float POS_TOP,
                const float SCALE = 1.0f,
                const bool WILL_BOX = false);

            virtual ~CreditsButton() = default;

        protected:
            virtual void OnClick(const sf::Vector2f &);
        };

        using CreditsButtonUPtr_t = std::unique_ptr<CreditsButton>;

        class ExitButton : public sfml_util::gui::FourStateButton
        {
        public:
            ExitButton(
                const float POS_LEFT,
                const float POS_TOP,
                const float SCALE = 1.0f,
                const bool WILL_BOX = false);

            virtual ~ExitButton() = default;

        protected:
            virtual void OnClick(const sf::Vector2f &);
        };

        using ExitButtonUPtr_t = std::unique_ptr<ExitButton>;

        class BackButton : public sfml_util::gui::FourStateButton
        {
        public:
            BackButton(
                const float POS_LEFT,
                const float POS_TOP,
                const float SCALE = 1.0f,
                const bool WILL_BOX = false);

            virtual ~BackButton() = default;

        protected:
            virtual void OnClick(const sf::Vector2f &);
        };

        using BackButtonUPtr_t = std::unique_ptr<BackButton>;

        class DeleteButton : public sfml_util::gui::FourStateButton
        {
        public:
            DeleteButton(
                const float POS_LEFT,
                const float POS_TOP,
                const float SCALE = 1.0f,
                const bool WILL_BOX = false);

            virtual ~DeleteButton() = default;

        protected:
            virtual void OnClick(const sf::Vector2f &);
        };

        using DeleteButtonUPtr_t = std::unique_ptr<DeleteButton>;

    } // namespace main_menu_buttons
} // namespace sfml_util
} // namespace heroespath

#endif // HEROESPATH_SFMLUTIL_MAINMENUBUTTONS_HPP_INCLUDED
