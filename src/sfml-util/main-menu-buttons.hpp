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

            virtual ~ResumeButton() {}

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

            virtual ~StartGameButton() {}

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

            virtual ~CreateCharactersButton() {}

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

            virtual ~SettingsButton() {}

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

            virtual ~CreditsButton() {}

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

            virtual ~ExitButton() {}

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

            virtual ~BackButton() {}

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

            virtual ~DeleteButton() {}

        protected:
            virtual void OnClick(const sf::Vector2f &);
        };

        using DeleteButtonUPtr_t = std::unique_ptr<DeleteButton>;
    }
}
}

#endif // HEROESPATH_SFMLUTIL_MAINMENUBUTTONS_HPP_INCLUDED
