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
#ifndef HEROESPATH_SETTINGSSTAGE_HPP_INCLUDED
#define HEROESPATH_SETTINGSSTAGE_HPP_INCLUDED
//
// settings-stage.hpp
//  A Stage class that allows changing system and game settings.
//
#include "sfml-util/display.hpp"
#include "sfml-util/gui/box.hpp"
#include "sfml-util/gui/radio-button.hpp"
#include "sfml-util/gui/text-region.hpp"
#include "sfml-util/i-callback-handler.hpp"
#include "sfml-util/sfml-graphics.hpp"
#include "sfml-util/sfml-system.hpp"
#include "sfml-util/sliderbar-effects.hpp"
#include "sfml-util/sliderbar-music.hpp"
#include "sfml-util/stage.hpp"

#include "popup/i-popup-callback.hpp"
#include "sfml-util/horiz-symbol.hpp"
#include "sfml-util/main-menu-buttons.hpp"
#include "sfml-util/main-menu-title.hpp"

#include <memory>
#include <string>

namespace heroespath
{
namespace stage
{

    // A simple class that displays the system and game settings
    class SettingsStage
        : public sfml_util::Stage
        , public popup::IPopupHandler_t
        , public sfml_util::callback::IRadioButtonSetCallbackHandler_t
    {
    public:
        SettingsStage(const SettingsStage &) = delete;
        SettingsStage(SettingsStage &&) = delete;
        SettingsStage & operator=(const SettingsStage &) = delete;
        SettingsStage & operator=(SettingsStage &&) = delete;

    public:
        SettingsStage();
        virtual ~SettingsStage();

        virtual const std::string HandlerName() const { return GetStageName(); }
        virtual bool HandleCallback(const sfml_util::callback::RadioButtonCallbackPackage_t &);
        virtual bool HandleCallback(const popup::PopupResponse &);

        virtual void Setup();
        virtual void Draw(sf::RenderTarget & target, const sf::RenderStates &);
        virtual void HandleResolutionChange();
        virtual bool KeyRelease(const sf::Event::KeyEvent &);

    private:
        void Setup_StageRegionToFullscreen();
        void Setup_BackgroundImage();
        void Setup_MainMenuTitle();
        void Setup_BottomSymbol();
        void Setup_BackButton();
        const sf::FloatRect Setup_WoodBackgroundBoxAndReturnInnerRect();
        void Setup_ResolutionChangeLabel(const sf::FloatRect & BG_BOX_INNER_RECT);
        void Setup_ResolutionChangeRadioButtonSet(const sf::FloatRect & BG_BOX_INNER_RECT);
        void Setup_AntiAliasLabel(const sf::FloatRect & BG_BOX_INNER_RECT);
        void Setup_AntiAliasRadioButtonSet(const sf::FloatRect & BG_BOX_INNER_RECT);
        void Setup_MusicVolumeLabel(const sf::FloatRect & BG_BOX_INNER_RECT);
        void Setup_MusicVolumeSlider(const sf::FloatRect & BG_BOX_INNER_RECT);
        void Setup_SfxVolumeLabel(const sf::FloatRect & BG_BOX_INNER_RECT);
        void Setup_SfxVolumeSlider(const sf::FloatRect & BG_BOX_INNER_RECT);
        void Setup_MusicInfoLabel(const sf::FloatRect & BG_BOX_INNER_RECT);
        void Setup_MusicInfoBox(const sf::FloatRect & BG_BOX_INNER_RECT);
        void Setup_RevisionNumber(const sf::FloatRect & BG_BOX_INNER_RECT);

        const sfml_util::gui::TextInfo CreateLabelTextInfo(const std::string & TITLE) const;

        const sfml_util::gui::MouseTextInfo CreateSliderbarValueTextInfoSet() const;

        float HorizPositionOfColumn(
            const std::size_t COLUMN_NUM, const sf::FloatRect & BG_BOX_INNER_RECT);

        float SliderLabelVertPad() const;

    private:
        const float SLIDER_LENGTH_VERT_;
        bool hasStageAlreadyBeenSetup_;
        unsigned prevAALevel_;
        sfml_util::gui::BackgroundImage bgTileImage_;
        sfml_util::MainMenuTitle mainMenuTitle_;
        sfml_util::BottomSymbol bottomSymbol_;
        sfml_util::main_menu_buttons::BackButtonUPtr_t backButtonUPtr_;
        sfml_util::gui::box::Box bgBox_;
        sfml_util::gui::TextRegionUPtr_t resLabelTextRegionUPtr_;
        sfml_util::gui::RadioButtonSetUPtr_t resRadioButtonSetUPtr_;
        sfml_util::gui::TextRegionUPtr_t aaLabelTextRegionUPtr_;
        sfml_util::gui::RadioButtonSetUPtr_t aaRadioButtonSetUPtr_;
        sfml_util::gui::TextRegionUPtr_t musicVolLabelTextRegionUPtr_;
        sfml_util::SliderBarLabeled_MusicUPtr_t musicVolSliderBarUPtr_;
        sfml_util::gui::TextRegionUPtr_t effectsVolLabelTextRegionUPtr_;
        sfml_util::SliderBarLabeled_EffectsUPtr_t effectsVolSliderBarUPtr_;
        sfml_util::gui::TextRegionUPtr_t musicInfoLabelTextRegionUPtr_;
        sfml_util::gui::TextRegionUPtr_t musicInfoDetailsTextRegionUPtr_;
        sfml_util::gui::TextRegionUPtr_t revLabelTextRegionUPtr_;
        sf::Texture woodTexture_;
    };
} // namespace stage
} // namespace heroespath

#endif // HEROESPATH_SETTINGSSTAGE_HPP_INCLUDED
