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
#ifndef HEROESPATH_MAINMENUSTAGE_HPP_INCLUDED
#define HEROESPATH_MAINMENUSTAGE_HPP_INCLUDED
//
// credits-stage.hpp
//  A Stage class that displays the rolling credits of the app.
//
#include "sfml-util/gradient.hpp"
#include "sfml-util/gui/background-image.hpp"
#include "sfml-util/gui/box.hpp"
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sfml-system.hpp"
#include "sfml-util/stage.hpp"

#include "sfml-util/horiz-symbol.hpp"
#include "sfml-util/main-menu-buttons.hpp"
#include "sfml-util/ouroboros.hpp"

#include "popup/i-popup-callback.hpp"

#include <memory>
#include <string>

namespace heroespath
{
namespace stage
{

    // Responsible for displaying and managing mouse-clicks of five menu options:
    // Resume Game, Create Characters, Settings, Credits, Exit
    class MainMenuStage
        : public sfml_util::Stage
        , public sfml_util::gui::callback::IFourStateButtonCallbackHandler_t
    {
    public:
        MainMenuStage(const MainMenuStage &) = delete;
        MainMenuStage(const MainMenuStage &&) = delete;
        MainMenuStage & operator=(const MainMenuStage &) = delete;
        MainMenuStage & operator=(const MainMenuStage &&) = delete;

    public:
        MainMenuStage();
        virtual ~MainMenuStage();

        const std::string HandlerName() const override { return GetStageName(); }

        bool HandleCallback(
            const sfml_util::gui::callback::FourStateButtonCallbackPackage_t &) override;

        void Setup() override;
        void Draw(sf::RenderTarget & target, const sf::RenderStates & STATES) override;
        bool KeyRelease(const sf::Event::KeyEvent &) override;

    private:
        const float SCREEN_WIDTH_;
        const float SCREEN_HEIGHT_;
        const sf::FloatRect SCREEN_RECT_;
        const float BUTTON_SCALE_;
        //
        sf::Texture titleTexture_;
        sf::Sprite titleSprite_;
        sfml_util::GradientRect gradient_;
        sfml_util::main_menu_buttons::ResumeButtonUPtr_t resumeButtonUPtr_;
        sfml_util::main_menu_buttons::CreateCharactersButtonUPtr_t createButtonUPtr_;
        sfml_util::main_menu_buttons::SettingsButtonUPtr_t settingsButtonUPtr_;
        sfml_util::main_menu_buttons::CreditsButtonUPtr_t creditsButtonUPtr_;
        sfml_util::main_menu_buttons::ExitButtonUPtr_t exitButtonUPtr_;
        sfml_util::OuroborosUPtr_t ouroborosUPtr_;
        sfml_util::BottomSymbol bottomSymbol_;
        sfml_util::gui::BackgroundImage backgroundImage_;
    };

    using MainMenuStageSPtr_t = std::shared_ptr<MainMenuStage>;
} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_MAINMENUSTAGE_HPP_INCLUDED
