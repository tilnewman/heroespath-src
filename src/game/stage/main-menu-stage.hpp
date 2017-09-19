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
#ifndef GAME_MAINMENUSTAGE_INCLUDED
#define GAME_MAINMENUSTAGE_INCLUDED
//
// credits-stage.hpp
//  A Stage class that displays the rolling credits of the app.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sfml-system.hpp"
#include "sfml-util/stage.hpp"
#include "sfml-util/gui/box.hpp"
#include "sfml-util/gradient.hpp"

#include "game/horiz-symbol.hpp"
#include "game/ouroboros.hpp"
#include "game/main-menu-buttons.hpp"
#include "popup/i-popup-callback.hpp"
#include "sfml-util/gui/background-image.hpp"

#include <memory>
#include <string>


namespace game
{
namespace stage
{

    //A simple class that displays the credits
    class MainMenuStage
    :
        public sfml_util::Stage,
        public sfml_util::gui::callback::IFourStateButtonCallbackHandler_t
    {
        //prevent copy construction
        MainMenuStage(const MainMenuStage &) =delete;

        //prevent copy assignment
        MainMenuStage & operator=(const MainMenuStage &) =delete;

    public:
        MainMenuStage();
        virtual ~MainMenuStage();

        inline virtual const std::string HandlerName() const { return GetStageName(); }
        virtual bool HandleCallback(const sfml_util::gui::callback::FourStateButtonCallbackPackage_t &);

        virtual void Setup();
        virtual void Draw(sf::RenderTarget & target, const sf::RenderStates & STATES);
        virtual bool KeyRelease(const sf::Event::KeyEvent &);

    private:
        const float SCREEN_WIDTH_;
        const float SCREEN_HEIGHT_;
        const sf::FloatRect SCREEN_RECT_;
        const float BUTTON_SCALE_;
        //
        sf::Texture                                     titleTexture_;
        sf::Sprite                                      titleSprite_;
        sfml_util::GradientRect                         gradient_;
        main_menu_buttons::ResumeButtonSPtr_t           resumeButtonSPtr_;
        main_menu_buttons::CreateCharactersButtonSPtr_t createButtonSPtr_;
        main_menu_buttons::SettingsButtonSPtr_t         settingsButtonSPtr_;
        main_menu_buttons::CreditsButtonSPtr_t          creditsButtonSPtr_;
        main_menu_buttons::ExitButtonSPtr_t             exitButtonSPtr_;
        OuroborosUPtr_t                                 ouroborosUPtr_;
        BottomSymbol                                    bottomSymbol_;
        sfml_util::gui::BackgroundImage                 backgroundImage_;
    };

    using MainMenuStageSPtr_t = std::shared_ptr<MainMenuStage>;

}
}
#endif //GAME_MAINMENUSTAGE_INCLUDED
