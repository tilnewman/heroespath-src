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
#ifndef GAME_SETTINGSSTAGE_INCLUDED
#define GAME_SETTINGSSTAGE_INCLUDED
//
// settings-stage.hpp
//  A Stage class that allows changing system and game settings.
//
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sfml-system.hpp"
#include "sfml-util/stage.hpp"
#include "sfml-util/display.hpp"
#include "sfml-util/sliderbar-music.hpp"
#include "sfml-util/sliderbar-effects.hpp"
#include "sfml-util/i-callback-handler.hpp"
#include "sfml-util/gui/text-region.hpp"
#include "sfml-util/gui/box.hpp"
#include "sfml-util/gui/radio-button.hpp"

#include "game/main-menu-buttons.hpp"
#include "game/horiz-symbol.hpp"
#include "game/i-popup-callback.hpp"
#include "game/main-menu-title.hpp"

#include <memory>
#include <string>


namespace game
{
namespace stage
{

    //A simple class that displays the system and game settings
    class SettingsStage
    :
        public sfml_util::Stage,
        public game::callback::IPopupHandler_t,
        public sfml_util::callback::IRadioButtonSetCallbackHandler_t
    {
        //prevent copy construction
        SettingsStage(const SettingsStage &) =delete;

        //prevent copy assignment
        SettingsStage & operator=(const SettingsStage &) =delete;

    public:
        SettingsStage();
        virtual ~SettingsStage();

        //required by callback handler
        inline virtual const std::string HandlerName() const { return GetStageName(); }
        virtual bool HandleCallback(const sfml_util::callback::RadioButtonCallbackPackage_t &);
        virtual bool HandleCallback(const game::callback::PopupResponse &);

        virtual void Setup();
        virtual void Draw(sf::RenderTarget & target, const sf::RenderStates &);
        virtual void HandleResolutionChange();
        virtual bool KeyRelease(const sf::Event::KeyEvent &);
        float GetBackButtonPosLeft() const;

    private:
        void ResetPositions();
        //
        static const float BOX_WIDTH_;
        static const float BOX_HEIGHT_;
        static const float BOX_INNER_PAD_;
        static const float MUSIC_VOLUME_HORIZ_POS_;
        static const float SOUNDEFFECTS_VOLUME_HORIZ_POS_;
        static const float ANTIALIAS_HORIZ_POS_;
        //
        float                                     boxTop_;
        sfml_util::gui::BackgroundImage           backgroundImage_;
        MainMenuTitle                             mainMenuTitle_;
        sfml_util::gui::box::Box                  box_;
        sfml_util::gui::RadioButtonSetSPtr_t      resRadioButtonSetSPtr_;
        sfml_util::gui::TextRegionUPtr_t          resolutionTextRegionUPtr_;
        sfml_util::gui::TextRegionUPtr_t          musicVolumeTextRegionUPtr_;
        sfml_util::SliderBarLabeled_MusicSPtr_t   musicVolumeSliderBarSPtr_;
        sfml_util::gui::TextRegionUPtr_t          effectsVolumeTextRegionUPtr_;
        sfml_util::SliderBarLabeled_EffectsSPtr_t effectsVolumeSliderBarSPtr_;
        main_menu_buttons::BackButtonSPtr_t       backButtonSPtr_;
        sfml_util::gui::TextRegionUPtr_t          revisionTextRegionUPtr_;
        sfml_util::gui::RadioButtonSetSPtr_t      aaRadioButtonSetSPtr_;
        sfml_util::gui::TextRegionUPtr_t          aaTextRegionUPtr_;
        unsigned                                  prevAALevel_;
        BottomSymbol bottomSymbol_;
        sfml_util::gui::TextRegionUPtr_t musicInfoLabelTextRegionUPtr_;
        sfml_util::gui::TextRegionUPtr_t musicInfoTextRegionUPtr_;
    };

}
}
#endif //GAME_SETTINGSSTAGE_INCLUDED
